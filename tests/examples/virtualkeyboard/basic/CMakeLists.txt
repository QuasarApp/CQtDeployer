cmake_minimum_required(VERSION 3.16)
project(basic LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/virtualkeyboard/basic")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)

qt_add_executable(basic
    main.cpp
)

set_target_properties(basic PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(basic PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
)

# Resources:
set(demo_resource_files
    "Basic.qml"
    "basic-b2qt.qml"
    "content/AutoScroller.qml"
    "content/FloatingButton_Active.svg"
    "content/FloatingButton_Available.svg"
    "content/FloatingButton_Unavailable.svg"
    "content/HandwritingModeButton.qml"
    "content/TextArea.qml"
    "content/TextField.qml"
)

qt6_add_resources(basic "demo"
    PREFIX
        "/"
    FILES
        ${demo_resource_files}
)

if(QT_FEATURE_static)
    find_package(Qt6 REQUIRED COMPONENTS Svg)
    target_link_libraries(basic PUBLIC
        Qt::Svg
    )
endif()

if(NOT QT_FEATURE_vkb_desktop)
    target_compile_definitions(basic PUBLIC
        MAIN_QML="basic-b2qt.qml"
    )
endif()

if(QT_FEATURE_vkb_desktop)
    target_compile_definitions(basic PUBLIC
        MAIN_QML="Basic.qml"
    )
endif()

install(TARGETS basic
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
