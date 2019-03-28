/*
 * Copyright (C) 2018-2019 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "deploy.h"
#include "deployutils.h"
#include "quasarapp.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>

QString DeployUtils::qtDir = "";
QStringList DeployUtils::extraPaths = QStringList();

QtModuleEntry DeployUtils::qtModuleEntries[] = {
    { QtBluetoothModule, "bluetooth", "Qt5Bluetooth", nullptr },
    { QtConcurrentModule, "concurrent", "Qt5Concurrent", "qtbase" },
    { QtCoreModule, "core", "Qt5Core", "qtbase" },
    { QtDeclarativeModule, "declarative", "Qt5Declarative", "qtquick1" },
    { QtDesignerModule, "designer", "Qt5Designer", nullptr },
    { QtDesignerComponents, "designercomponents", "Qt5DesignerComponents", nullptr },
    { QtEnginioModule, "enginio", "Enginio", nullptr },
    { QtGamePadModule, "gamepad", "Qt5Gamepad", nullptr },
    { QtGuiModule, "gui", "Qt5Gui", "qtbase" },
    { QtHelpModule, "qthelp", "Qt5Help", "qt_help" },
    { QtMultimediaModule, "multimedia", "Qt5Multimedia", "qtmultimedia" },
    { QtMultimediaWidgetsModule, "multimediawidgets", "Qt5MultimediaWidgets", "qtmultimedia" },
    { QtMultimediaQuickModule, "multimediaquick", "Qt5MultimediaQuick_p", "qtmultimedia" },
    { QtNetworkModule, "network", "Qt5Network", "qtbase" },
    { QtNfcModule, "nfc", "Qt5Nfc", nullptr },
    { QtOpenGLModule, "opengl", "Qt5OpenGL", nullptr },
    { QtPositioningModule, "positioning", "Qt5Positioning", nullptr },
    { QtPrintSupportModule, "printsupport", "Qt5PrintSupport", nullptr },
    { QtQmlModule, "qml", "Qt5Qml", "qtdeclarative" },
    { QtQmlToolingModule, "qmltooling", "qmltooling", nullptr },
    { QtQuickModule, "quick", "Qt5Quick", "qtdeclarative" },
    { QtQuickParticlesModule, "quickparticles", "Qt5QuickParticles", nullptr },
    { QtQuickWidgetsModule, "quickwidgets", "Qt5QuickWidgets", nullptr },
    { QtScriptModule, "script", "Qt5Script", "qtscript" },
    { QtScriptToolsModule, "scripttools", "Qt5ScriptTools", "qtscript" },
    { QtSensorsModule, "sensors", "Qt5Sensors", nullptr },
    { QtSerialPortModule, "serialport", "Qt5SerialPort", "qtserialport" },
    { QtSqlModule, "sql", "Qt5Sql", "qtbase" },
    { QtSvgModule, "svg", "Qt5Svg", nullptr },
    { QtTestModule, "test", "Qt5Test", "qtbase" },
    { QtWebKitModule, "webkit", "Qt5WebKit", nullptr },
    { QtWebKitWidgetsModule, "webkitwidgets", "Qt5WebKitWidgets", nullptr },
    { QtWebSocketsModule, "websockets", "Qt5WebSockets", nullptr },
    { QtWidgetsModule, "widgets", "Qt5Widgets", "qtbase" },
    { QtWinExtrasModule, "winextras", "Qt5WinExtras", nullptr },
    { QtXmlModule, "xml", "Qt5Xml", "qtbase" },
    { QtXmlPatternsModule, "xmlpatterns", "Qt5XmlPatterns", "qtxmlpatterns" },
    { QtWebEngineCoreModule, "webenginecore", "Qt5WebEngineCore", nullptr },
    { QtWebEngineModule, "webengine", "Qt5WebEngine", "qtwebengine" },
    { QtWebEngineWidgetsModule, "webenginewidgets", "Qt5WebEngineWidgets", nullptr },
    { Qt3DCoreModule, "3dcore", "Qt53DCore", nullptr },
    { Qt3DRendererModule, "3drenderer", "Qt53DRender", nullptr },
    { Qt3DQuickModule, "3dquick", "Qt53DQuick", nullptr },
    { Qt3DQuickRendererModule, "3dquickrenderer", "Qt53DQuickRender", nullptr },
    { Qt3DInputModule, "3dinput", "Qt53DInput", nullptr },
    { Qt3DAnimationModule, "3danimation", "Qt53DAnimation", nullptr },
    { Qt3DExtrasModule, "3dextras", "Qt53DExtras", nullptr },
    { QtLocationModule, "geoservices", "Qt5Location", nullptr },
    { QtWebChannelModule, "webchannel", "Qt5WebChannel", nullptr },
    { QtTextToSpeechModule, "texttospeech", "Qt5TextToSpeech", nullptr },
    { QtSerialBusModule, "serialbus", "Qt5SerialBus", nullptr },
    { QtWebViewModule, "webview", "Qt5WebView", nullptr }
};

int DeployUtils::getLibPriority(const QString &lib) {

    if (!QFileInfo(lib).isFile()) {
        return 0;
    }

    if (isQtLib(lib)) {
        return 3;
    }

    if (isExtraLib(lib)) {
        return 2;
    }

    return 1;
}

void DeployUtils::verboseLog(const QString &str) {
    if (QuasarAppUtils::Params::isEndable("verbose")) {
        qDebug() << str;
    }
}

void DeployUtils::help() {
    qInfo() << "";
    qInfo() << "Usage: cqtdeployer <-bin    [params]> [options]";
    qInfo() << "";
    qInfo() << "Options:";
    qInfo() << "   help / h                 : show help.";
    qInfo() << "   always-overwrite         : Copy files even if the target file exists.";
    qInfo() << "   -bin    [list, params]   : deployment binry or directory.";
    qInfo() << "                            | example -bin ~/my/project/bin/,~/my/project/bin.exe";
    qInfo() << "   -binDir [params]         : folder with deployment binaries with recursive search";
    qInfo() << "                            | WARNING this flag support only 'so', 'dll' and 'exe' files";
    qInfo() << "                            | if you want deploy linux binary then use '-bin' flag";
    qInfo() << "   -qmlDir [params]         : qml datadir of project. for example -qmlDir ~/my/project/qml";
    qInfo() << "   deploy-not-qt            : deploy all libs";
    qInfo() << "   -qmake  [params]         : qmake path. for example";
    qInfo() << "                            | -qmake ~/Qt/5.11.1/gcc_64/bin/qmake";
    qInfo() << "   -ignore [list,params]    : ignore filter for libs";
    qInfo() << "                            | for example -ignore libicudata.so.56,libicudata2.so.56";
    qInfo() << "   clear                    : delete all old deploy data";
    qInfo() << "                            | for example -runScript myApp.sh";
    qInfo() << "   allQmlDependes           : This flag will force to extract all qml libraries.";
    qInfo() << "                            | (not recommended, as it takes up a lot of memory)";
    qInfo() << "   -libDir [list,params]    : set additional path for extralib of app.";
    qInfo() << "                            | for example -libDir ~/myLib,~/newLibs ";
    qInfo() << "   -extraPlugin[list,params]: set additional path for extraPlugin of app";
    qInfo() << "   -recursiveDepth [params] : set Depth for recursive search of libs (default 0)";
    qInfo() << "   -targetDir [params]      : set target Dir for binaryes (default is path of first target)";
    qInfo() << "   noStrip                  : skip strip step";
    qInfo() << "   noTranslations           : skip translations files";

    qInfo() << "   verbose                  : show debug log";

    qInfo() << "";
    qInfo() << "Example: cqtdeployer -bin myApp -qmlDir ~/Qt/5.11.1/gcc_64/qml -qmake ~/Qt/5.11.1/gcc_64/bin/qmake clear";
    qInfo() << "Example (only C libs): cqtdeployer -bin myApp clear";
}

bool DeployUtils::parseQt(Deploy *deploy) {

    if (!(QuasarAppUtils::Params::isEndable("bin") ||
            QuasarAppUtils::Params::isEndable("binDir"))) {
        qWarning() << "you need to use -bin or -binDir flags";
        return false;
    }

    auto bin = QuasarAppUtils::Params::getStrArg("bin").split(',');
    bin.removeAll("");

    if (!deploy->setTargets(bin)) {

        auto binDir = QuasarAppUtils::Params::getStrArg("binDir");

        if (!deploy->setTargetsRecursive(binDir)) {
            qCritical() << "error init targeet dir";
            return false;
        }
    }

    if (QuasarAppUtils::Params::isEndable("clear")) {
        qInfo() << "clear old data";
        deploy->clear();
    }

    deploy->initEnvirement();

    int limit = 0;

    if (QuasarAppUtils::Params::isEndable("recursiveDepth")) {
        bool ok;
        limit = QuasarAppUtils::Params::getArg("recursiveDepth").toInt(&ok);
        if (!ok) {
            limit = 0;
            qWarning() << "recursiveDepth is invalid! use default value 0";
        }
    }

    deploy->setDepchLimit(limit);

    auto listLibDir = QuasarAppUtils::Params::getStrArg("libDir").split(",");
    auto listExtraPlugin =
            QuasarAppUtils::Params::getStrArg("extraPlugin").split(",");
    deploy->setExtraPath(listLibDir);
    deploy->setExtraPlugins(listExtraPlugin);

    auto qmake = QuasarAppUtils::Params::getStrArg("qmake");
    QString basePath = "";

    QFileInfo info(qmake);

    if (!info.isFile() || (info.baseName() != "qmake")) {
        qInfo() << "deploy only C libs because qmake is not found";
        deploy->setOnlyCLibs(true);
        return true;
    }

    basePath = info.absolutePath();
    deploy->setQmake(qmake);
#ifdef Q_OS_WIN
    auto scaner = basePath + QDir::separator() + "qmlimportscanner.exe";
#else
    auto scaner = basePath + QDir::separator() + "qmlimportscanner";
#endif
    auto qmlDir = QuasarAppUtils::Params::getStrArg("qmlDir");

    QDir dir(basePath);

    if (QFileInfo::exists(qmlDir) && QFileInfo::exists(scaner)) {

        deploy->setDeployQml(true);
        deploy->setQmlScaner(scaner);

    } else if (QuasarAppUtils::Params::isEndable("allQmlDependes")) {
        deploy->setDeployQml(true);
    } else {
        qCritical() << "wrong qml dir!";
    }

    if (!dir.cdUp()) {
        return false;
    }

    deploy->setQtDir(dir.absolutePath());

    return true;
}

QStringList DeployUtils::extractTranslation(const QStringList &libs) {
    QSet<QString> res;
    const size_t qtModulesCount = sizeof(qtModuleEntries) / sizeof(QtModuleEntry);

    for (auto &&lib: libs) {
        for (size_t i = 0; i < qtModulesCount; ++i) {
            if (lib.contains(qtModuleEntries[i].libraryName) &&
                    qtModuleEntries[i].translation) {
                res.insert(qtModuleEntries[i].translation);
            }
        }
    }
    return res.toList();
}

MSVCVersion DeployUtils::getMSVC(const QString &_qmake) {
    QFileInfo qmake(_qmake);

    int res = MSVCVersion::MSVC_Unknown;

    if (!qmake.isFile()) {
        QuasarAppUtils::Params::verboseLog("qmake is wrong!");
        return static_cast<MSVCVersion>(res);
    }

    QDir dir = qmake.absoluteDir();

    if (!dir.cdUp()) {
        QuasarAppUtils::Params::verboseLog("is not standart qt repo");
        return static_cast<MSVCVersion>(res);
    }

    auto msvcPath = dir.absolutePath();

    if (!(dir.cdUp() && dir.cdUp())) {
        QuasarAppUtils::Params::verboseLog("is not standart qt repo");
        return static_cast<MSVCVersion>(res);
    }

    if (!msvcPath.contains("msvc")) {
        QuasarAppUtils::Params::verboseLog("vcredis not defined");
        return static_cast<MSVCVersion>(res);
    }

    auto base = msvcPath.mid(msvcPath.indexOf("msvc"), 11);
    auto version = base.mid(4 , 4);
    auto type = base.right(2);

    if (version == "2013") {
        res |= MSVC_13;
    }
    else if (version == "2015") {
        res |= MSVC_15;
    }
    else if (version == "2017") {
        res |= MSVC_17;
    }
    else if (version == "2019") {
        res |= MSVC_19;
    }

    if (type == "32") {
        res |= MSVC_x32;
    }
    else if (type == "64") {
        res |= MSVC_x64;
    }

    return static_cast<MSVCVersion>(res);
}

QString DeployUtils::getVCredist(const QString &_qmake) {
    auto msvc = getMSVC(_qmake);

    QFileInfo qmake(_qmake);

    QDir dir = qmake.absoluteDir();

    if (!(dir.cdUp() && dir.cdUp() && dir.cdUp() && dir.cd("vcredist"))) {
        QuasarAppUtils::Params::verboseLog("redist not findet!");
        return "";
    }

    auto infoList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

    auto name = getMSVCName(msvc);
    auto version = getMSVCVersion(msvc);

    for (auto &&info: infoList) {
        auto file = QFileInfo(info).fileName();
        if (file.contains(name) && file.contains(version)) {
            return info.absoluteFilePath();
        }
    }

    return "";
}

QString DeployUtils::getMSVCName(MSVCVersion msvc) {
    if (msvc | MSVCVersion::MSVC_13) {
        return "msvc2013";
    } else if (msvc | MSVCVersion::MSVC_15) {
        return "msvc2015";
    } else if (msvc | MSVCVersion::MSVC_17) {
        return "msvc2017";
    } else if (msvc | MSVCVersion::MSVC_19) {
        return "msvc2019";
    }

    return "";
}

QString DeployUtils::getMSVCVersion(MSVCVersion msvc) {
    if (msvc | MSVCVersion::MSVC_x32) {
        return "x86";
    } else if (msvc | MSVCVersion::MSVC_x64) {
        return "x64";
    }

    return "";
}

bool DeployUtils::isQtLib(const QString &lib) {
    QFileInfo info(lib);
    return !qtDir.isEmpty() && info.absoluteFilePath().contains(qtDir);

}

bool DeployUtils::isExtraLib(const QString &lib) {
    QFileInfo info(lib);

    for (auto i : extraPaths) {
        if (info.absoluteFilePath().contains(i)) {
            return true;
        }
    }

    return false;
}
