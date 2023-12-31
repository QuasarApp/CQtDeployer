#
# Copyright (C) 2020-2024 QuasarApp.
# Distributed under the GPLv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

cmake_minimum_required(VERSION 3.18)

get_filename_component(CURRENT_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR} NAME)

set(CURRENT_PROJECT "${PROJECT_NAME}-${CURRENT_PROJECT_DIR}")

file(GLOB MAIN_CPP
    "*.cpp" "*.h" "*.qrc"
    "units/*.cpp" "units/*.h"
    "modules/*.cpp" "modules/*.h"
)

if (UNIX)
    file(GLOB UNITS_CPP
        "units/linux/*.cpp" "units/linux/*.h"
    )
endif()

if (WIN32)
    file(GLOB UNITS_CPP
        "units/win/*.cpp" "units/win/*.h"
    )
endif()

set(SOURCE_CPP ${MAIN_CPP} ${UNITS_CPP})

set(PUBLIC_INCUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(PUBLIC_INCUDE_DIR ${PUBLIC_INCUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/units")

if (UNIX)
    set(PUBLIC_INCUDE_DIR ${PUBLIC_INCUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/units/linux")
endif()

if (WIN32)
    set(PUBLIC_INCUDE_DIR ${PUBLIC_INCUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/units/win")
endif()

set(PUBLIC_INCUDE_DIR ${PUBLIC_INCUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/modules")

add_definitions(-DQT_BASE_DIR="${QT_QT_ROOT}/")
add_definitions(-DTEST_BIN_DIR="${CMAKE_CURRENT_LIST_DIR}/../testcases/bin/")

add_executable(${CURRENT_PROJECT} ${SOURCE_CPP})
target_link_libraries(${CURRENT_PROJECT} PRIVATE Qt${QT_VERSION_MAJOR}::Test Deploy)

target_include_directories(${CURRENT_PROJECT} PUBLIC ${PUBLIC_INCUDE_DIR})


initTests()

SET(QT_TEST_EXTRA_ARGS -maxwarnings 0)
addTestsArg(${PROJECT_NAME} ${CURRENT_PROJECT} "${QT_TEST_EXTRA_ARGS}")

