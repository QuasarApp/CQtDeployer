TEMPLATE = app
TARGET = basic
QT += qml quick

QT_FOR_CONFIG += virtualkeyboard

SOURCES += main.cpp
CONFIG += link_pkgconfig
contains(CONFIG, static) {
    QT += svg
    QTPLUGIN += qtvirtualkeyboardplugin
}

target.path = $$[QT_INSTALL_EXAMPLES]/virtualkeyboard/basic
INSTALLS += target

RESOURCES += \
    demo.qrc

OTHER_FILES += \
    Basic.qml \
    basic-b2qt.qml \
    content/AutoScroller.qml \
    content/HandwritingModeButton.qml \
    content/TextArea.qml \
    content/TextField.qml \

!qtConfig(vkb-desktop) {
    DEFINES += MAIN_QML=\\\"basic-b2qt.qml\\\"
} else {
    DEFINES += MAIN_QML=\\\"Basic.qml\\\"
}
