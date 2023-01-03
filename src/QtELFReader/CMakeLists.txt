#
# Copyright (C) 2020-2023 QuasarApp.
# Distributed under the GPLv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

cmake_minimum_required(VERSION 3.18)

get_filename_component(CURRENT_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR} NAME)

set(CURRENT_PROJECT "${CURRENT_PROJECT_DIR}")
add_definitions(-DQT_ELFREADER_LIBRARY)


file(GLOB_RECURSE SOURCE_CPP
    "src/*.cpp"
    "src/*.h"
)

set(PUBLIC_INCUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

if (NOT QT_VERSION_MAJOR)
    find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
endif()

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

add_library(${CURRENT_PROJECT} ${SOURCE_CPP})

target_link_libraries(${CURRENT_PROJECT} PUBLIC Qt::Core )

target_include_directories(${CURRENT_PROJECT} PUBLIC ${PUBLIC_INCUDE_DIR})


configure_file_in(${CURRENT_PROJECT} "${CMAKE_CURRENT_SOURCE_DIR}/src/elfreader_global.h")

install(TARGETS ${CURRENT_PROJECT}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
