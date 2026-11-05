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

elseif (APPLE)
    # The binarycreator of the macOS hosts creates the Qt Installer
    # Framework installer as the .app bundle (issue #738), not the
    # self-extracting .run file of the Linux hosts, so the .run suffix
    # does not belong to the installer name: the release asset of the
    # macOS hosts is <INSTALLER_FULL_NAME>.app.zip (issue #884).
    set(INSTALLER_SUFIX "")
endif()

set(BASE_NAME "${CURRENT_PROJECT}_${CQT_DEPLOYER_VERSION}")
string(TOLOWER ${BASE_NAME} SNAP_BASE_NAME)

set(BASE_FULL_NAME "${BASE_NAME}_${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}")

set(INSTALLER_FULL_NAME "${BASE_FULL_NAME}${INSTALLER_SUFIX}")

if(DEFINED ENV{GITHUB_ENV})
    file(APPEND $ENV{GITHUB_ENV} "INSTALLER_FULL_NAME=${INSTALLER_FULL_NAME}\n")
    file(APPEND $ENV{GITHUB_ENV} "BASE_FULL_NAME=${BASE_FULL_NAME}\n")
    file(APPEND $ENV{GITHUB_ENV} "SNAP_BASE_NAME=${SNAP_BASE_NAME}\n")


endif()

# The libDir entry of the self-deploy configuration points explicitly
# to the library directory of the Qt installation (the libDir path
# within QT_ROOT), not to the whole Qt prefix. Registering the entire
# prefix in the deploy environment puts every file of the Qt
# installation (headers, QML modules, mkspecs, documentation) into the
# dependency scan, and cqtdeployer can pull those extraneous files into
# the release package. The explicit lib path also keeps the Qt-bundled
# system libraries (e.g. ICU) as ExtraLib candidates, so they are
# preferred over the system copies (review note on PR #886).
if (QT_QMAKE_EXECUTABLE)
    get_filename_component(CQT_DEPLOYER_QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)
    get_filename_component(CQT_DEPLOYER_QT_ROOT "${CQT_DEPLOYER_QT_BIN_DIR}" DIRECTORY)
    if (WIN32)
        # Qt for Windows keeps its DLLs in the bin directory of the kit.
        set(CQT_DEPLOYER_QT_LIB_DIR "${CQT_DEPLOYER_QT_ROOT}/bin")
    else()
        set(CQT_DEPLOYER_QT_LIB_DIR "${CQT_DEPLOYER_QT_ROOT}/lib")
    endif()
else()
    set(CQT_DEPLOYER_QT_LIB_DIR "${CMAKE_PREFIX_PATH}")
endif()

# The "AppImage" folder with the bundled appimagetool is bundled into the
# package only on the Linux hosts (the extraData entry of the self-deploy
# configs, issue #898): the AppImage tool is a Linux-only tool and the
# "appimage" option is available on the Linux host platforms only.
if (UNIX AND NOT APPLE)
    set(DEPLOY_EXTRA_DATA "./QIFW/QIF\", \"./AppImage")
    set(APPIMAGE_QIF_EXTRADATA "        [\"QIF\", \"./AppImage\"],")
else()
    set(DEPLOY_EXTRA_DATA "./QIFW/QIF")
    set(APPIMAGE_QIF_EXTRADATA "")
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
# zip packages. The QIF folder itself is tracked in the repository (so any
# change to the staged binaries is visible in the git diff), therefore the
# stale content of a previous build for another platform is removed before
# the copy. Provenance of the binaries: Deploy/QIFW/binaries/README.md.
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
    # Remove the stale content first: the QIF folder is tracked in the
    # repository, so a checkout may contain the binaries of another
    # platform which must not end up in the package.
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${QIF_DESTINATION_PATH}"
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

# The appimagetool AppImage (issue #898) is stored in the repository for
# every Linux platform (Deploy/AppImageSources/<platform>) because the
# tool must be available on the hosts where the AppImage tools are not
# installed by default (much like the Qt Installer Framework binaries of
# issue #867). The AppImage tool is a Linux-only tool, so the staging
# runs on the Linux hosts only (the "appimage" option is available on the
# Linux host platforms only, see the appimage option of the Deploy
# library). The target below stages the platform-matching binary and the
# wrapper script into the AppImage destination folder, which is bundled
# into the installer (the "QIF" package) and into the flat packages. The
# AppImage folder itself is tracked in the repository (so any change to
# the staged binaries is visible in the git diff), therefore the stale
# staged content of a previous build for another platform is removed
# before the copy. Provenance of the binaries:
# Deploy/AppImageSources/README.md.
if (UNIX AND NOT APPLE)
    set(APPIMAGE_DESTINATION_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/AppImage")
    set(APPIMAGE_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/AppImageSources")
    set(APPIMAGE_WRAPPER_SOURCE "${APPIMAGE_SOURCES_DIR}/appimagetool")

    if (CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
        set(APPIMAGE_PLATFORM_DIR linux-aarch64)
    else()
        set(APPIMAGE_PLATFORM_DIR linux-x86_64)
    endif()

    set(APPIMAGE_SOURCE_BIN_PATH "${APPIMAGE_SOURCES_DIR}/${APPIMAGE_PLATFORM_DIR}/appimagetool.AppImage")
    set(APPIMAGE_OUT_BIN_PATH "${APPIMAGE_DESTINATION_PATH}/appimagetool.AppImage")
    set(APPIMAGE_OUT_WRAPPER "${APPIMAGE_DESTINATION_PATH}/appimagetool")

    # The AppImage tool is also staged next to the built executable so that a
    # development build of cqtdeployer finds it at runtime (the AppImage
    # distribution searches <executable dir>/AppImage, see
    # src/Deploy/src/Distributions/appimage.cpp).
    set(APPIMAGE_DEV_DIR "${CMAKE_BINARY_DIR}/src/CQtDeployer/AppImage")
    set(APPIMAGE_DEV_BIN_PATH "${APPIMAGE_DEV_DIR}/appimagetool.AppImage")
    set(APPIMAGE_DEV_WRAPPER "${APPIMAGE_DEV_DIR}/appimagetool")

    add_custom_target(
        copyAppImage
        COMMAND ${CMAKE_COMMAND} -E rm -f "${APPIMAGE_OUT_BIN_PATH}" "${APPIMAGE_OUT_WRAPPER}"
        COMMAND ${CMAKE_COMMAND} -E copy "${APPIMAGE_SOURCE_BIN_PATH}" "${APPIMAGE_OUT_BIN_PATH}"
        COMMAND ${CMAKE_COMMAND} -E copy "${APPIMAGE_WRAPPER_SOURCE}" "${APPIMAGE_OUT_WRAPPER}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${APPIMAGE_DEV_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy "${APPIMAGE_SOURCE_BIN_PATH}" "${APPIMAGE_DEV_BIN_PATH}"
        COMMAND ${CMAKE_COMMAND} -E copy "${APPIMAGE_WRAPPER_SOURCE}" "${APPIMAGE_DEV_WRAPPER}"

        COMMENT "Stage the appimagetool AppImage (${APPIMAGE_PLATFORM_DIR}) to ${APPIMAGE_DESTINATION_PATH}"
    )

    add_dependencies(deploy${CURRENT_PROJECT} copyAppImage)
    add_dependencies(deploy${CURRENT_PROJECT}Single copyAppImage)
endif()



if (UNIX AND NOT APPLE)
    addDeploySnap(${CURRENT_PROJECT} "${CMAKE_SOURCE_DIR}/Distro/")

    if(TARGET snapcraft${CURRENT_PROJECT})
        message("snap is depends of single deploy")
        add_dependencies(snapcraft${CURRENT_PROJECT} "deploy${CURRENT_PROJECT}Single")
    endif()

    addReleaseSnap(${CURRENT_PROJECT} latest/edge)
endif()
