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

set(TEST_CLASSES DeployConfig DistroModule DistroStruct Envirement
    FileManager IgnoreRule LibInfo PathUtils Pe TargetInfo ZipCompresser)

# Path to the PE fixture (example.exe) from the pe-parse submodule. It is
# used by PeTest to parse a real PE file. The definition is only added when
# the fixture exists, so the PE tests are gracefully skipped (QSKIP) in
# source trees without the pe-parse test assets.
set(PE_EXAMPLE_EXE "${CMAKE_CURRENT_SOURCE_DIR}/../submodules/pe-parse/tests/assets/example.exe")

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

foreach (TEST_CLASS ${TEST_CLASSES})
    string(TOLOWER ${TEST_CLASS} TEST_CLASS_LOWER)
    set(TEST_EXEC ${PROJECT_NAME}-tests-${TEST_CLASS})

    add_executable(${TEST_EXEC} ${TEST_CLASS_LOWER}test.cpp ${TEST_CLASS_LOWER}test.h)
    target_link_libraries(${TEST_EXEC} PRIVATE Qt${QT_VERSION_MAJOR}::Test Deploy)

    if (TEST_CLASS STREQUAL "Pe" AND EXISTS "${PE_EXAMPLE_EXE}")
        target_compile_definitions(${TEST_EXEC} PRIVATE PE_EXAMPLE_EXE="${PE_EXAMPLE_EXE}")
    endif()

    # Copy the runtime dependencies of the test executable next to it, so
    # that the executable can also be started directly from the build tree
    # without a deployment step.
    add_custom_command(TARGET ${TEST_EXEC} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Deploy>
            $<TARGET_FILE:QuasarApp>
            $<TARGET_FILE:QtELFReader>
            $<TARGET_FILE:pe-parse>
            $<TARGET_FILE:zip>
            $<TARGET_FILE_DIR:${TEST_EXEC}>)

    if (WIN32)
        add_custom_command(TARGET ${TEST_EXEC} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:Qt${QT_VERSION_MAJOR}::Core>
                $<TARGET_FILE:Qt${QT_VERSION_MAJOR}::Test>
                $<TARGET_FILE_DIR:${TEST_EXEC}>)
    endif()

    # Register the test executable with the QuasarApp CI test targets.
    addTestsArg(${TEST_CLASS} ${TEST_EXEC} "-o;test_report.txt;-txt")

    # The test targets deploy the test executable from the build tree.
    # Make sure the executable is built before the deploy step runs and
    # that the common ${DIR_FOR_TESTING} parent directory exists before
    # any of the parallel deployments starts (see prepareTestDir above).
    add_dependencies(deployTest${TEST_CLASS} ${TEST_EXEC} prepareTestDir)
endforeach()

# On Linux the official Qt builds link their libQt6Core against the ICU
# libraries, which live in the Qt installation directory. The addTestsArg
# deploy command only scans the source and the build tree, so the ICU
# libraries would be classified as system libraries and not copied into
# the deployment. The deployed libQt6Core.so.6 carries a RUNPATH of
# $ORIGIN, so the ICU libraries must sit next to it. Copy them into the
# build tree: there they are scanned as extra (project) libraries and end
# up in every test deployment.
if (UNIX AND NOT APPLE)
    get_target_property(QT_CORE_LIB_DIR Qt${QT_VERSION_MAJOR}::Core LOCATION)
    if (NOT QT_CORE_LIB_DIR)
        get_target_property(QT_CORE_LIB_DIR Qt${QT_VERSION_MAJOR}::Core LOCATION_RELEASE)
    endif()
    get_filename_component(QT_CORE_LIB_DIR "${QT_CORE_LIB_DIR}" DIRECTORY)
    file(GLOB QT_ICU_LIBS "${QT_CORE_LIB_DIR}/libicu*.so.[0-9]*")
    if (QT_ICU_LIBS)
        set(QUASAR_TEST_ICU_DIR "${CMAKE_BINARY_DIR}/qt-icu")
        add_custom_command(
            OUTPUT ${QUASAR_TEST_ICU_DIR}/.stamp
            COMMAND ${CMAKE_COMMAND} -E make_directory ${QUASAR_TEST_ICU_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_ICU_LIBS} ${QUASAR_TEST_ICU_DIR}
            COMMAND ${CMAKE_COMMAND} -E touch ${QUASAR_TEST_ICU_DIR}/.stamp
            DEPENDS ${QT_ICU_LIBS}
            COMMENT "Copy the Qt ICU libraries into the build tree for the test deployments")
        add_custom_target(deployTestICU DEPENDS ${QUASAR_TEST_ICU_DIR}/.stamp)
        foreach (TEST_CLASS ${TEST_CLASSES})
            add_dependencies(deployTest${TEST_CLASS} deployTestICU)
        endforeach()
    endif()
endif()