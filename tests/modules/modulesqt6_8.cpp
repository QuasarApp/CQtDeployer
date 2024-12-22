/*
 * Copyright (C) 2023-2024 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */


#include "modulesqt6_8.h"

ModulesQt6_8::ModulesQt6_8() {}

QSet<QString> ModulesQt6_8::qmlLibs(const QString &distDir) const {
    TestUtils utils;
    auto Tree = ModulesQt6_6::qmlLibs(distDir);

#ifdef Q_OS_LINUX
    Tree -= utils.createTree(
        {
            "./" + distDir + "/lib/libQt6Pdf.so" ,
            "./" + distDir + "/plugins/imageformats/libqpdf.so" ,
            "./" + distDir + "/qml/QtQml/Base/libqmlplugin.so" ,
            "./" + distDir + "/qml/QtQml/Base/plugins.qmltypes" ,
            "./" + distDir + "/qml/QtQml/Base/qmldir",
            "./" + distDir + "/qml/QtQml/libqmlmetaplugin.so",
            "./" + distDir + "/qml/QtQuick/Controls/Imagine/impl/qtquickcontrols2imaginestyleimplplugin.qmltypes" ,
        }
        );

    Tree += utils.createTree(
        {
            "./" + distDir + "//lib/libQt6Network.so",
            "./" + distDir + "//lib/libQt6QmlMeta.so",
            "./" + distDir + "//lib/libQt6QuickControls2Basic.so",
            "./" + distDir + "//lib/libQt6QuickControls2BasicStyleImpl.so",
            "./" + distDir + "//lib/libQt6QuickControls2FluentWinUI3StyleImpl.so",
            "./" + distDir + "//lib/libQt6QuickControls2Fusion.so",
            "./" + distDir + "//lib/libQt6QuickControls2FusionStyleImpl.so",
            "./" + distDir + "//lib/libQt6QuickControls2Imagine.so",
            "./" + distDir + "//lib/libQt6QuickControls2ImagineStyleImpl.so",
            "./" + distDir + "//lib/libQt6QuickControls2Material.so",
            "./" + distDir + "//lib/libQt6QuickControls2MaterialStyleImpl.so",
            "./" + distDir + "//lib/libQt6QuickControls2Universal.so",
            "./" + distDir + "//lib/libQt6QuickControls2UniversalStyleImpl.so",
            "./" + distDir + "//lib/libQt6QuickEffects.so",
            "./" + distDir + "//lib/libQt6QuickLayouts.so",
            "./" + distDir + "//qml/QML/plugins.qmltypes",
            "./" + distDir + "//qml/QML/qmldir",
            "./" + distDir + "//qml/QtQml/libqmlplugin.so",
            "./" + distDir + "//qml/QtQml/plugins.qmltypes",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/ApplicationWindow.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/BusyIndicator.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/Button.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/CheckBox.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/CheckDelegate.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/ComboBox.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/Config.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/DelayButton.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/Dialog.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/DialogButtonBox.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/FocusFrame.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/Frame.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/GroupBox.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/ItemDelegate.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/Menu.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/MenuBar.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/MenuBarItem.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/MenuItem.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/MenuSeparator.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/PageIndicator.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/Popup.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/ProgressBar.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/RadioButton.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/RadioDelegate.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/RangeSlider.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/RoundButton.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/Slider.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/SpinBox.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/StyleImage.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/SwipeDelegate.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/Switch.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/SwitchDelegate.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/TabBar.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/TabButton.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/TextArea.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/TextField.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/ToolBar.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/ToolButton.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/ToolSeparator.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/ToolTip.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/impl/ButtonBackground.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/impl/CheckIndicator.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/impl/RadioIndicator.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/impl/SwitchIndicator.qml",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/impl/libqtquickcontrols2fluentwinui3styleimplplugin.so",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/impl/plugins.qmltypes",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/impl/qmldir",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/libqtquickcontrols2fluentwinui3styleplugin.so",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/plugins.qmltypes",
            "./" + distDir + "//qml/QtQuick/Controls/FluentWinUI3/qmldir",
            "./" + distDir + "//qml/QtQuick/Controls/Imagine/impl/QuickControls2ImagineStyleImpl.qmltypes",
            "./" + distDir + "//qml/QtQuick/Effects/libeffectsplugin.so",
            "./" + distDir + "//qml/QtQuick/Effects/plugins.qmltypes",
            "./" + distDir + "//qml/QtQuick/Effects/qmldir",
            "./" + distDir + "//qml/QtQuick/Layouts/libqquicklayoutsplugin.so",
            "./" + distDir + "//qml/QtQuick/Layouts/plugins.qmltypes",
            "./" + distDir + "//qml/QtQuick/Layouts/qmldir",
            "./" + distDir + "//translations/qtdeclarative_ka.qm"
        }
        );
#endif

    return Tree;
}

QSet<QString> ModulesQt6_8::qmlVirtualKeyBoadrLibs(const QString &distDir) const{
    TestUtils utils;

    auto Tree = ModulesQt6_6::qmlVirtualKeyBoadrLibs(distDir);
#ifdef Q_OS_LINUX
    Tree -= utils.createTree(
        {
            "./" + distDir + "plugins/multimedia/libgstreamermediaplugin.so"
        }
        );

    Tree += utils.createTree(
        {
            "./" + distDir + "/translations/qtmultimedia_ka.qm",
            "./" + distDir + "/lib/libQt6VirtualKeyboardSettings.so"
        }
        );
#endif
    return Tree;
}

QSet<QString> ModulesQt6_8::qtWebEngine(const QString &distDir) const{
    TestUtils utils;

    auto Tree = ModulesQt6_6::qtWebEngine(distDir);

#ifdef Q_OS_LINUX
    Tree -= utils.createTree(
        {
            "./" + distDir + "/lib/libQt6Pdf.so" ,
            "./" + distDir + "/plugins/imageformats/libqpdf.so"
        }
        );

    Tree += utils.createTree(
        {
            "./" + distDir + "/lib/libQt6Network.so",
            "./" + distDir + "/lib/libQt6QmlMeta.so",
            "./" + distDir + "/lib/libQt6QmlWorkerScript.so",
            "./" + distDir + "/translations/qtdeclarative_ka.qm",
            "./" + distDir + "/translations/qtserialport_ka.qm",
            "./" + distDir + "/translations/qtwebengine_ka.qm"
        }
        );
#endif
    return Tree;
}

QSet<QString> ModulesQt6_8::qtWebEngineWidgets(const QString &distDir) const{
    TestUtils utils;

    auto Tree = ModulesQt6_6::qtWebEngineWidgets(distDir);

#ifdef Q_OS_LINUX


    Tree += utils.createTree(
        {
            "./" + distDir + "/lib/libQt6Network.so",
            "./" + distDir + "/lib/libQt6QmlMeta.so",
            "./" + distDir + "/lib/libQt6QmlWorkerScript.so",
            "./" + distDir + "/translations/qtdeclarative_ka.qm",
            "./" + distDir + "/translations/qtserialport_ka.qm",
            "./" + distDir + "/translations/qtwebengine_ka.qm"
        }
        );

    Tree -= utils.createTree(
        {
            "./" + distDir + "/lib/libQt6Pdf.so" ,
            "./" + distDir + "/plugins/imageformats/libqpdf.so"
        }
        );

#endif
    return Tree;
}

QSet<QString> ModulesQt6_8::qtLibs(const QString &distDir) const {
    auto Tree = ModulesQt6_6::qtLibs(distDir);
    TestUtils utils;

#ifdef Q_OS_LINUX
    Tree -= utils.createTree(
        {
            "./" + distDir + "/lib/libQt6Network.so",
            "./" + distDir + "/lib/libQt6Pdf.so"
                             "./" + distDir + "/plugins/imageformats/libqpdf.so"
        }
        );

    Tree += utils.createTree(
        {
            "./" + distDir + "/translations/qtbase_ka.qm",
            "./" + distDir + "/plugins/wayland-decoration-client/libadwaita.so",
        }
        );
#endif

    return Tree;
}
