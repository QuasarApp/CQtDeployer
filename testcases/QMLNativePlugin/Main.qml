import QtCore
import QtQuick
import NativePluginModule

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("Native QML Plugin Test")

    // A native (c++) type provided by the auto generated
    // "NativePluginModule" qml plugin.
    MyType {
        id: myType
    }

    Text {
        anchors.centerIn: parent
        text: "value: " + myType.value
    }
}
