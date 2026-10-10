cmake_minimum_required(VERSION 3.16)
project(QMLNativePlugin LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Quick REQUIRED)

# A custom qml module that contains a native (c++) type (MyType).
# Because the module is built without the NO_PLUGIN option,
# qt_add_qml_module auto generates a qml plugin shared library
# (libNativePluginModule.so) into the build directory.
#
# See https://github.com/QuasarApp/CQtDeployer/issues/847 for more
# information about deploying such auto generated qml plugins.
qt_add_library(NativePluginModule SHARED
    mytype.cpp
)
target_include_directories(NativePluginModule PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
qt_add_qml_module(NativePluginModule
    URI NativePluginModule
    VERSION 1.0
)

# The application that imports the custom module above.
qt_add_executable(QMLNativePlugin
    main.cpp
)
qt_add_qml_module(QMLNativePlugin
    URI QMLNativePlugin
    VERSION 1.0
    QML_FILES Main.qml
)
target_link_libraries(QMLNativePlugin
    PRIVATE Qt6::Quick NativePluginModule)
