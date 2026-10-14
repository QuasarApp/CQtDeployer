#
# Copyright (C) 2026 QuasarApp.
# Distributed under the GPLv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

cmake_minimum_required(VERSION 3.18)

#
# Fast unit tests (issue #861).
#
# Unlike the integration tests (tests/ -> CQtDeployer-tests) this
# executable does not deploy any sample application and does not need
# a full Qt installation, qmake or Qt Installer Framework. It only
# exercises single functions of the Deploy library, so it can be used
# for fast checks in pull requests.
#
# The executable is not registered with CTest, because the QuasarApp
# CI targets (initAll -> initTests) reserve the "test" target name
# that CTest requires. It is run directly from the build tree:
#
#   cmake --build <build-dir> --target CQtDeployer-unittests
#   <build-dir>/tests/unittests/CQtDeployer-unittests
#

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
set(PE_EXAMPLE_EXE "${CMAKE_CURRENT_SOURCE_DIR}/../../submodules/pe-parse/tests/assets/example.exe")
if (EXISTS "${PE_EXAMPLE_EXE}")
    target_compile_definitions(${CURRENT_PROJECT} PRIVATE PE_EXAMPLE_EXE="${PE_EXAMPLE_EXE}")
endif()

# The unit tests are run directly from the build tree by ctest.
# Copy the runtime dependencies of the test executable next to it,
# so that the executable can be started without any deployment step.
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
