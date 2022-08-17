cmake_minimum_required(VERSION 3.16)
project(webui LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/webenginewidgets/webui")

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui WebEngineWidgets)

# Resources:
file(GLOB_RECURSE SOURCE_QRC
    "*.qrc"
)

add_executable(webui
    main.cpp
    webuihandler.cpp webuihandler.h
    ${SOURCE_QRC}

)

set_target_properties(webui PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(webui PUBLIC
    Qt::Core
    Qt::Gui
    Qt::WebEngineWidgets
)
