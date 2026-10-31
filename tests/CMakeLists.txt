#
# Copyright (C) 2020-2026 QuasarApp.
# Distributed under the lgplv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

cmake_minimum_required(VERSION 3.18)

#
# Fast unit tests (issue #861).
#
# Every test class is a small stand-alone QTest executable that uses the
# standard QTEST_GUILESS_MAIN macro (no custom main(), see the Qt test
# tutorial https://doc.qt.io/qt-6/qttestlib-tutorial1-example.html).
#
# The executables are registered with the addTestsArg helper of the
# QuasarApp CMake CI targets
# (submodules/QuasarAppLib/CMake/QuasarAppCITargets.cmake). For every test
# addTestsArg creates a deployTest<Name> target that deploys the executable
# with the cqtdeployer tool into ${DIR_FOR_TESTING}/<Name> and a test<Name>
# target that runs the deployed executable. All test targets are attached
# to the `test` target, so all tests are run automatically on every
# platform with:
#
#   cmake --build <build-dir> --target test
#

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)

set(TEST_CLASSES AppImage DeployConfig DeployCore Dmg DistroModule DistroStruct Envirement
    FileManager IgnoreRule LibInfo MacOS PathUtils Pe Qifw TargetInfo ZipCompresser)

# The deploy step of the test targets runs the cqtdeployer tool with the
# qmake taken from the QT_QMAKE_EXECUTABLE variable. The Qt CMake packages
# do not set this variable, so resolve qmake from the PATH when it is empty.
if (NOT QT_QMAKE_EXECUTABLE)
    find_program(QT_QMAKE_EXECUTABLE NAMES qmake6 qmake)
endif()

# The deployTest<Name> targets start one cqtdeployer instance per test, all
# in parallel. They all create their deployment folder inside the common
# ${DIR_FOR_TESTING} directory. If that parent directory does not exist yet,
# the first two cqtdeployer instances race on its creation and the losing
# one aborts with "Fail to copy targets" (QDir::mkdir fails when the
# directory was created by the other process in the meantime). Create the
# directory in a dedicated target that every deploy target depends on, so
# the parent exists before any of the parallel deployments starts.
add_custom_target(
    prepareTestDir
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DIR_FOR_TESTING}
    COMMENT "Create the ${DIR_FOR_TESTING} directory for the parallel test deployments")

set(QT_ROOT_DIR "$ENV{QT_ROOT_DIR}")
message("QT_ROOT_DIR = ${QT_ROOT_DIR}")

foreach (TEST_CLASS ${TEST_CLASSES})
    string(TOLOWER ${TEST_CLASS} TEST_CLASS_LOWER)
    set(TEST_EXEC ${PROJECT_NAME}-tests-${TEST_CLASS})

    add_executable(${TEST_EXEC} ${TEST_CLASS_LOWER}test.cpp ${TEST_CLASS_LOWER}test.h)
    target_link_libraries(${TEST_EXEC} PRIVATE Qt${QT_VERSION_MAJOR}::Test Deploy)

    # Register the test executable with the QuasarApp CI test targets.
    addTestsArg(${TEST_CLASS} ${TEST_EXEC} "-o;test_report.txt;-txt")

    add_dependencies(deployTest${TEST_CLASS} ${TEST_EXEC} prepareTestDir)
endforeach()

# The Qifw test runs the cqtdeployer executable of the current build tree
# (not the stable release installed by the CI action), so it verifies the
# code under review. The build tree executable and the qmake of the
# toolchain are passed to the test as compile definitions (issue #738).
target_compile_definitions(${PROJECT_NAME}-tests-Qifw PRIVATE
    QIFW_TEST_CQT_DEPLOYER_EXE="$<TARGET_FILE:${PROJECT_NAME}>"
    QIFW_TEST_QMAKE="${QT_QMAKE_EXECUTABLE}")

# The cqtdeployer of the build tree finds the Qt Installer Framework tools
# next to itself: the copyQIFW target stages the platform pair from the
# repository (issue #867). The copyQIFW target is not part of the `all`
# target and the CI runs the `test` target separately, so the Qifw test
# target depends on it explicitly - the qtifw distribution is then tested
# on every platform, including the macOS hosts (issue #738).
add_dependencies(testQifw copyQIFW)

# The AppImage test runs the cqtdeployer executable of the current build
# tree (not the stable release installed by the CI action), so it
# verifies the code under review (issue #898). The build tree executable
# and the qmake of the toolchain are passed to the test as compile
# definitions.
target_compile_definitions(${PROJECT_NAME}-tests-AppImage PRIVATE
    APPIMAGE_TEST_CQT_DEPLOYER_EXE="$<TARGET_FILE:${PROJECT_NAME}>"
    APPIMAGE_TEST_QMAKE="${QT_QMAKE_EXECUTABLE}")

# The cqtdeployer of the build tree finds the appimagetool next to itself:
# the copyAppImage target stages the platform binary and the wrapper
# script from the repository (issue #898). The copyAppImage target is not
# part of the `all` target and the CI runs the `test` target separately,
# so the AppImage test target depends on it explicitly - the appimage
# distribution is then tested on every Linux host.
add_dependencies(testAppImage copyAppImage)

# The Dmg test runs the cqtdeployer executable of the current build
# tree (not the stable release installed by the CI action), so it
# verifies the code under review (issue #899). The DMG packages are
# created with the hdiutil tool of the macOS system (no bundled tools,
# the copy targets are not needed), on the other host platforms the
# test verifies the warning of the skipped option only.
target_compile_definitions(${PROJECT_NAME}-tests-Dmg PRIVATE
    DMG_TEST_CQT_DEPLOYER_EXE="$<TARGET_FILE:${PROJECT_NAME}>"
    DMG_TEST_QMAKE="${QT_QMAKE_EXECUTABLE}")

# The self-deploy configs of the build (the configured CQtDeployer.json
# and CQtDeployerSingle.json of the source tree, see the configure_file_in
# call of the CQtDeployer project) are used by the MacOS test to check
# that the multi-package config (the main QtIFW configuration for all
# platforms: the 2 packages cqtdeployer and QIF into one target folder)
# generates the Qt Installer Framework installer (the qif template + the
# qifOut name, issue #884) and that the flat single config is the one
# package version for DEB, ZIP and DMG (the dmg option is enabled: the
# deploy command creates the DMG package on the macOS hosts, issue #899;
# no qif and no qifOut entry, issue #884). The install script of the
# installer template
# of the repository (the QIFWTemplate folder) is used to check the
# cqt + cqtdeployer console symlinks of the macOS hosts (issue #884).
target_compile_definitions(${PROJECT_NAME}-tests-MacOS PRIVATE
    CQT_DEPLOYER_SELF_DEPLOY_CONFIG="${CMAKE_SOURCE_DIR}/src/CQtDeployer/Deploy/CQtDeployer.json"
    CQT_DEPLOYER_SELF_DEPLOY_SINGLE_CONFIG="${CMAKE_SOURCE_DIR}/src/CQtDeployer/Deploy/CQtDeployerSingle.json"
    CQT_DEPLOYER_QIFW_TEMPLATE_INSTALLSCRIPT="${CMAKE_SOURCE_DIR}/src/CQtDeployer/Deploy/QIFWTemplate/packages/CQtDeployer.1_6/meta/installscript.js"
    CQT_DEPLOYER_QIFW_MAC_BINARIES_DIR="${CMAKE_SOURCE_DIR}/src/CQtDeployer/Deploy/QIFW/binaries/mac")

initTests()