#
# Copyright (C) 2020-2022 QuasarApp.
# Distributed under the GPLv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

cmake_minimum_required(VERSION 3.18)
project(CQtDeployer LANGUAGES CXX)
if(TARGET ${PROJECT_NAME})
  message("The ${PROJECT_NAME} arledy included in main Project")
  return()
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS ON)

if (IOS)
    set(BUILD_SHARED_LIBS OFF)
endif()

if (NOT QT_VERSION_MAJOR)
    find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Test QUIET)
endif()
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Test QUIET)

include(submodules/QuasarAppLib/CMake/QuasarApp.cmake)

updateGitVars()
set(CQT_DEPLOYER_VERSION "1.6.${GIT_COMMIT_COUNT}.${GIT_COMMIT_HASH}")

option(CQT_DEPLOYER_TESTS "This option disables or enables tests of the ${PROJECT_NAME} project"  ON)
option(CQT_DEPLOYER_TOOL "This option disables or enables example app of the ${PROJECT_NAME} project" ON)

if (ANDROID OR IOS OR NOT QT_VERSION_MAJOR OR QA_WASM32)
    set(CQT_DEPLOYER_TESTS OFF CACHE BOOL "This option force disbled for ANDROID IOS QA_WASM32 and Not Qt projects" FORCE)
endif()

if (NOT QT_VERSION_MAJOR)
    set(CQT_DEPLOYER_TOOL OFF CACHE BOOL "This option force disbled for Not Qt projects" FORCE)
endif()

make_directory(Distro)

initAll()

add_subdirectory(submodules/QuasarAppLib)
add_subdirectory(src/QtELFReader)

option(BUILD_COMMAND_LINE_TOOLS "Build Command Line Tools" OFF)
add_subdirectory(submodules/pe-parse)
add_subdirectory(submodules/zip)

add_subdirectory(src/LibDeploy)

if (DEFINED CQT_DEPLOYER_TOOL)
    add_subdirectory(src/CQtDeployer)
endif()

if (CQT_DEPLOYER_TESTS)
    add_subdirectory(tests)
else()
    message("The ${PROJECT_NAME} tests is disabled.")
endif()


addDoc(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/doxygen.conf)
addDeployFromCustomFile("CQtDeployer" "${CMAKE_CURRENT_SOURCE_DIR}/Deploy/CQtDeployer.json")
