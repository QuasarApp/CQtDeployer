cmake_minimum_required(VERSION 3.16)

project(QMLFileDialog LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Quick REQUIRED)

qt_add_executable(QMLFileDialog
    main.cpp
)

qt_add_qml_module(QMLFileDialog
    URI QMLFileDialog
    VERSION 1.0
    RESOURCE_PREFIX "/"
    QML_FILES main.qml 
)

target_link_libraries(QMLFileDialog
    PRIVATE Qt6::Quick)
