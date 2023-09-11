#
# Copyright (C) 2021-2023 QuasarApp.
# Distributed under the GPLv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

cmake_minimum_required(VERSION 3.18)

set(CMAKE_AUTOMOC ON)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Qml Quick)
set(CURRENT_PROJECT "TestQMLWidgets")

# needed for test RPATH functions
get_filename_component(QT_QT_ROOT "${Qt${QT_VERSION_MAJOR}Core_DIR}/../../.." ABSOLUTE)
set(CMAKE_BUILD_RPATH ${QT_QT_ROOT}/bin)

file(GLOB_RECURSE SOURCE_CPP
    "*.cpp" "*.h"
)

file(GLOB_RECURSE SOURCE_QRC
    "*.qrc"
)

set(ALL_SOURCES ${SOURCE_CPP} ${SOURCE_QRC})
set(PUBLIC_INCUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(${CURRENT_PROJECT} ${ALL_SOURCES} )

target_link_libraries(${CURRENT_PROJECT} PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
)

target_include_directories(${CURRENT_PROJECT} PUBLIC ${PUBLIC_INCUDE_DIR})
