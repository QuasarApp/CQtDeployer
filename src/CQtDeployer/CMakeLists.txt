#
# Copyright (C) 2021-2026 QuasarApp.
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

set(BASE_NAME "${CURRENT_PROJECT}_${CQT_DEPLOYER_VERSION}")
string(TOLOWER ${BASE_NAME} SNAP_BASE_NAME)

set(BASE_FULL_NAME "${BASE_NAME}_${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}")

set(INSTALLER_FULL_NAME "${BASE_FULL_NAME}${INSTALLER_SUFIX}")

# macOS: the deployed application is packed into a .app bundle and a
# compressed DMG disk image by the cqtdeployer "dmg" distribution
# (issue #166). The DMG name and the enabled flag are used in the
# Deploy/CQtDeployer.json.in template.
set(DMG_NAME "${BASE_FULL_NAME}.dmg")
if (APPLE)
    set(DMG_ENABLED true)
else()
    set(DMG_ENABLED false)
endif()

if(DEFINED ENV{GITHUB_ENV} AND APPLE)
    file(APPEND $ENV{GITHUB_ENV} "DMG_NAME=${DMG_NAME}\n")
endif()

if(DEFINED ENV{GITHUB_ENV})
    file(APPEND $ENV{GITHUB_ENV} "INSTALLER_FULL_NAME=${INSTALLER_FULL_NAME}\n")
    file(APPEND $ENV{GITHUB_ENV} "BASE_FULL_NAME=${BASE_FULL_NAME}\n")
    file(APPEND $ENV{GITHUB_ENV} "SNAP_BASE_NAME=${SNAP_BASE_NAME}\n")


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

set(QIF_DESTINATION_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/QIFW/QIF")

addDeployFromCustomFile(${CURRENT_PROJECT}Single "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/CQtDeployerSingle.json")

# The Qt Installer Framework binaries (binarycreator + installerbase) are
# stored in the repository for every supported platform
# (Deploy/QIFW/binaries/<platform>) because aqtinstall does not provide
# IFW 4.8+ (miurahr/aqtinstall#792, QuasarApp/CQtDeployer#867). The target
# below copies the platform-matching pair into the QIF destination folder,
# which is bundled into the installer (the "QIF" package) and into the flat
# zip packages. Provenance of the binaries: Deploy/QIFW/binaries/README.md.
set(QIF_BINARIES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/QIFW/binaries")

if (WIN32)
    set(QIF_PLATFORM_DIR windows)
    set(QIF_BIN_SUFFIX ".exe")

elseif (APPLE)
    # one universal binary (x86_64 + arm64) covers both macOS architectures
    set(QIF_PLATFORM_DIR mac)
    set(QIF_BIN_SUFFIX "")

elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
    set(QIF_PLATFORM_DIR linux-arm64)
    set(QIF_BIN_SUFFIX "")

else()
    set(QIF_PLATFORM_DIR linux-x86_64)
    set(QIF_BIN_SUFFIX "")

endif()

set(QIF_SOURCE_BIN_PATH "${QIF_BINARIES_DIR}/${QIF_PLATFORM_DIR}/binarycreator${QIF_BIN_SUFFIX}")
set(QIF_SOURCE_BIN_PATH_B "${QIF_BINARIES_DIR}/${QIF_PLATFORM_DIR}/installerbase${QIF_BIN_SUFFIX}")

set(QIF_OUT_BIN_PATH "${QIF_DESTINATION_PATH}/binarycreator${QIF_BIN_SUFFIX}")
set(QIF_OUT_BIN_PATH_B "${QIF_DESTINATION_PATH}/installerbase${QIF_BIN_SUFFIX}")

# The QIF tools are also copied next to the built executable so that a
# development build of cqtdeployer finds them at runtime (the QIF
# distribution searches <executable dir>/QIF, see
# src/Deploy/src/Distributions/qif.cpp).
set(QIF_DEV_DIR "${CMAKE_BINARY_DIR}/src/CQtDeployer/QIF")
set(QIF_DEV_OUT_BIN_PATH "${QIF_DEV_DIR}/binarycreator${QIF_BIN_SUFFIX}")
set(QIF_DEV_OUT_BIN_PATH_B "${QIF_DEV_DIR}/installerbase${QIF_BIN_SUFFIX}")

add_custom_target(
    copyQIFW
    COMMAND ${CMAKE_COMMAND} -E make_directory "${QIF_DESTINATION_PATH}"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${QIF_SOURCE_BIN_PATH}"
        "${QIF_OUT_BIN_PATH}"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${QIF_SOURCE_BIN_PATH_B}"
        "${QIF_OUT_BIN_PATH_B}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${QIF_DEV_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${QIF_SOURCE_BIN_PATH}"
        "${QIF_DEV_OUT_BIN_PATH}"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${QIF_SOURCE_BIN_PATH_B}"
        "${QIF_DEV_OUT_BIN_PATH_B}"

    COMMENT "Copy the Qt Installer Framework ${QIFW_VERSION} binaries (${QIF_PLATFORM_DIR}) to ${QIF_DESTINATION_PATH}"
)

add_dependencies(deploy${CURRENT_PROJECT} copyQIFW)
add_dependencies(deploy${CURRENT_PROJECT}Single copyQIFW)



if (UNIX AND NOT APPLE)
    addDeploySnap(${CURRENT_PROJECT} "${CMAKE_SOURCE_DIR}/Distro/")

    if(TARGET snapcraft${CURRENT_PROJECT})
        message("snap is depends of single deploy")
        add_dependencies(snapcraft${CURRENT_PROJECT} "deploy${CURRENT_PROJECT}Single")
    endif()

    addReleaseSnap(${CURRENT_PROJECT} latest/edge)
endif()
