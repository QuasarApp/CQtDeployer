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

set(TEST_CLASSES DeployConfig DeployCore DistroModule DistroStruct Envirement
    FileManager IgnoreRule LibInfo MacOS PathUtils Pe TargetInfo ZipCompresser)

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

initTests()