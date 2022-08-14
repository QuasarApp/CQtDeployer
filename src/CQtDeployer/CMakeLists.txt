#
# Copyright (C) 2021-2022 QuasarApp.
# Distributed under the GPLv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

cmake_minimum_required(VERSION 3.18)

get_filename_component(CURRENT_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR} NAME)

set(CURRENT_PROJECT "${PROJECT_NAME}")
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


if (${QT_VERSION_MAJOR} EQUAL 6)

    qt_add_executable(${CURRENT_PROJECT} MANUAL_FINALIZATION ${ALL_SOURCES})

else()
    if (ANDROID)
        add_library(${CURRENT_PROJECT} ${ALL_SOURCES})
    else ()
        add_executable(${CURRENT_PROJECT} ${ALL_SOURCES} )
    endif()

endif()

target_link_libraries(${CURRENT_PROJECT} PUBLIC LibDeploy)

target_include_directories(${CURRENT_PROJECT} PUBLIC ${PUBLIC_INCUDE_DIR})
target_include_directories(${CURRENT_PROJECT} PRIVATE ${PRIVATE_INCUDE_DIR})

SET(TARGET_DIR "${CMAKE_SOURCE_DIR}/Distro")
file(MAKE_DIRECTORY ${TARGET_DIR})

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

configure_file_in(${CURRENT_PROJECT} "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/CQtDeployer.json")
configure_file_in(${CURRENT_PROJECT} "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/CQtDeployerSingle.json")

# qifw installer
configure_file_in(${CURRENT_PROJECT} "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/QIFWTemplate/packages/CQtDeployer/meta/package.xml")
configure_file_in(${CURRENT_PROJECT} "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/QIFWTemplate/packages/CQtDeployer.1_6/meta/package.xml")
configure_file_in(${CURRENT_PROJECT} "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/QIFWTemplate/packages/QIF/meta/package.xml")

# sets qifw variable
set(HomeDir "@HomeDir@")
configure_file_in(${CURRENT_PROJECT} "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/QIFWTemplate/config/config.xml")


addDeployFromCustomFile(${CURRENT_PROJECT} "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/CQtDeployer.json")

set(QIFW_PLATFORM linux)

if (WIN32)
    set(QIFW_PLATFORM windows)
endif()

ADD_CUSTOM_TARGET(
    downloadQIFW
    SOURCES ${${name}files}
    COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/QIFW.py" ${QIFW_PLATFORM} ${QIFW_VERSION} "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/QIFW"
    COMMENT python "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/QIFW.py" ${QIFW_PLATFORM} ${QIFW_VERSION} "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/QIFW"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_dependencies(deploy${CURRENT_PROJECT} downloadQIFW)

addDeployFromCustomFile(${CURRENT_PROJECT}Single "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/CQtDeployerSingle.json")
