#
# Copyright (C) 2020-2026 QuasarApp.
# Distributed under the GPLv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

cmake_minimum_required(VERSION 3.18)

#
# Fast unit tests (issue #861).
#
# This executable replaces the old heavy integration tests. It does not
# deploy any sample application and does not require Qt Installer
# Framework. It only exercises single functions of the Deploy library,
# so it is fast enough for the pull request checks.
#
# The tests are registered with the QuasarApp CMake test targets (the same
# pattern as addTestsArg from
# submodules/QuasarAppLib/CMake/QuasarAppCITargets.cmake), so they are
# deployed with the cqtdeployer tool and run with the test target:
#
#   cmake --build <build-dir> --target test
#
# The deploy command is duplicated here (instead of calling addTestsArg)
# because the Qt runtime lib directory is added to the -libDir option.
# The official Qt builds link their libQt6Core against the ICU libraries,
# which cqtdeployer classifies as system libraries and does not copy.
# The deployed libQt6Core.so.6 carries a RUNPATH of $ORIGIN, so the ICU
# libraries must sit next to it in the deployed lib directory. Listing
# the Qt lib directory in -libDir makes cqtdeployer treat these
# libraries as project libraries and copy them into the deployment.

get_filename_component(CURRENT_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR} NAME)

set(CURRENT_PROJECT "${PROJECT_NAME}-${CURRENT_PROJECT_DIR}")

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)

file(GLOB TEST_CPP "*.cpp" "*.h")

add_executable(${CURRENT_PROJECT} ${TEST_CPP})

target_link_libraries(${CURRENT_PROJECT} PRIVATE Qt${QT_VERSION_MAJOR}::Test Deploy)

# Path to the PE fixture (example.exe) from the pe-parse submodule.
# It is used by PeTest to parse a real PE file. The definition is only
# added when the fixture exists, so the PE tests are gracefully skipped
# (QSKIP) in source trees without the pe-parse test assets.
set(PE_EXAMPLE_EXE "${CMAKE_CURRENT_SOURCE_DIR}/../submodules/pe-parse/tests/assets/example.exe")
if (EXISTS "${PE_EXAMPLE_EXE}")
    target_compile_definitions(${CURRENT_PROJECT} PRIVATE PE_EXAMPLE_EXE="${PE_EXAMPLE_EXE}")
endif()

# Copy the runtime dependencies of the test executable next to it,
# so that the executable can also be started directly from the build
# tree without a deployment step.
add_custom_command(TARGET ${CURRENT_PROJECT} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Deploy>
        $<TARGET_FILE:QuasarApp>
        $<TARGET_FILE:QtELFReader>
        $<TARGET_FILE:pe-parse>
        $<TARGET_FILE:zip>
        $<TARGET_FILE_DIR:${CURRENT_PROJECT}>)

if (WIN32)
    add_custom_command(TARGET ${CURRENT_PROJECT} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Qt${QT_VERSION_MAJOR}::Core>
            $<TARGET_FILE:Qt${QT_VERSION_MAJOR}::Test>
            $<TARGET_FILE_DIR:${CURRENT_PROJECT}>)
endif()

# The deploy step of the test targets runs the cqtdeployer tool with
# the qmake taken from the QT_QMAKE_EXECUTABLE variable. The Qt CMake
# packages do not set this variable, so resolve qmake from the PATH
# when it is empty.
if (NOT QT_QMAKE_EXECUTABLE)
    find_program(QT_QMAKE_EXECUTABLE NAMES qmake6 qmake)
endif()

# Directory that contains the Qt runtime libraries of the current Qt
# installation (e.g. .../gcc_64/lib on Linux, .../win64_mingw/lib on
# Windows). It is added to the -libDir option of the deploy step, see
# the note in the file header.
get_target_property(QT_CORE_LIB_DIR Qt${QT_VERSION_MAJOR}::Core LOCATION)
if (NOT QT_CORE_LIB_DIR)
    get_target_property(QT_CORE_LIB_DIR Qt${QT_VERSION_MAJOR}::Core LOCATION_RELEASE)
endif()
get_filename_component(QT_CORE_LIB_DIR "${QT_CORE_LIB_DIR}" DIRECTORY)

set(EXEC_TEST "${CMAKE_CURRENT_BINARY_DIR}/${CURRENT_PROJECT}")
set(RUN_CMD "${DIR_FOR_TESTING}/${PROJECT_NAME}/${CURRENT_PROJECT}.sh")

if (WIN32)
    set(EXEC_TEST "${CMAKE_CURRENT_BINARY_DIR}/${CURRENT_PROJECT}.exe")
    set(RUN_CMD "${DIR_FOR_TESTING}/${PROJECT_NAME}/${CURRENT_PROJECT}.exe")
endif()

ADD_CUSTOM_TARGET(
    deployTest${PROJECT_NAME}
    COMMAND cqtdeployer clear -bin ${EXEC_TEST} -qmake ${QT_QMAKE_EXECUTABLE}
        -targetDir ${DIR_FOR_TESTING}/${PROJECT_NAME}
        -libDir "${CMAKE_SOURCE_DIR},${CMAKE_BINARY_DIR},${QT_CORE_LIB_DIR}"
        -recursiveDepth 5
    COMMENT "Deploy Test: cqtdeployer clear -bin ${EXEC_TEST} -qmake ${QT_QMAKE_EXECUTABLE} -targetDir ${DIR_FOR_TESTING}/${PROJECT_NAME}"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

ADD_CUSTOM_TARGET(
    test${PROJECT_NAME}
    COMMAND ${RUN_CMD} -o test_report.txt -txt
    COMMENT "=================== Run Test ==================="
    WORKING_DIRECTORY ${DIR_FOR_TESTING}/${PROJECT_NAME}
    DEPENDS deployTest${PROJECT_NAME}
)

add_dependencies(test test${PROJECT_NAME})

# The test targets deploy the test executable from the build tree.
# Make sure the executable is built before the deploy step runs.
add_dependencies(deployTest${PROJECT_NAME} ${CURRENT_PROJECT})
