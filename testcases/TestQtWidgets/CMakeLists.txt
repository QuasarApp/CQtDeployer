#
# Copyright (C) 2021-2022 QuasarApp.
# Distributed under the GPLv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

cmake_minimum_required(VERSION 3.18)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Widgets)

set(CURRENT_PROJECT "QtWidgetsProject")

file(GLOB_RECURSE SOURCE_CPP
    "*.cpp" "*.h"
)

file(GLOB_RECURSE SOURCE_QRC
    "*.qrc"
)

set(ALL_SOURCES ${SOURCE_CPP} ${SOURCE_QRC})
set(PUBLIC_INCUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(${CURRENT_PROJECT} ${ALL_SOURCES} )

target_link_libraries(${CURRENT_PROJECT} PUBLIC Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets)
target_include_directories(${CURRENT_PROJECT} PUBLIC ${PUBLIC_INCUDE_DIR})
