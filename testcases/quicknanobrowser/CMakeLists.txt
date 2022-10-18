cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOMOC ON)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Qml Quick WebEngineQuick)

# Resources:
file(GLOB_RECURSE SOURCE_QRC
    "*.qrc"
)

add_executable(quicknanobrowser
    main.cpp
    utils.h
    ${SOURCE_QRC}
)

set_target_properties(quicknanobrowser PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(quicknanobrowser PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
    Qt::WebEngineQuick
)
