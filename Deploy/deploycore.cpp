/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "extracter.h"
#include "deploycore.h"
#include "quasarapp.h"
#include "pathutils.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QLibraryInfo>
#include <configparser.h>

//QString DeployCore::qtDir = "";
//QStringList DeployCore::extraPaths = QStringList();


const DeployConfig* DeployCore::_config = nullptr;

QtModuleEntry DeployCore::qtModuleEntries[] = {
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

DeployCore::QtModule DeployCore::getQtModule(const QString& path) {
    auto priority = DeployCore::getLibPriority(path);

    if (priority != QtLib) {
        return DeployCore::QtModule::NONE;
    }

    int modulesCount = sizeof (qtModuleEntries) / sizeof (QtModuleEntry);

    auto lIbName = QFileInfo(path).fileName();

    for (int i = 0; i < modulesCount; ++i) {
        if (lIbName.contains(qtModuleEntries[i].libraryName)) {            
            return static_cast<DeployCore::QtModule>(qtModuleEntries[i].module);
        }
    }

    return DeployCore::QtModule::NONE;
}

void DeployCore::addQtModule(DeployCore::QtModule &module, const QString &path) {

    QuasarAppUtils::Params::verboseLog("current module " + QString::number(module),
                                       QuasarAppUtils::Info);

    auto mod = getQtModule(path);
    QuasarAppUtils::Params::verboseLog("add new module from path " + path  +
                                       " module value " + QString::number(mod),
                                       QuasarAppUtils::Info);


    module = static_cast<DeployCore::QtModule>(
                static_cast<quint64>(module) | static_cast<quint64>(mod));


}

LibPriority DeployCore::getLibPriority(const QString &lib) {

    if (!QFileInfo(lib).isFile()) {
        return NotFile;
    }

    if (isQtLib(lib)) {
        return QtLib;
    }

    if (isExtraLib(lib)) {
        return ExtraLib;
    }

    return SystemLib;
}

void DeployCore::verboseLog(const QString &str) {
    if (QuasarAppUtils::Params::isEndable("verbose")) {
        qDebug() << str;
    }
}

#define C(X) QuasarAppUtils::Params::isEndable(X)
RunMode DeployCore::getMode() {
    if (C("help") || C("h") || C("v") || C("version")) {
        return RunMode::Info;
    }

    if (C("init")) {
        return RunMode::Init;
    }

    if (QuasarAppUtils::Params::customParamasSize() == 0 || C("bin") || C("binDir")) {
        return RunMode::Deploy;
    }

    if (C("clear") || C("force-clear")) {
        return RunMode::Clear;
    }

    return RunMode::Info;
}

QString DeployCore::help() {

    QStringList help = {
    { "CQtDeployer version: " + getAppVersion()},
    { "Usage: cqtdeployer <-bin    [params]> [options]"},
    { "" },
    { "Options:" },
    { "   help / h                 : Shows help." },
    { "   init   [params]          : will initialize cqtdeployer.json file (configuration file)" },
    { "                            | For example: cqtdeployer init - for initialize single package configuration" },
    { "                            | cqtdeployer -init multiPackage - for initialize multi package configuration" },
    { "   noOverwrite              : Prevents replacing existing files." },
    { "   -bin    [list, params]   : Deployable file or folder." },
    { "                            | For example -bin /my/project/bin/,/my/project/bin.exe" },
    { "   -confFile [params]       | The path to the json file with all deployment configurations."},
    { "                            : Using this file, you can add the necessary options,"},
    { "                            : thereby simplifying the command invocation in the console."},
    { "                            : However, the parameters in Kansol have a higher priority than in the file."},
    { "   -binDir [params]         : A folder which includes deployable files (recursive search)." },
    { "                            | WARNING: this flag supports 'so', 'dll' and 'exe' files only." },
    { "                            | Use '-bin' flag if you want to deploy linux binary files" },
    { "   -qmlDir [package;path,path]: Qml data dir. For example -qmlDir ~/my/project/qml" },
    { "   deploySystem             : Deploys all libs" },
    { "   deploySystem-with-libc   : Skip Deploys system core libs libs" },
    { "   -qmake  [params]         : Qmake path." },
    { "                            | For example -qmake ~/Qt/5.14.0/gcc_64/bin/qmake" },
    { "   -ignore [list,params]    : The list of libs to ignore." },
    { "                            | For example -ignore libicudata.so.56,libicudata2.so.56" },
    { "   -ignoreEnv [list,params] : The list of the environment to ignore" },
    { "                            | For example -ignoreEnv /bad/dir,/my/bad/Dir" },
    { "   clear                    : Deletes deployable files of the previous session." },
    { "   force-clear              : Deletes the destination directory before deployment." },
    { "   allQmlDependes           : Extracts all the qml libraries." },
    { "                            | (not recommended, as it takes great amount of computer memory)" },
    { "   -libDir [list,params]    : Sets additional paths for extra libs of an app." },
    { "                            | For example -libDir /myLib,/newLibs " },
    { "   -extraLibs [list,params] | Sets the mask of the library name for forced copying."},
    { "                            : For example: '-extraLib mySql' - forces to copy all libraries"},
    { "                            : whose names contain mySql to the project folder. This option is case sensitive."},
    { "                            : This option is case sensitive."},
    { "   -extraPlugin[list,params]: Sets an additional path to extraPlugin of an app" },
    { "   -recursiveDepth [params] : Sets the Depth of recursive search of libs (default 0)" },
    { "   -targetDir [params]      : Sets target directory(by default it is the path to the first deployable file)" },
    { "   -targetPackage [tar1;path,path]: Sets package for target( by default it is empty value)" },
    { "   noStrip                  : Skips strip step" },
    { "   extractPlugins           | This flag will cause cqtdeployer to retrieve dependencies from plugins." },
    { "                            : Starting with version 1.4, this option has been disabled by default,"},
    { "                            : as it can add low-level graphics libraries to the distribution," },
    { "                            : which will not be compatible with equipment on users' hosts."},
    { "   noTranslations           : Skips the translations files." },
    { "                            | It doesn't work without qmake and inside a snap package" },
    { "   noCheckRPATH             : Disables automatic search of paths to qmake in executable files." },
    { "   noCheckPATH              : Disables automatic search of paths to qmake in system PATH." },
    { "   -qmlOut [package;val,val] : Sets path to qml out directory" },
    { "   -libOut [package;val,val] : Sets path to libraries out directory" },
    { "   -trOut [package;val,val]  : Sets path to translations out directory" },
    { " -pluginOut [package;val,val]: Sets path to plugins out directory" },
    { "   -binOut [package;val,val] : Sets path to binary out directory" },
    { "   -recOut [package;val,val] : Sets path to recurses out directory" },

    { "   -name [package;val,val]   : Sets name for package. If this if you do not specify a package, the value will be assigned to the default package ("")" },
    { "   -description [package;val,val] : Sets description for package" },
    { "   -deployVersion [package;val,val] : Sets version for package" },
    { "   -releaseDate [package;val,val] : Sets release date for package" },
    { "   -icon [package;val,val]   : Sets path to icon for package" },
    { "   -publisher [package;val,val]: Sets Publisher for package" },

    { "   -qif [params]            : Create the QIF installer for deployement programm" },
    { "                            : By default params value is 'Default'" },
    { "                            : Examples:" },
    { "                            : cqtdeployer qif - for use default templates of qt installer framework." },
    { "                            : cqtdeployer -qif path/to/folder/with/qifTemplate - for use custom templates of qt installer framework." },
    { "   -customScript [scriptCode]: Insert extra code inTo All run script."},
    { "   v / version              : Shows compiled version" },
    { "   verbose [1-3]            : Shows debug log" },

    { "" },
    { "" },
    { "Example: cqtdeployer -bin myApp -qmlDir ~/Qt/5.14.0/gcc_64/qml -qmake ~/Qt/5.14.0/gcc_64/bin/qmake clear" },
    { "Example (only C libs): cqtdeployer -bin myApp clear" }};

    QuasarAppUtils::Params::showHelp(help);

    return help.join(" ");
}

QStringList DeployCore::helpKeys() {
    return {
        "help",
        "noOverwrite",
        "bin",
        "binDir",
        "qmlDir",
        "deploySystem",
        "deploySystem-with-libc",
        "qmake",
        "ignore",
        "ignoreEnv",
        "clear",
        "force-clear",
        "allQmlDependes",
        "libDir",
        "extraLibs",
        "extraPlugin",
        "recursiveDepth",
        "targetDir",
        "targetPackage",
        "noStrip",
        "extractPlugins",
        "noTranslations",
        "qmlOut",
        "libOut",
        "trOut",
        "pluginOut",
        "binOut",
        "recOut",
        "version",
        "verbose",
        "qif",
        "noCheckRPATH",
        "noCheckPATH",
        "name",
        "description",
        "deployVersion",
        "releaseDate",
        "icon",
        "publisher",
        "customScript"
    };
}

QStringList DeployCore::extractTranslation(const QSet<QString> &libs) {
    QSet<QString> res;
    const size_t qtModulesCount = sizeof(qtModuleEntries) / sizeof(QtModuleEntry);

    for (const auto &lib: libs) {
        for (size_t i = 0; i < qtModulesCount; ++i) {
            if (lib.contains(qtModuleEntries[i].libraryName) &&
                    qtModuleEntries[i].translation) {
                res.insert(qtModuleEntries[i].translation);
            }
        }
    }
    return res.toList();
}

QString DeployCore::getAppVersion() {
    return APP_VERSION;
}

QString DeployCore::getQtVersion() {
#ifdef QT_VERSION_STR
    return QT_VERSION_STR;
#else
    return "without qt";
#endif
}

void DeployCore::printVersion() {
    qInfo() << "CQtDeployer: " + getAppVersion();
    qInfo() << "Qt: " +  getQtVersion();
}

bool DeployCore::isExecutable(const QFileInfo& file) {
    auto sufix = file.completeSuffix();
    return sufix.contains("exe", Qt::CaseInsensitive) || sufix.contains("run", Qt::CaseInsensitive) || sufix.isEmpty();
}

bool DeployCore::isContainsArraySeparators(const QString &val, int lastLvl) {
    while (lastLvl) {
        if (val.contains(getSeparator(--lastLvl)))
            return true;
    }

    return false;
}

QString DeployCore::findProcess(const QString &env, const QString& proc) {
    auto list = env.split(DeployCore::getEnvSeparator());

    for (const auto& path : list) {
        auto files = QDir(path).entryInfoList(QDir::NoDotAndDotDot | QDir::Files);

        for (const auto& bin : files) {
            if (bin.baseName().compare(proc, ONLY_WIN_CASE_INSENSIATIVE) == 0) {
                return bin.absoluteFilePath();
            }
        }
    }

    return "";
}

int DeployCore::find(const QString &str, const QStringList &list) {
    for (int i = 0 ; i < list.size(); ++i) {
        if (list[i].contains(str))
            return i;
    }
    return -1;
}

bool DeployCore::isLib(const QFileInfo &file) {
    return file.completeSuffix().contains("so", Qt::CaseInsensitive)
            || file.completeSuffix().contains("dll", Qt::CaseInsensitive);
}

MSVCVersion DeployCore::getMSVC(const QString &_qtBin) {
    int res = MSVCVersion::MSVC_Unknown;

    QDir dir = QFileInfo(_qtBin).absoluteFilePath();

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

QString DeployCore::getVCredist(const QString &_qtbinDir) {
    auto msvc = getMSVC(_qtbinDir);

    QDir dir = _qtbinDir;

    if (!(dir.cdUp() && dir.cdUp() && dir.cdUp() && dir.cd("vcredist"))) {
        QuasarAppUtils::Params::verboseLog("redist not findet!");
        return "";
    }

    auto infoList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

    auto name = getMSVCName(msvc);
    auto version = getMSVCVersion(msvc);

    for (const auto &info: infoList) {
        auto file = info.fileName();
        if (file.contains(name, Qt::CaseInsensitive) &&
                file.contains(version, Qt::CaseInsensitive)) {

            return info.absoluteFilePath();
        }
    }

    return "";
}

QString DeployCore::getMSVCName(MSVCVersion msvc) {
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

QString DeployCore::getMSVCVersion(MSVCVersion msvc) {
    if (msvc | MSVCVersion::MSVC_x32) {
        return "x86";
    } else if (msvc | MSVCVersion::MSVC_x64) {
        return "x64";
    }

    return "";
}

bool DeployCore::isQtLib(const QString &lib) {
    QFileInfo info((lib));

    if (_config) {
        return _config->qtDir.isQt(PathUtils::toFullPath(info.absoluteFilePath()));
    }

    return isLib(info) && info.fileName().contains("Qt", Qt::CaseInsensitive);
}

bool DeployCore::isExtraLib(const QString &lib) {
    QFileInfo info(lib);
    return _config->extraPaths.contains(info.absoluteFilePath());
}

QChar DeployCore::getSeparator(int lvl) {
    switch (lvl) {
    case 0:  return ',';
    case 1:  return ';';
    default:
        return '\0';
    }
}

char DeployCore::getEnvSeparator() {
#ifdef  Q_OS_UNIX
    return ':';
#else
    return ';';
#endif
}

uint qHash(WinAPI i) {
    return static_cast<uint>(i);
}
