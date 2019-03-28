/*
 * Copyright (C) 2018-2019 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef DEPLOYUTILS_H
#define DEPLOYUTILS_H

#include <QStringList>
#include <QDebug>
#include "deploy_global.h"

enum MSVCVersion: int {
    MSVC_Unknown = 0x0,
    MSVC_x64 = 0x01,
    MSVC_x32 = 0x02,
    MSVC_13 = 0x10,
    MSVC_15 = 0x20,
    MSVC_17 = 0x40,
    MSVC_19 = 0x80,
};

struct DEPLOYSHARED_EXPORT QtModuleEntry {
    quint64 module;
    const char *option;
    const char *libraryName;
    const char *translation;
};

class Deploy;

class DEPLOYSHARED_EXPORT DeployUtils
{

private:
    static QString getMSVCName(MSVCVersion msvc);
    static QString getMSVCVersion(MSVCVersion msvc);

public:
    enum QtModule : quint64
    {
        QtBluetoothModule         = 0x0000000000000001,
        QtConcurrentModule        = 0x0000000000000002,
        QtCoreModule              = 0x0000000000000004,
        QtDeclarativeModule       = 0x0000000000000008,
        QtDesignerComponents      = 0x0000000000000010,
        QtDesignerModule          = 0x0000000000000020,
        QtGuiModule               = 0x0000000000000040,
        QtHelpModule              = 0x0000000000000080,
        QtMultimediaModule        = 0x0000000000000100,
        QtMultimediaWidgetsModule = 0x0000000000000200,
        QtMultimediaQuickModule   = 0x0000000000000400,
        QtNetworkModule           = 0x0000000000000800,
        QtNfcModule               = 0x0000000000001000,
        QtOpenGLModule            = 0x0000000000002000,
        QtPositioningModule       = 0x0000000000004000,
        QtPrintSupportModule      = 0x0000000000008000,
        QtQmlModule               = 0x0000000000010000,
        QtQuickModule             = 0x0000000000020000,
        QtQuickParticlesModule    = 0x0000000000040000,
        QtScriptModule            = 0x0000000000080000,
        QtScriptToolsModule       = 0x0000000000100000,
        QtSensorsModule           = 0x0000000000200000,
        QtSerialPortModule        = 0x0000000000400000,
        QtSqlModule               = 0x0000000000800000,
        QtSvgModule               = 0x0000000001000000,
        QtTestModule              = 0x0000000002000000,
        QtWidgetsModule           = 0x0000000004000000,
        QtWinExtrasModule         = 0x0000000008000000,
        QtXmlModule               = 0x0000000010000000,
        QtXmlPatternsModule       = 0x0000000020000000,
        QtWebKitModule            = 0x0000000040000000,
        QtWebKitWidgetsModule     = 0x0000000080000000,
        QtQuickWidgetsModule      = 0x0000000100000000,
        QtWebSocketsModule        = 0x0000000200000000,
        QtEnginioModule           = 0x0000000400000000,
        QtWebEngineCoreModule     = 0x0000000800000000,
        QtWebEngineModule         = 0x0000001000000000,
        QtWebEngineWidgetsModule  = 0x0000002000000000,
        QtQmlToolingModule        = 0x0000004000000000,
        Qt3DCoreModule            = 0x0000008000000000,
        Qt3DRendererModule        = 0x0000010000000000,
        Qt3DQuickModule           = 0x0000020000000000,
        Qt3DQuickRendererModule   = 0x0000040000000000,
        Qt3DInputModule           = 0x0000080000000000,
        QtLocationModule          = 0x0000100000000000,
        QtWebChannelModule        = 0x0000200000000000,
        QtTextToSpeechModule      = 0x0000400000000000,
        QtSerialBusModule         = 0x0000800000000000,
        QtGamePadModule           = 0x0001000000000000,
        Qt3DAnimationModule       = 0x0002000000000000,
        QtWebViewModule           = 0x0004000000000000,
        Qt3DExtrasModule          = 0x0008000000000000
    };

    DeployUtils() = delete;

    static QString qtDir;
    static QStringList extraPaths;
    static QtModuleEntry qtModuleEntries[];

    static MSVCVersion getMSVC(const QString & _qmake);
    static QString getVCredist(const QString & _qmake);

    static bool isQtLib(const QString &lib);
    static bool isExtraLib(const QString &lib);
    static int getLibPriority(const QString &lib);
    static void verboseLog(const QString &str);
    static void help();
    static bool parseQt(Deploy *deploy);
    static QStringList extractTranslation(const QStringList& libs);

};

#endif // DEPLOYUTILS_H
