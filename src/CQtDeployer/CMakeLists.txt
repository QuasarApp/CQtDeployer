#
# Copyright (C) 2021-2024 QuasarApp.
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

target_link_libraries(${CURRENT_PROJECT} PUBLIC Deploy)

target_include_directories(${CURRENT_PROJECT} PUBLIC ${PUBLIC_INCUDE_DIR})
target_include_directories(${CURRENT_PROJECT} PRIVATE ${PRIVATE_INCUDE_DIR})

SET(TARGET_DIR "${CMAKE_SOURCE_DIR}/Distro")
file(MAKE_DIRECTORY ${TARGET_DIR})

if(QT_VERSION_MAJOR EQUAL 6)
    # Add This line if your project use the Quick module
    # qt_import_qml_plugins(${CURRENT_PROJECT})
    qt_finalize_executable(${CURRENT_PROJECT})
endif()


# sets qifw variable
set(ApplicationsDir "@HomeDir@")
set(QIFW_PLATFORM linux)
set(INSTALLER_SUFIX ".run")
if (WIN32)
    set(QIFW_PLATFORM windows)
    set(INSTALLER_SUFIX ".exe")
    set(ApplicationsDir "@ApplicationsDir@")

endif()

set(BASE_FULL_NAME "${CURRENT_PROJECT}_${CQT_DEPLOYER_VERSION}_${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}")
set(INSTALLER_FULL_NAME "${BASE_FULL_NAME}${INSTALLER_SUFIX}")

if(DEFINED ENV{GITHUB_ENV})
    file(APPEND $ENV{GITHUB_ENV} "INSTALLER_FULL_NAME="${INSTALLER_FULL_NAME}"\n")
    file(APPEND $ENV{GITHUB_ENV} "BASE_FULL_NAME="${BASE_FULL_NAME}"\n")

endif()

configure_file_in(${CURRENT_PROJECT} "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/CQtDeployer.json")
configure_file_in(${CURRENT_PROJECT} "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/CQtDeployerSingle.json")

# qifw installer
configure_file_in(${CURRENT_PROJECT} "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/QIFWTemplate/packages/CQtDeployer/meta/package.xml")
configure_file_in(${CURRENT_PROJECT} "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/QIFWTemplate/packages/CQtDeployer.1_6/meta/package.xml")
configure_file_in(${CURRENT_PROJECT} "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/QIFWTemplate/packages/QIF/meta/package.xml")

# snap
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Deploy/snapcraft.yaml.in" "${CMAKE_SOURCE_DIR}/snap/snapcraft.yaml" @ONLY)
add_custom_target(${name}Templates ALL
    SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/snapcraft.yaml.in"
)

configure_file_in(${CURRENT_PROJECT} "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/QIFWTemplate/config/config.xml")


addDeployFromCustomFile(${CURRENT_PROJECT} "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/CQtDeployer.json")


ADD_CUSTOM_TARGET(
    downloadQIFW
    SOURCES ${${name}files}
    COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/QIFW.py" ${QIFW_PLATFORM} ${QIFW_VERSION} "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/QIFW"
    COMMENT python "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/QIFW.py" ${QIFW_PLATFORM} ${QIFW_VERSION} "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/QIFW"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_dependencies(deploy${CURRENT_PROJECT} downloadQIFW)

addDeployFromCustomFile(${CURRENT_PROJECT}Single "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/CQtDeployerSingle.json")
add_dependencies(deploy${CURRENT_PROJECT}Single downloadQIFW)

if (UNIX)
    addDeploySnap(${CURRENT_PROJECT} "${CMAKE_SOURCE_DIR}/Distro/")

    if(TARGET snapcraft${CURRENT_PROJECT})
        message("snap is depends of single deploy")
        add_dependencies(snapcraft${CURRENT_PROJECT} "deploy${CURRENT_PROJECT}Single")
    endif()

    addReleaseSnap(${CURRENT_PROJECT} latest/edge)
endif()
