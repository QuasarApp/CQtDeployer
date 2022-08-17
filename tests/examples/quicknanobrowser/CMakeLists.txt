cmake_minimum_required(VERSION 3.16)
project(quicknanobrowser LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/webenginequick/quicknanobrowser")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick WebEngineQuick)

qt_add_executable(quicknanobrowser
    main.cpp
    utils.h
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

# Resources:
set(resources_resource_files
    "ApplicationRoot.qml"
    "BrowserDialog.qml"
    "BrowserWindow.qml"
    "DownloadView.qml"
    "FindBar.qml"
    "FullScreenNotification.qml"
)

qt_add_resources(quicknanobrowser "resources"
    PREFIX
        "/"
    FILES
        ${resources_resource_files}
)

set(resources1_resource_files
    "icons/3rdparty/go-next.png"
    "icons/3rdparty/go-previous.png"
    "icons/3rdparty/process-stop.png"
    "icons/3rdparty/view-refresh.png"
)

qt_add_resources(quicknanobrowser "resources1"
    PREFIX
        "/icons"
    BASE
        "icons/3rdparty"
    FILES
        ${resources1_resource_files}
)

if(TARGET Qt::Widgets)
    target_link_libraries(quicknanobrowser PUBLIC
        Qt::Widgets
    )
endif()

install(TARGETS quicknanobrowser
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
