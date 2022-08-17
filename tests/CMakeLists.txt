#
# Copyright (C) 2020-2022 QuasarApp.
# Distributed under the GPLv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

cmake_minimum_required(VERSION 3.18)

get_filename_component(CURRENT_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR} NAME)

set(CURRENT_PROJECT "${PROJECT_NAME}-${CURRENT_PROJECT_DIR}")

file(GLOB_RECURSE SOURCE_CPP
    "*.cpp" "*.h" "*.qrc"
)

set(PUBLIC_INCUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(PUBLIC_INCUDE_DIR ${PUBLIC_INCUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/units")

get_filename_component(QT_QT_ROOT "${Qt${QT_VERSION_MAJOR}Core_DIR}/../../.." ABSOLUTE)

add_definitions(-DQT_BASE_DIR="${QT_QT_ROOT}")
add_definitions(-DTEST_BIN_DIR="${CMAKE_CURRENT_LIST_DIR}")

add_executable(${CURRENT_PROJECT} ${SOURCE_CPP})
target_link_libraries(${CURRENT_PROJECT} PRIVATE Qt${QT_VERSION_MAJOR}::Test LibDeploy)

target_include_directories(${CURRENT_PROJECT} PUBLIC ${PUBLIC_INCUDE_DIR})


initTests()
addTests(${PROJECT_NAME} ${CURRENT_PROJECT})

add_subdirectory(examples/quicknanobrowser)
add_subdirectory(examples/TestCPPOnly)
add_subdirectory(examples/TestOnlyC)
add_subdirectory(examples/TestQMLWidgets)
add_subdirectory(examples/TestQtWidgets)
add_subdirectory(examples/virtualkeyboard)
add_subdirectory(examples/webui)
