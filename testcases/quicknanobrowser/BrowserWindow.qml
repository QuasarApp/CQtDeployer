/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWebEngine module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt.labs.settings
import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import QtWebEngine

ApplicationWindow {
    id: browserWindow
    property QtObject applicationRoot
    property Item currentWebView: tabBar.currentIndex < tabBar.count ? tabLayout.children[tabBar.currentIndex] : null
    property int previousVisibility: Window.Windowed
    property int createdTabs: 0

    width: 1300
    height: 900
    visible: true
    title: currentWebView && currentWebView.title

    // Make sure the Qt.WindowFullscreenButtonHint is set on OS X.
    Component.onCompleted: flags = flags | Qt.WindowFullscreenButtonHint

    onCurrentWebViewChanged: {
        findBar.reset();
    }

    // When using style "mac", ToolButtons are not supposed to accept focus.
    property bool platformIsMac: Qt.platform.os == "osx"

    Settings {
        id : appSettings
        property alias autoLoadImages: loadImages.checked
        property alias javaScriptEnabled: javaScriptEnabled.checked
        property alias errorPageEnabled: errorPageEnabled.checked
        property alias pluginsEnabled: pluginsEnabled.checked
        property alias fullScreenSupportEnabled: fullScreenSupportEnabled.checked
        property alias autoLoadIconsForPage: autoLoadIconsForPage.checked
        property alias touchIconsEnabled: touchIconsEnabled.checked
        property alias webRTCPublicInterfacesOnly : webRTCPublicInterfacesOnly.checked
        property alias devToolsEnabled: devToolsEnabled.checked
        property alias pdfViewerEnabled: pdfViewerEnabled.checked
    }

    Action {
        shortcut: "Ctrl+D"
        onTriggered: {
            downloadView.visible = !downloadView.visible;
        }
    }
    Action {
        id: focus
        shortcut: "Ctrl+L"
        onTriggered: {
            addressBar.forceActiveFocus();
            addressBar.selectAll();
        }
    }
    Action {
        shortcut: StandardKey.Refresh
        onTriggered: {
            if (currentWebView)
                currentWebView.reload();
        }
    }
    Action {
        shortcut: StandardKey.AddTab
        onTriggered: {
            tabBar.createTab(tabBar.count != 0 ? currentWebView.profile : defaultProfile);
            addressBar.forceActiveFocus();
            addressBar.selectAll();
        }
    }
    Action {
        shortcut: StandardKey.Close
        onTriggered: {
            currentWebView.triggerWebAction(WebEngineView.RequestClose);
        }
    }
    Action {
        shortcut: StandardKey.Quit
        onTriggered: browserWindow.close()
    }
    Action {
        shortcut: "Escape"
        onTriggered: {
            if (currentWebView.state == "FullScreen") {
                browserWindow.visibility = browserWindow.previousVisibility;
                fullScreenNotification.hide();
                currentWebView.triggerWebAction(WebEngineView.ExitFullScreen);
            }

            if (findBar.visible)
                findBar.visible = false;
        }
    }
    Action {
        shortcut: "Ctrl+0"
        onTriggered: currentWebView.zoomFactor = 1.0
    }
    Action {
        shortcut: StandardKey.ZoomOut
        onTriggered: currentWebView.zoomFactor -= 0.1
    }
    Action {
        shortcut: StandardKey.ZoomIn
        onTriggered: currentWebView.zoomFactor += 0.1
    }

    Action {
        shortcut: StandardKey.Copy
        onTriggered: currentWebView.triggerWebAction(WebEngineView.Copy)
    }
    Action {
        shortcut: StandardKey.Cut
        onTriggered: currentWebView.triggerWebAction(WebEngineView.Cut)
    }
    Action {
        shortcut: StandardKey.Paste
        onTriggered: currentWebView.triggerWebAction(WebEngineView.Paste)
    }
    Action {
        shortcut: "Shift+"+StandardKey.Paste
        onTriggered: currentWebView.triggerWebAction(WebEngineView.PasteAndMatchStyle)
    }
    Action {
        shortcut: StandardKey.SelectAll
        onTriggered: currentWebView.triggerWebAction(WebEngineView.SelectAll)
    }
    Action {
        shortcut: StandardKey.Undo
        onTriggered: currentWebView.triggerWebAction(WebEngineView.Undo)
    }
    Action {
        shortcut: StandardKey.Redo
        onTriggered: currentWebView.triggerWebAction(WebEngineView.Redo)
    }
    Action {
        shortcut: StandardKey.Back
        onTriggered: currentWebView.triggerWebAction(WebEngineView.Back)
    }
    Action {
        shortcut: StandardKey.Forward
        onTriggered: currentWebView.triggerWebAction(WebEngineView.Forward)
    }
    Action {
        shortcut: StandardKey.Find
        onTriggered: {
            if (!findBar.visible)
                findBar.visible = true;
        }
    }
    Action {
        shortcut: StandardKey.FindNext
        onTriggered: findBar.findNext()
    }
    Action {
        shortcut: StandardKey.FindPrevious
        onTriggered: findBar.findPrevious()
    }

    menuBar: ToolBar {
        id: navigationBar
        RowLayout {
            anchors.fill: parent
            ToolButton {
                enabled: currentWebView && (currentWebView.canGoBack || currentWebView.canGoForward)
                onClicked: historyMenu.open()
                text: qsTr("▼")
                Menu {
                    id: historyMenu
                    Instantiator {
                        model: currentWebView && currentWebView.history.items
                        MenuItem {
                            text: model.title
                            onTriggered: currentWebView.goBackOrForward(model.offset)
                            checkable: !enabled
                            checked: !enabled
                            enabled: model.offset
                        }

                        onObjectAdded: function(index, object) {
                            historyMenu.insertItem(index, object)
                        }
                        onObjectRemoved: function(index, object) {
                            historyMenu.removeItem(object)
                        }
                    }
                }
            }

            ToolButton {
                id: backButton
                icon.source: "qrc:/icons/go-previous.png"
                onClicked: currentWebView.goBack()
                enabled: currentWebView && currentWebView.canGoBack
                activeFocusOnTab: !browserWindow.platformIsMac
            }
            ToolButton {
                id: forwardButton
                icon.source: "qrc:/icons/go-next.png"
                onClicked: currentWebView.goForward()
                enabled: currentWebView && currentWebView.canGoForward
                activeFocusOnTab: !browserWindow.platformIsMac
            }
            ToolButton {
                id: reloadButton
                icon.source: currentWebView && currentWebView.loading ? "qrc:/icons/process-stop.png" : "qrc:/icons/view-refresh.png"
                onClicked: currentWebView && currentWebView.loading ? currentWebView.stop() : currentWebView.reload()
                activeFocusOnTab: !browserWindow.platformIsMac
            }
            TextField {
                id: addressBar
                Image {
                    anchors.verticalCenter: addressBar.verticalCenter;
                    x: 5
                    z: 2
                    id: faviconImage
                    width: 16; height: 16
                    sourceSize: Qt.size(width, height)
                    source: currentWebView && currentWebView.icon ? currentWebView.icon : ''
                }
                MouseArea {
                    id: textFieldMouseArea
                    acceptedButtons: Qt.RightButton
                    anchors.fill: parent
                    onClicked: {
                        var textSelectionStartPos = addressBar.selectionStart;
                        var textSelectionEndPos = addressBar.selectionEnd;
                        textFieldContextMenu.open();
                        addressBar.select(textSelectionStartPos, textSelectionEndPos);
                    }
                    Menu {
                        id: textFieldContextMenu
                        x: textFieldMouseArea.mouseX
                        y: textFieldMouseArea.mouseY
                        MenuItem {
                            text: qsTr("Cut")
                            onTriggered: addressBar.cut()
                            enabled: addressBar.selectedText.length > 0
                        }
                        MenuItem {
                            text: qsTr("Copy")
                            onTriggered: addressBar.copy()
                            enabled: addressBar.selectedText.length > 0
                        }
                        MenuItem {
                            text: qsTr("Paste")
                            onTriggered: addressBar.paste()
                            enabled: addressBar.canPaste
                        }
                        MenuItem {
                            text: qsTr("Delete")
                            onTriggered: addressBar.text = qsTr("")
                            enabled: addressBar.selectedText.length > 0
                        }
                        MenuSeparator {}
                        MenuItem {
                            text: qsTr("Select All")
                            onTriggered: addressBar.selectAll()
                            enabled: addressBar.text.length > 0
                        }
                    }
                }
                leftPadding: 26
                focus: true
                Layout.fillWidth: true
                Binding on text {
                    when: currentWebView
                    value: currentWebView.url
                }
                onAccepted: currentWebView.url = utils.fromUserInput(text)
                selectByMouse: true
            }
            ToolButton {
                id: settingsMenuButton
                text: qsTr("⋮")
                onClicked: settingsMenu.open()
                Menu {
                    id: settingsMenu
                    y: settingsMenuButton.height
                    MenuItem {
                        id: loadImages
                        text: "Autoload images"
                        checkable: true
                        checked: WebEngine.settings.autoLoadImages
                    }
                    MenuItem {
                        id: javaScriptEnabled
                        text: "JavaScript On"
                        checkable: true
                        checked: WebEngine.settings.javascriptEnabled
                    }
                    MenuItem {
                        id: errorPageEnabled
                        text: "ErrorPage On"
                        checkable: true
                        checked: WebEngine.settings.errorPageEnabled
                    }
                    MenuItem {
                        id: pluginsEnabled
                        text: "Plugins On"
                        checkable: true
                        checked: true
                    }
                    MenuItem {
                        id: fullScreenSupportEnabled
                        text: "FullScreen On"
                        checkable: true
                        checked: WebEngine.settings.fullScreenSupportEnabled
                    }
                    MenuItem {
                        id: offTheRecordEnabled
                        text: "Off The Record"
                        checkable: true
                        checked: currentWebView && currentWebView.profile === otrProfile
                        onToggled: function(checked) {
                            if (currentWebView) {
                                currentWebView.profile = checked ? otrProfile : defaultProfile;
                            }
                        }
                    }
                    MenuItem {
                        id: httpDiskCacheEnabled
                        text: "HTTP Disk Cache"
                        checkable: currentWebView && !currentWebView.profile.offTheRecord
                        checked: currentWebView && (currentWebView.profile.httpCacheType === WebEngineProfile.DiskHttpCache)
                        onToggled: function(checked) {
                            if (currentWebView) {
                                currentWebView.profile.httpCacheType = checked ? WebEngineProfile.DiskHttpCache : WebEngineProfile.MemoryHttpCache;
                            }
                        }
                    }
                    MenuItem {
                        id: autoLoadIconsForPage
                        text: "Icons On"
                        checkable: true
                        checked: WebEngine.settings.autoLoadIconsForPage
                    }
                    MenuItem {
                        id: touchIconsEnabled
                        text: "Touch Icons On"
                        checkable: true
                        checked: WebEngine.settings.touchIconsEnabled
                        enabled: autoLoadIconsForPage.checked
                    }
                    MenuItem {
                        id: webRTCPublicInterfacesOnly
                        text: "WebRTC Public Interfaces Only"
                        checkable: true
                        checked: WebEngine.settings.webRTCPublicInterfacesOnly
                    }
                    MenuItem {
                        id: devToolsEnabled
                        text: "Open DevTools"
                        checkable: true
                        checked: false
                    }
                    MenuItem {
                        id: pdfViewerEnabled
                        text: "PDF viewer enabled"
                        checkable: true
                        checked: WebEngine.settings.pdfViewerEnabled
                    }
                }
            }
        }
        ProgressBar {
            id: progressBar
            height: 3
            anchors {
                left: parent.left
                top: parent.bottom
                right: parent.right
                leftMargin: parent.leftMargin
                rightMargin: parent.rightMargin
            }
            background: Item {}
            z: -2
            from: 0
            to: 100
            value: (currentWebView && currentWebView.loadProgress < 100) ? currentWebView.loadProgress : 0
        }
    }

    StackLayout {
        id: tabLayout
        currentIndex: tabBar.currentIndex

        anchors.top: tabBar.bottom
        anchors.bottom: devToolsView.top
        anchors.left: parent.left
        anchors.right: parent.right
    }

    Component {
        id: tabButtonComponent

        TabButton {
            property color frameColor: "#999"
            property color fillColor: "#eee"
            property color nonSelectedColor: "#ddd"
            property string tabTitle: "New Tab"

            id: tabButton
            contentItem: Rectangle {
                id: tabRectangle
                color: tabButton.down ? fillColor : nonSelectedColor
                border.width: 1
                border.color: frameColor
                implicitWidth: Math.max(text.width + 30, 80)
                implicitHeight: Math.max(text.height + 10, 20)
                Rectangle { height: 1 ; width: parent.width ; color: frameColor}
                Rectangle { height: parent.height ; width: 1; color: frameColor}
                Rectangle { x: parent.width - 2; height: parent.height ; width: 1; color: frameColor}
                Text {
                    id: text
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 6
                    text: tabButton.tabTitle
                    elide: Text.ElideRight
                    color: tabButton.down ? "black" : frameColor
                    width: parent.width - button.background.width
                }
                Button {
                    id: button
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.rightMargin: 4
                    height: 12
                    background: Rectangle {
                        implicitWidth: 12
                        implicitHeight: 12
                        color: button.hovered ? "#ccc" : tabRectangle.color
                        Text {text: "x"; anchors.centerIn: parent; color: "gray"}
                    }
                    onClicked: tabButton.closeTab()
                }
            }

            onClicked: addressBar.text = tabLayout.itemAt(TabBar.index).url;
            function closeTab() {
                tabBar.removeView(TabBar.index);
            }
        }
    }

    TabBar {
        id: tabBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        Component.onCompleted: createTab(defaultProfile)

        function createTab(profile, focusOnNewTab = true, url = undefined) {
            var webview = tabComponent.createObject(tabLayout, {profile: profile});
            var newTabButton = tabButtonComponent.createObject(tabBar, {tabTitle: Qt.binding(function () { return webview.title; })});
            tabBar.addItem(newTabButton);
            if (focusOnNewTab) {
                tabBar.setCurrentIndex(tabBar.count - 1);
            }
            if (url !== undefined) {
                webview.url = url;
            }
            return webview;
        }

        function removeView(index) {
            tabBar.removeItem(index);
            if (tabBar.count > 1) {
                tabBar.removeItem(tabBar.itemAt(index));
                tabLayout.children[index].destroy();
            } else {
                browserWindow.close();
            }
        }

        Component {
            id: tabComponent
            WebEngineView {
                id: webEngineView
                focus: true

                onLinkHovered: function(hoveredUrl) {
                    if (hoveredUrl == "")
                        hideStatusText.start();
                    else {
                        statusText.text = hoveredUrl;
                        statusBubble.visible = true;
                        hideStatusText.stop();
                    }
                }

                states: [
                    State {
                        name: "FullScreen"
                        PropertyChanges {
                            target: tabBar
                            visible: false
                            height: 0
                        }
                        PropertyChanges {
                            target: navigationBar
                            visible: false
                        }
                    }
                ]
                settings.autoLoadImages: appSettings.autoLoadImages
                settings.javascriptEnabled: appSettings.javaScriptEnabled
                settings.errorPageEnabled: appSettings.errorPageEnabled
                settings.pluginsEnabled: appSettings.pluginsEnabled
                settings.fullScreenSupportEnabled: appSettings.fullScreenSupportEnabled
                settings.autoLoadIconsForPage: appSettings.autoLoadIconsForPage
                settings.touchIconsEnabled: appSettings.touchIconsEnabled
                settings.webRTCPublicInterfacesOnly: appSettings.webRTCPublicInterfacesOnly
                settings.pdfViewerEnabled: appSettings.pdfViewerEnabled

                onCertificateError: function(error) {
                    error.defer();
                    sslDialog.enqueue(error);
                }

                onNewWindowRequested: function(request) {
                    if (!request.userInitiated)
                        console.warn("Blocked a popup window.");
                    else if (request.destination === WebEngineNewWindowRequest.InNewTab) {
                        var tab = tabBar.createTab(currentWebView.profile, true, request.requestedUrl);
                        tab.acceptAsNewWindow(request);
                    } else if (request.destination === WebEngineNewWindowRequest.InNewBackgroundTab) {
                        var backgroundTab = tabBar.createTab(currentWebView.profile, false);
                        backgroundTab.acceptAsNewWindow(request);
                    } else if (request.destination === WebEngineNewWindowRequest.InNewDialog) {
                        var dialog = applicationRoot.createDialog(currentWebView.profile);
                        dialog.currentWebView.acceptAsNewWindow(request);
                    } else {
                        var window = applicationRoot.createWindow(currentWebView.profile);
                        window.currentWebView.acceptAsNewWindow(request);
                    }
                }

                onFullScreenRequested: function(request) {
                    if (request.toggleOn) {
                        webEngineView.state = "FullScreen";
                        browserWindow.previousVisibility = browserWindow.visibility;
                        browserWindow.showFullScreen();
                        fullScreenNotification.show();
                    } else {
                        webEngineView.state = "";
                        browserWindow.visibility = browserWindow.previousVisibility;
                        fullScreenNotification.hide();
                    }
                    request.accept();
                }

                onQuotaRequested: function(request) {
                    if (request.requestedSize <= 5 * 1024 * 1024)
                        request.accept();
                    else
                        request.reject();
                }

                onRegisterProtocolHandlerRequested: function(request) {
                    console.log("accepting registerProtocolHandler request for "
                                + request.scheme + " from " + request.origin);
                    request.accept();
                }

                onRenderProcessTerminated: function(terminationStatus, exitCode) {
                    var status = "";
                    switch (terminationStatus) {
                    case WebEngineView.NormalTerminationStatus:
                        status = "(normal exit)";
                        break;
                    case WebEngineView.AbnormalTerminationStatus:
                        status = "(abnormal exit)";
                        break;
                    case WebEngineView.CrashedTerminationStatus:
                        status = "(crashed)";
                        break;
                    case WebEngineView.KilledTerminationStatus:
                        status = "(killed)";
                        break;
                    }

                    print("Render process exited with code " + exitCode + " " + status);
                    reloadTimer.running = true;
                }

                onSelectClientCertificate: function(selection) {
                    selection.certificates[0].select();
                }

                onFindTextFinished: function(result) {
                    if (!findBar.visible)
                        findBar.visible = true;

                    findBar.numberOfMatches = result.numberOfMatches;
                    findBar.activeMatch = result.activeMatch;
                }

                onLoadingChanged: function(loadRequest) {
                    if (loadRequest.status == WebEngineView.LoadStartedStatus)
                        findBar.reset();
                }

                Timer {
                    id: reloadTimer
                    interval: 0
                    running: false
                    repeat: false
                    onTriggered: currentWebView.reload()
                }
            }
        }
    }
    WebEngineView {
        id: devToolsView
        visible: devToolsEnabled.checked
        height: visible ? 400 : 0
        inspectedView: visible && tabBar.currentIndex < tabBar.count ? tabLayout.children[tabBar.currentIndex] : null
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        onNewWindowRequested: function(request) {
            var tab = tabBar.createTab(currentWebView.profile);
            request.openIn(tab);
        }

        Timer {
            id: hideTimer
            interval: 0
            running: false
            repeat: false
            onTriggered: devToolsEnabled.checked = false
        }
        onWindowCloseRequested: function(request) {
            // Delay hiding for keep the inspectedView set to receive the ACK message of close.
            hideTimer.running = true;
        }
    }
    Dialog {
        id: sslDialog
        anchors.centerIn: parent
        contentWidth: Math.max(mainText.width, detailedText.width)
        contentHeight: mainText.height + detailedText.height
        property var certErrors: []
        // fixme: icon!
        // icon: StandardIcon.Warning
        standardButtons: Dialog.No | Dialog.Yes
        title: "Server's certificate not trusted"
        contentItem: Item {
            id: textContentItem
            Label {
                id: mainText
                text: "Do you wish to continue?"
            }
            Text {
                id: detailedText
                anchors.top: mainText.bottom
                text: "If you wish so, you may continue with an unverified certificate.\n" +
                      "Accepting an unverified certificate means\n" +
                      "you may not be connected with the host you tried to connect to.\n" +
                      "Do you wish to override the security check and continue?"
            }
        }

        onAccepted: {
            certErrors.shift().acceptCertificate();
            presentError();
        }
        onRejected: reject()

        function reject(){
            certErrors.shift().rejectCertificate();
            presentError();
        }
        function enqueue(error){
            certErrors.push(error);
            presentError();
        }
        function presentError(){
            visible = certErrors.length > 0
        }
    }

    FullScreenNotification {
        id: fullScreenNotification
    }

    DownloadView {
        id: downloadView
        visible: false
        anchors.fill: parent
    }

    function onDownloadRequested(download) {
        downloadView.visible = true;
        downloadView.append(download);
        download.accept();
    }

    FindBar {
        id: findBar
        visible: false
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.top: parent.top

        onFindNext: {
            if (text)
                currentWebView && currentWebView.findText(text);
            else if (!visible)
                visible = true;
        }
        onFindPrevious: {
            if (text)
                currentWebView && currentWebView.findText(text, WebEngineView.FindBackward);
            else if (!visible)
                visible = true;
        }
    }


    Rectangle {
        id: statusBubble
        color: "oldlace"
        property int padding: 8
        visible: false

        anchors.left: parent.left
        anchors.bottom: parent.bottom
        width: statusText.paintedWidth + padding
        height: statusText.paintedHeight + padding

        Text {
            id: statusText
            anchors.centerIn: statusBubble
            elide: Qt.ElideMiddle

            Timer {
                id: hideStatusText
                interval: 750
                onTriggered: {
                    statusText.text = "";
                    statusBubble.visible = false;
                }
            }
        }
    }
}
