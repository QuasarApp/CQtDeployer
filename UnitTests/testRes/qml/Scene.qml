import QtQuick 2.11
import QtQuick.Controls.Material 2.0
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

Item {
    id: scene;
    z: -2

    Rectangle {
        id: background;
        color: "#ffffff"
        anchors.fill: parent;

        Behavior on color {

            ColorAnimation {
                duration: 5000
            }
        }

        z: -3

    }

    property var model: (contr)? contr: null;
    property var arrayObjects: []
    property bool showMenu: false
    property bool isPause: false

    function add (cppObjId) {
        if (!model) {
            console.log("create object fail")
            return;
        }
        var objModel = model.getGameObject(cppObjId);

        if (!objModel) {
            console.log("object model not found");
            return;
        }

        var viewTemplate = objModel.viewTemplate;

        var temp = Qt.createComponent( viewTemplate + ".qml")
        if (temp.status === Component.Ready) {
            var obj = temp.createObject(parent) // parent - это обьект на который будет помещен соззданный элемент
            obj.model = model.getGameObject(cppObjId);
            obj.z = -2;
            arrayObjects.push(obj)
        } else {
            console.log("wrong viewTemplate in model");
        }
    }

    function remove(id) {
        if (typeof id !== "number" || id < 0) {
            console.log("id not found");
            return;
        }

        for (var i = 0; i < arrayObjects.length; ++i) {
            if (id === arrayObjects[i].guiId) {
                arrayObjects.splice(i,1);
            }
        }
    }

    function setAuto (auto) {
        if (auto && model) {
            model.newGame();

        }
        showMenu = (auto && model)
        autoTimer.running = auto && model;
    }

    function updateBackgroundColor(lvl) {
        switch(lvl % 7) {
        case 0: background.color = "#d6eaf8"; break;
        case 1: background.color = "#d0ece7"; break;
        case 2: background.color = "#d4efdf"; break;
        case 3: background.color = "#fcf3cf"; break;
        case 4: background.color = "#f6ddcc"; break;
        case 5: background.color = "#f2d7d5"; break;
        case 6: background.color = "#ebdef0"; break;
        case 7: background.color = "#fbfcfc"; break;

        }
    }


    Timer {
        id :autoTimer;
        repeat: true;
        running: false;
        interval: 1000
        onTriggered: {
            interval = Math.random() * 600
            scene.model.buttonPress();
        }
    }

    Connections {
        target: model;
        onGameObjectsChanged: {
            if (!dif) {
                console.log("dif not found");
                return;
            }

            var tempDifRem = [];
            tempDifRem = dif.getRemoveIds();
            var tempDifAdd = [];
            tempDifAdd = dif.getAddedIds();

            for (var i = 0; i < tempDifAdd.length; ++i) {
                add(tempDifAdd[i]);
            }

            for (i = 0; i < tempDifRem.length; ++i) {
                remove(tempDifRem[i]);
            }
        }

        onFinished: {

            var isVictory = victory;
            var gameLvl = lvl + 1;
            var dist = distance;
            updateBackgroundColor(gameLvl);

            if (isVictory ) {

                if (!autoTimer.running)
                    notification.show(qsTr(" Next Lvl!!!"),
                                  qsTr(" You anblock next lvl (" + gameLvl + ")" ),
                                  "qrc:/texture/up");

                model.nextLvl();
            } else if (autoTimer.running) {
                model.newGame();
            } else {
                showMenu = true;
            }
        }
    }

    Component.onCompleted: {
        updateBackgroundColor(0);
    }

    MouseArea {
        anchors.fill: parent;

        onClicked: {
            if (!model) {
                console.log("model not found");
                return;
            }

            model.buttonPress();
        }
    }

    NotificationForm {
        z: -1
        id: notification;
        margin: mainWindow.point;

        x: parent.width - width - margin;
        y: margin;

        width: 40 * mainWindow.point;
        height: width * 0.5

    }

    Button {
        id: returnToMenu;

        text: "<<"

        anchors.left: parent.left
        anchors.leftMargin: point

        anchors.top: parent.top
        anchors.topMargin: point
        z: 1

        onClicked: {
            showMenu = true;
        }

        visible: !showMenu
    }

    Button {
        id: pause

        text: (isPause)?  "▶" :"||"

        anchors.left: returnToMenu.right
        anchors.leftMargin: point

        anchors.top: parent.top
        anchors.topMargin: point
        z: returnToMenu.z

        onClicked: {
            isPause = !isPause;
            if (model) model.setPause(isPause);
        }

        visible: !showMenu

    }

    Button {
        id: long_
        Label {
            anchors.fill: parent;

            textFormat: Text.AutoText
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap

            text: qsTr("lvl long: ") + ((model)? model.long_: "0")
        }

        width: 35 * point;
        height: pause.height;

        anchors.left: pause.right
        anchors.leftMargin: point

        anchors.top: parent.top
        anchors.topMargin: point
        z: returnToMenu.z

        visible: !showMenu

    }

    Button {
        Label {
            anchors.fill: parent;

            textFormat: Text.AutoText
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap

            text: qsTr("general long: ") + ((model)? model.generalLong: "0")
        }

        width: 35 * point;
        height: long_.height;

        anchors.left: long_.right
        anchors.leftMargin: point

        anchors.top: parent.top
        anchors.topMargin: point
        z: returnToMenu.z

        visible: !showMenu

    }
}
