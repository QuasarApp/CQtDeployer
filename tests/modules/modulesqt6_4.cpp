/*
 * Copyright (C) 2023-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "modulesqt6_4.h"

ModulesQt6_4::ModulesQt6_4()
{

}

QSet<QString> ModulesQt6_4::qmlLibs(const QString &distDir) const {
    TestUtils utils;
    auto Tree = ModulesQt63::qmlLibs(distDir);

    Tree -= utils.createTree(
        {
            "./" + distDir + "/qml/QtQuick/Controls/Imagine/impl/plugins.qmltypes"
        }
    );

    Tree += utils.createTree(
        {
         "./" + distDir + "/qml/QtQuick/Controls/Fusion/TreeViewDelegate.qml",
         "./" + distDir + "/qml/QtQuick/Controls/Imagine/impl/qtquickcontrols2imaginestyleimplplugin.qmltypes",
         "./" + distDir + "/qml/QtQuick/Controls/Material/TreeViewDelegate.qml"
        }
    );

    return Tree;
}

QSet<QString> ModulesQt6_4::qmlVirtualKeyBoadrLibs(const QString &distDir) const {

    TestUtils utils;

    auto Tree = ModulesQt63::qmlVirtualKeyBoadrLibs(distDir);

    Tree -= utils.createTree({
        "./" + distDir + "/plugins/virtualkeyboard/libqtvirtualkeyboard_hangul.so",
        "./" + distDir + "/plugins/virtualkeyboard/libqtvirtualkeyboard_openwnn.so",
        "./" + distDir + "/plugins/virtualkeyboard/libqtvirtualkeyboard_pinyin.so",
        "./" + distDir + "/plugins/virtualkeyboard/libqtvirtualkeyboard_tcime.so",
        "./" + distDir + "/plugins/virtualkeyboard/libqtvirtualkeyboard_thai.so",
        "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Settings/libqtquickvirtualkeyboardsettingsplugin.so",
        "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Styles/libqtquickvirtualkeyboardstylesplugin.so",
        "./" + distDir + "/qml/QtQuick/VirtualKeyboard/libqtquickvirtualkeyboardplugin.so",
    });

    Tree += utils.createTree({
             "./" + distDir + "/lib/libQt6Multimedia.so",
             "./" + distDir + "/lib/libQt6MultimediaQuick.so",
             "./" + distDir + "/qml/QtMultimedia/Video.qml",
             "./" + distDir + "/qml/QtMultimedia/libquickmultimediaplugin.so",
             "./" + distDir + "/qml/QtMultimedia/plugins.qmltypes",
             "./" + distDir + "/qml/QtMultimedia/qmldir",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/AlternativeKeys.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/BackspaceKey.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/BaseKey.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/ChangeLanguageKey.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/CharacterPreviewBubble.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/EnterKey.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/FillerKey.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/FlickKey.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/FunctionPopupList.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/HandwritingModeKey.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/HideKeyboardKey.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/InputModeKey.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/Key.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/Keyboard.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/KeyboardColumn.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/KeyboardLayout.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/KeyboardLayoutLoader.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/KeyboardRow.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/ModeKey.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/MultiSoundEffect.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/MultitapInputMethod.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/NumberKey.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/PopupList.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/SelectionControl.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/ShadowInputControl.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/ShiftKey.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/SpaceKey.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/SymbolModeKey.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/TraceInputArea.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/TraceInputKey.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/WordCandidatePopupList.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/libqtvkbcomponentsplugin.so",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/qmldir",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Components/qtvkbcomponentsplugin.qmltypes",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/EnterKey.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/HandwritingInputPanel.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/InputPanel.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Layouts/libqtvkblayoutsplugin.so",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Layouts/qmldir",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Layouts/qtvkblayoutsplugin.qmltypes",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Plugins/Hangul/libqtvkbhangulplugin.so",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Plugins/Hangul/plugins.qmltypes",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Plugins/Hangul/qmldir",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Plugins/OpenWNN/libqtvkbopenwnnplugin.so",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Plugins/OpenWNN/plugins.qmltypes",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Plugins/OpenWNN/qmldir",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Plugins/Pinyin/libqtvkbpinyinplugin.so",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Plugins/Pinyin/plugins.qmltypes",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Plugins/Pinyin/qmldir",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Plugins/TCIme/libqtvkbtcimeplugin.so",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Plugins/TCIme/plugins.qmltypes",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Plugins/TCIme/qmldir",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Plugins/Thai/libqtvkbthaiplugin.so",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Plugins/Thai/plugins.qmltypes",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Plugins/Thai/qmldir",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Plugins/libqtvkbpluginsplugin.so",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Plugins/qmldir",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Plugins/qtvkbpluginsplugin.qmltypes",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Settings/libqtvkbsettingsplugin.so",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Styles/Builtin/libqtvkbbuiltinstylesplugin.so",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Styles/Builtin/plugins.qmltypes",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Styles/Builtin/qmldir",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Styles/KeyIcon.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Styles/KeyPanel.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Styles/KeyboardStyle.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Styles/SelectionListItem.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Styles/TraceCanvas.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Styles/TraceInputKeyPanel.qml",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Styles/TraceUtils.js",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/Styles/libqtvkbstylesplugin.so",
             "./" + distDir + "/qml/QtQuick/VirtualKeyboard/libqtvkbplugin.so",
             "./" + distDir + "/translations/qtmultimedia_ar.qm",
             "./" + distDir + "/translations/qtmultimedia_bg.qm",
             "./" + distDir + "/translations/qtmultimedia_ca.qm",
             "./" + distDir + "/translations/qtmultimedia_cs.qm",
             "./" + distDir + "/translations/qtmultimedia_da.qm",
             "./" + distDir + "/translations/qtmultimedia_de.qm",
             "./" + distDir + "/translations/qtmultimedia_en.qm",
             "./" + distDir + "/translations/qtmultimedia_es.qm",
             "./" + distDir + "/translations/qtmultimedia_fa.qm",
             "./" + distDir + "/translations/qtmultimedia_fi.qm",
             "./" + distDir + "/translations/qtmultimedia_fr.qm",
             "./" + distDir + "/translations/qtmultimedia_hr.qm",
             "./" + distDir + "/translations/qtmultimedia_hu.qm",
             "./" + distDir + "/translations/qtmultimedia_it.qm",
             "./" + distDir + "/translations/qtmultimedia_ja.qm",
             "./" + distDir + "/translations/qtmultimedia_ko.qm",
             "./" + distDir + "/translations/qtmultimedia_nl.qm",
             "./" + distDir + "/translations/qtmultimedia_nn.qm",
             "./" + distDir + "/translations/qtmultimedia_pl.qm",
             "./" + distDir + "/translations/qtmultimedia_pt_BR.qm",
             "./" + distDir + "/translations/qtmultimedia_ru.qm",
             "./" + distDir + "/translations/qtmultimedia_sk.qm",
             "./" + distDir + "/translations/qtmultimedia_tr.qm",
             "./" + distDir + "/translations/qtmultimedia_uk.qm",
             "./" + distDir + "/translations/qtmultimedia_zh_CN.qm",
             "./" + distDir + "/translations/qtmultimedia_zh_TW.qm"
        }
    );
    return Tree;
}

QSet<QString> ModulesQt6_4::qtWebEngine(const QString &distDir) const
{
    TestUtils utils;

    auto Tree = ModulesQt63::qtWebEngine(distDir);

    Tree += utils.createTree(
        {
         "./" + distDir + "/qml/QtWebChannel/libwebchannelplugin.so",
         "./" + distDir + "/qml/QtWebChannel/plugins.qmltypes",
         "./" + distDir + "/qml/QtWebChannel/qmldir",
         "./" + distDir + "/translations/qtserialport_zh_CN.qm"
        }
    );
    return Tree;
}

QSet<QString> ModulesQt6_4::qtWebEngineWidgets(const QString &distDir) const
{
    TestUtils utils;

    auto Tree = ModulesQt63::qtWebEngineWidgets(distDir);

    Tree += utils.createTree(
        {
            "./" + distDir + "/translations/qtserialport_zh_CN.qm"
        }
        );
    return Tree;
}
