#
# Copyright (C) 2020-2023 QuasarApp.
# Distributed under the GPLv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOMOC ON)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_subdirectory(TestCPPOnly)
add_subdirectory(TestOnlyC)
add_subdirectory(TestQMLWidgets)
add_subdirectory(TestQtWidgets)

if (NOT WIN32)
    add_subdirectory(QMLFileDialog)
    add_subdirectory(virtualkeyboard)
    add_subdirectory(quicknanobrowser)
    add_subdirectory(webui)
endif()
