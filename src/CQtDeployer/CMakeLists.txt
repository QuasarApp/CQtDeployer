#
# Copyright (C) 2021-2022 QuasarApp.
# Distributed under the GPLv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

cmake_minimum_required(VERSION 3.18)

get_filename_component(CURRENT_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR} NAME)

set(CURRENT_PROJECT "${PROJECT_NAME}${CURRENT_PROJECT_DIR}")
option(SIGN_APP "This option enable od disabled sign apk and aab files" ON)

file(GLOB_RECURSE SOURCE_CPP
    "*.cpp" "*.h"
)

if (${QT_VERSION_MAJOR})
    file(GLOB_RECURSE SOURCE_QRC
        "*.qrc"
    )
endif()

set(ALL_SOURCES ${SOURCE_CPP} ${SOURCE_QRC})

set(PUBLIC_INCUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(PRIVATE_INCUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Private")

updateGitVars()

if (${QT_VERSION_MAJOR} EQUAL 6)

    qt_add_executable(${CURRENT_PROJECT} MANUAL_FINALIZATION ${ALL_SOURCES})

else()
    if (ANDROID)
        add_library(${CURRENT_PROJECT} ${ALL_SOURCES})
    else ()
        add_executable(${CURRENT_PROJECT} ${ALL_SOURCES} )
    endif()

endif()

target_link_libraries(${CURRENT_PROJECT} PUBLIC ${PROJECT_NAME}Library)

target_include_directories(${CURRENT_PROJECT} PUBLIC ${PUBLIC_INCUDE_DIR})
target_include_directories(${CURRENT_PROJECT} PRIVATE ${PRIVATE_INCUDE_DIR})

SET(TARGET_DIR "${CMAKE_SOURCE_DIR}/Distro")
file(MAKE_DIRECTORY ${TARGET_DIR})

if (ANDROID)

    set(ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android CACHE INTERNAL "")
    set(ENV{ANDROID_API_VERSION} 31)
    set(ANDROID_API_VERSION $ENV{ANDROID_API_VERSION})
    set(OPENSSL_ROOT_PATH "$ENV{OPENSSL_ROOT_DIR}")

    set(CQtDeployer_EXTRA_LIBS
        ${PROJECT_NAME}Library
#        libName
        )

#    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS AndroidExtras REQUIRED)
#    target_link_libraries(${CURRENT_PROJECT} PRIVATE Qt${QT_VERSION_MAJOR}::AndroidExtras)

    if (SIGN_APP)
        message("SIGN_APP")
        addDeploySignedAPK(${CURRENT_PROJECT}
            "${CMAKE_CURRENT_LIST_DIR}/android"
            "QuasarAppProject"
            "${SIGPATH}/quasarapp.keystore"
            "${SIGPASS_QUASARAPP}"
            "${TARGET_DIR}"
            "${CQtDeployer_EXTRA_LIBS}")

    else()
        message("NO_SIGN_APP")

        addDeployAPK(${CURRENT_PROJECT}
            "${CMAKE_CURRENT_LIST_DIR}/android"
            "${TARGET_DIR}"
            "${CQtDeployer_EXTRA_LIBS}")

    endif()
    set(manifest_file "${CMAKE_CURRENT_SOURCE_DIR}/android/AndroidManifest.xml")
    configure_file("${manifest_file}.in" ${manifest_file} @ONLY)

    set(gradle_file "${CMAKE_CURRENT_SOURCE_DIR}/android/build.gradle")
    configure_file("${gradle_file}.in" ${gradle_file} @ONLY)

    file(GLOB java_files
       "android/src/com/quasarapp/androidtools/*.java"
       "android/*.xml"
       "android/*.gradle"
       "android/gradlew"
       "android/gradle.*"

    )

    add_custom_target(${name}Android
        SOURCES ${java_files}
    )

elseif(IOS)
#    set_xcode_property(${CURRENT_PROJECT} PRODUCT_BUNDLE_IDENTIFIER ${CHEATCARD_PACKAGE_ID} All)

    set_target_properties(${CURRENT_PROJECT} PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER ${CQtDeployer_PACKAGE_ID}
        MACOSX_BUNDLE_BUNDLE_VERSION ${CQtDeployer_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${CQtDeployer_VERSION}
    )
else()
    # Desctop deploying

    message(GIT_COMMIT_COUNT = ${GIT_COMMIT_COUNT})

    set(DeployFile "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/CQtDeployer.json")
    configure_file("${DeployFile}.in" ${DeployFile} @ONLY)

    addDeployFromCustomFile("CQtDeployer" ${DeployFile})

endif()

if(QT_VERSION_MAJOR EQUAL 6)
    # Add This line if your project use the Quick module
    # qt_import_qml_plugins(${CURRENT_PROJECT})
    qt_finalize_executable(${CURRENT_PROJECT})
endif()


set(LANGS ${CMAKE_CURRENT_SOURCE_DIR}/languages/en.ts
          ${CMAKE_CURRENT_SOURCE_DIR}/languages/ru.ts
          ${CMAKE_CURRENT_SOURCE_DIR}/languages/uk.ts
          ${CMAKE_CURRENT_SOURCE_DIR}/languages/ja.ts
          ${CMAKE_CURRENT_SOURCE_DIR}/languages/tr.ts
          ${CMAKE_CURRENT_SOURCE_DIR}/languages/zh.ts
          ${CMAKE_CURRENT_SOURCE_DIR}/languages/de.ts
          ${CMAKE_CURRENT_SOURCE_DIR}/languages/fr.ts
          ${CMAKE_CURRENT_SOURCE_DIR}/languages/es.ts
          ${CMAKE_CURRENT_SOURCE_DIR}/languages/pl.ts)

prepareQM(${CURRENT_PROJECT} ${CMAKE_CURRENT_SOURCE_DIR}/../ "${LANGS}")


