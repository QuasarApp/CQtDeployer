/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "extracter.h"
#include "deploycore.h"
#include "quasarapp.h"
#include "pathutils.h"
#include "pluginsparser.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QProcess>
#include <configparser.h>
#include <iostream>

//QString DeployCore::qtDir = "";
//QStringList DeployCore::extraPaths = QStringList();


const DeployConfig* DeployCore::_config = nullptr;

QtModuleEntry DeployCore::qtModuleEntries[] = {
    { QtBluetoothModule, "bluetooth", "QtXBluetooth", nullptr },
    { QtConcurrentModule, "concurrent", "QtXConcurrent", "qtbase" },
    { QtCoreModule, "core", "QtXCore", "qtbase" },
    { QtDeclarativeModule, "declarative", "QtXDeclarative", "qtquick1" },
    { QtDesignerModule, "designer", "QtXDesigner", nullptr },
    { QtDesignerComponents, "designercomponents", "QtXDesignerComponents", nullptr },
    { QtEnginioModule, "enginio", "Enginio", nullptr },
    { QtGamePadModule, "gamepad", "QtXGamepad", nullptr },
    { QtGuiModule, "gui", "QtXGui", "qtbase" },
    { QtHelpModule, "qthelp", "QtXHelp", "qt_help" },
    { QtMultimediaModule, "multimedia", "QtXMultimedia", "qtmultimedia" },
    { QtMultimediaWidgetsModule, "multimediawidgets", "QtXMultimediaWidgets", "qtmultimedia" },
    { QtMultimediaQuickModule, "multimediaquick", "QtXMultimediaQuick_p", "qtmultimedia" },
    { QtNetworkModule, "network", "QtXNetwork", "qtbase" },
    { QtNfcModule, "nfc", "QtXNfc", nullptr },
    { QtOpenGLModule, "opengl", "QtXOpenGL", nullptr },
    { QtOpenGLWidgetsModule, "openglwidgets", "QtXOpenGLWidgets", nullptr },
    { QtPositioningModule, "positioning", "QtXPositioning", nullptr },
    { QtPrintSupportModule, "printsupport", "QtXPrintSupport", nullptr },
    { QtQmlModule, "qml", "QtXQml", "qtdeclarative" },
    { QtQmlToolingModule, "qmltooling", "qmltooling", nullptr },
    { QtQuickModule, "quick", "QtXQuick", "qtdeclarative" },
    { QtQuickParticlesModule, "quickparticles", "QtXQuickParticles", nullptr },
    { QtQuickWidgetsModule, "quickwidgets", "QtXQuickWidgets", nullptr },
    { QtScriptModule, "script", "QtXScript", "qtscript" },
    { QtScriptToolsModule, "scripttools", "QtXScriptTools", "qtscript" },
    { QtSensorsModule, "sensors", "QtXSensors", nullptr },
    { QtSerialPortModule, "serialport", "QtXSerialPort", "qtserialport" },
    { QtSqlModule, "sql", "QtXSql", "qtbase" },
    { QtSvgWidgetsModule, "svgwidgets", "QtXSvgWidgets", nullptr },
    { QtSvgModule, "svg", "QtXSvg", nullptr },
    { QtTestModule, "test", "QtXTest", "qtbase" },
    { QtWebKitModule, "webkit", "QtXWebKit", nullptr },
    { QtWebKitWidgetsModule, "webkitwidgets", "QtXWebKitWidgets", nullptr },
    { QtWebSocketsModule, "websockets", "QtXWebSockets", nullptr },
    { QtWidgetsModule, "widgets", "QtXWidgets", "qtbase" },
    { QtWinExtrasModule, "winextras", "QtXWinExtras", nullptr },
    { QtXmlModule, "xml", "QtXXml", "qtbase" },
    { QtXmlPatternsModule, "xmlpatterns", "QtXXmlPatterns", "qtxmlpatterns" },
    { QtWebEngineCoreModule, "webenginecore", "QtXWebEngineCore", nullptr },
    { QtWebEngineModule, "webengine", "QtXWebEngine", "qtwebengine" },
    { QtWebEngineWidgetsModule, "webenginewidgets", "QtXWebEngineWidgets", nullptr },
    { Qt3DCoreModule, "3dcore", "QtX3DCore", nullptr },
    { Qt3DRendererModule, "3drenderer", "QtX3DRender", nullptr },
    { Qt3DQuickModule, "3dquick", "QtX3DQuick", nullptr },
    { Qt3DQuickRendererModule, "3dquickrenderer", "QtX3DQuickRender", nullptr },
    { Qt3DInputModule, "3dinput", "QtX3DInput", nullptr },
    { Qt3DAnimationModule, "3danimation", "QtX3DAnimation", nullptr },
    { Qt3DExtrasModule, "3dextras", "QtX3DExtras", nullptr },
    { QtLocationModule, "geoservices", "QtXLocation", nullptr },
    { QtWebChannelModule, "webchannel", "QtXWebChannel", nullptr },
    { QtTextToSpeechModule, "texttospeech", "QtXTextToSpeech", nullptr },
    { QtSerialBusModule, "serialbus", "QtXSerialBus", nullptr },
    { QtWebViewModule, "webview", "QtXWebView", nullptr }
};

DeployCore::QtModule DeployCore::getQtModule(const QString& path) {
    auto Qt = DeployCore::isQtLib(path);

    if (!Qt) {
        return DeployCore::QtModule::NONE;
    }

    int modulesCount = sizeof (qtModuleEntries) / sizeof (QtModuleEntry);

    auto lIbName = QFileInfo(path).fileName();

    for (int i = 0; i < modulesCount; ++i) {
        if (containsModule(qtModuleEntries[i].libraryName, lIbName)) {
            return static_cast<DeployCore::QtModule>(qtModuleEntries[i].module);
        }
    }

    return DeployCore::QtModule::NONE;
}

void DeployCore::addQtModule(DeployCore::QtModule &module, const QString &path) {

    QuasarAppUtils::Params::log("current module " + QString::number(module),
                                       QuasarAppUtils::Debug);

    auto mod = getQtModule(path);
    QuasarAppUtils::Params::log("add new module from path " + path  +
                                       " module value " + QString::number(mod),
                                       QuasarAppUtils::Debug);


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

    if (isAlienLib(lib)) {
        return AlienLib;
    }

    if (isAllowedLib(lib)) {
        return AllowedLib;
    }

    return SystemLib;
}

bool DeployCore::containsModule(const QString& moduleLibrary, const QString& lib) {
    QRegExp erfexp(QString(moduleLibrary).replace("QtX", "Qt[4,5,6]"));
    return lib.contains(erfexp);
}

#define C(X) QuasarAppUtils::Params::isEndable(X)
RunMode DeployCore::getMode() {
    if (C("help") || C("h") || C("v") || C("version")) {
        return RunMode::Info;
    }

    if (C("init")) {
        return RunMode::Init;
    }

    if (C("getDefaultTemplate")) {
        return RunMode::Template;
    }

    if (C("bin") || C("extraData")) {
        return RunMode::Deploy;
    }

    if (C("clear") || C("force-clear")) {
        return RunMode::Clear;
    }

    return RunMode::Info;
}

void DeployCore::help() {


    QuasarAppUtils::Help::Charters help = {
        {
            "Part 0 General", {
                {"CQtDeployer version", getAppVersion()},
                {"Usage", "cqtdeployer <-bin [params]> [options]"},
            }
        },
        {
            "Part 1 Boolean options", {
                {"init", "will initialize cqtdeployer.json file (configuration file)."
                 " For example: 'cqtdeployer init' - for initialize base package configuration."
                 " 'cqtdeployer -init multi' - for initialize multi package configuration"
                 " 'cqtdeployer -init single' - for initialize singel package configuration"},
                {"help / h", "Shows help"},
                {"clear", "Deletes deployable files of the previous session."},
                {"force-clear", "Deletes the destination directory before deployment."},
                {"noStrip", "Skips strip step"},
                {"noTranslations", "Skips the translations files. It doesn't work without qmake."},
                {"noOverwrite", "Prevents replacing existing files."},
                {"noCheckRPATH", "Disables automatic search of paths to qmake in executable files."},
                {"noCheckPATH", "Disables automatic search of paths to qmake in system PATH."},
                {"noRecursiveiIgnoreEnv", "Disables recursive ignore for ignoreEnv option."},
                {"v / version", "Shows compiled version"},
                {"qif", "Create the QIF installer for deployment programm"
                        " You can specify the path to your own installer template. Examples: cqtdeployer -qif path/to/myCustom/qif."},
                {"qifFromSystem", "force use system binarycreator tool of qif from path or qt"},
                {"zip", "Create the ZIP arhive for deployment programm"},
                {"deb", "Create the deb package for deployment programm"
                        " You can specify the path to your own debian template. Examples: cqtdeployer -deb path/to/myCustom/DEBIAN."},
                {"deploySystem", "Deploys all libraries."
                 " Not recomendet because there may be conflicts with system libraries"
                 " (on snap version you need to turn on permission)"},
                {"deploySystem-with-libc", "deploys all libs include libc (only linux)."
                 " Do not use this option with gui application."
                 " For gui application sue the deploySystem option "
                 "(on snap version you need to turn on permission)"},
                {"noQt", "Ignors the error of initialize of a qmake. Use only if your application does not use the qt framework."},
                {"allowEmptyPackages", "Allows configure the empty packages."},
                {"getDefaultTemplate", "Extracts defaults deb or qif templates."
                 " All templates extract into targetDirectory."
                 " For change target directory use the targetDir option."
                 " Example: cqtdeployer -bin myExecutable getDefaultTemplate qif deb."},
                {"noBase", "The **NoBase** option force cqtdeployer disable generate a base package. For more information about base packages see official documentations"},


            }
        },
        {
            "Part 2 Deploy options", {
                {"-bin [list, params]", "Deployable file or folder. For example -bin ~/my/project/bin/,~/my/project/bin.exe"},
                {"-confFile [params]", "The path to the json file with all deployment configurations. Using this file,"
                 " you can add the necessary options, thereby simplifying the command invocation in the console."
                 " However, the parameters in Kansol have a higher priority than in the file."
                 " For more info about this flag see https://github.com/QuasarApp/CQtDeployer/wiki/DeployConfigFileEn"},
                {"-qmlDir [params]", "Qml data dir. For example -qmlDir ~/my/project/qml"},
                {"-qmake [params]", "Deployable file or folder. For example -bin ~/my/project/bin/,~/my/project/bin.exe"},
                {"-ignore [list,params]", "The list of libs to ignore. For example -ignore libicudata.so.56,libicudata2.so.56"},
                {"-ignoreEnv [list,params]", "The list of the environment to ignore. For example -ignoreEnv /bad/dir,/my/bad/Dir"},
                {"-libDir [list,params]", "Sets additional paths for extra libs of an app. For example -libDir ~/myLib,~/newLibs"},
                {"-extraLibs [list,params]", "Sets the mask of the library name for forced copying."
                 " Example: '-extraLibs mySql' - forces to copy all libraries whose names contain mySql to the project folder."
                 " This option is case sensitive."},
                {"-customScript [scriptCode]", "Insert extra code inTo All run script."},
                {"-recursiveDepth [params]", "Sets the Depth of recursive search of libs and depth for ignoreEnv option (default 0)"},
                {"-targetDir [params]", "Sets target directory(by default it is the path to the first deployable file)"},
                {"-runScript [list,parems]", "forces cqtdeployer swap default run script to new from the arguments of option."
                 " This option copy all content from input file and insert all code into runScript.sh or .bat"
                 " Example of use: cqtdeployer -runScript \"myTargetMame;path/to/my/myCustomLaunchScript.sh,myTargetSecondMame;path/to/my/mySecondCustomLaunchScript.sh\""},

                {"-verbose [0-3]", "Shows debug log"},

            }
        },
        {
            "Part 3 Control of packages options", {
                {"-targetPackage [package;tar1,package;tar2]", "Creates a new package and adds 'tar1 and tar2' to it."
                 "If you want configure the package that do not have any targets use the allowEmptyPackages option."},
                {"-qmlOut [package;path,path]", "Sets path to qml out directory"},
                {"-libOut [package;path,path]", "Sets path to libraries out directory"},
                {"-trOut [package;path,path]", "Sets path to translations out directory"},
                {"-pluginOut [package;path,path]", "Sets path to plugins out directory"},
                {"-binOut [package;path,path]", "Sets path to binary out directory"},
                {"-recOut [package;path,path]", "Sets path to recurses out directory"},
                {"-extraDataOut [package;path,path]", "Sets path to extra data files out directory. By Default it is root dir of the distribution."},
                {"-name [package;val,val]", "Sets name for a package. "
                 "If this if you do not specify a package, the value will be assigned to the default package ("")"},
                {"-description [package;val,val]", "Sets description for a package"},
                {"-deployVersion [package;val,val]", "Sets version for a package"},
                {"-releaseDate [package;val,val]", "Sets release date for a package"},
                {"-icon [package;val,val]", "Sets path to icon for a package"},
                {"-publisher [package;val,val]", "Sets publisher for a package"},
                {"-homePage [package;val,val]", "Sets the home page url for a package"},
                {"-prefix [package;val,val]", "Sets the prefix for the package relatively a target directory "},
                {"-extraData [package;val,val]", "Adds the extra files or directories like a target. The selected directory will be copy to the extraDataOut location with save own structure."},
                {"-basePackage [basePkg;pkg1;pkg2,...]", "Forces the cqtdeployer create a base package for the tar1 and tar2. For more information about base packages see official documentation."},

            }
        },

        {
            "Part 4 Plugins Control Options", {
                {"-extraPlugin [package;val1;val2,SingeleVal]", "Sets an additional path to third-party application plug-in"},
                {"-enablePlugins [package;val1;val2,SingeleVal", "Enables additional plugins for distribution."
                 " By default disabled next plugins: " + PluginsParser::defaultForbidenPlugins().join(',') + " if you want enable"
                 " it then use '-enablePlugins " + PluginsParser::defaultForbidenPlugins().join(',') + "' option"},
                {"-disablePlugins [package;val1;val2,SingeleVal]", "Disables plugins for distribution. "
                 "You can disable any plugin of your Qt build, just see the yourQtFolder/plugins forlder for available plugins."
                 " Example if you want disable qxcb plugin: -disablePlugins qxcb."
                 " Note that the name of the plugin is indicated without its extension"},

            }
        },
        {
            "Part 5 QtInstallFramework options", {
                {"-qifStyle [path/to/style.css]", "Sets the path to the CSS style file or sets the default style."
                 " Available styles: quasar, quasarDark"},
                {"-qifBanner [path/to/banner.png]", "Sets path to the banner png file."},
                {"-qifLogo [path/to/logo.png]", "Sets path to the logo png file."},
            }
        },
        {
            "Support", {
                {"Support for you", "If you have any questions or problems with cqtdeployer you can write to us about the problem on the GitHub page: https://github.com/QuasarApp/CQtDeployer/issues"},
                {"Support for us", "If you liked cqtdeployer, then you can join any patreon plan of your choice for support us: https://www.patreon.com/QuasarApp"},
            }
        }
    };

    help.unite(QuasarAppUtils::Params::getparamsHelp());

    QuasarAppUtils::Params::showHelp(help);

    return;
}

QStringList DeployCore::helpKeys() {
    return {
        "help",
        "noOverwrite",
        "bin",
        "extraData",
        "qmlDir",
        "deploySystem",
        "deploySystem-with-libc",
        "qmake",
        "ignore",
        "ignoreEnv",
        "clear",
        "force-clear",
        "libDir",
        "extraLibs",
        "extraPlugin",
        "recursiveDepth",
        "targetDir",
        "targetPackage",
        "noStrip",
        "extractPlugins",
        "noTranslations",
        "noRecursiveiIgnoreEnv",
        "qifFromSystem",
        "qmlOut",
        "libOut",
        "trOut",
        "pluginOut",
        "binOut",
        "recOut",
        "extraDataOut",
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
        "customScript",
        "qifStyle",
        "qifBanner",
        "qifLogo",
        "zip",
        "noQt",
        "homePage",
        "prefix",
        "deb",
        "allowEmptyPackages",
        "runScript",
        "getDefaultTemplate",
        "noBase",
        "basePackage"
    };
}

QStringList DeployCore::extractTranslation(const QSet<QString> &libs) {
    QSet<QString> res;
    const size_t qtModulesCount = sizeof(qtModuleEntries) / sizeof(QtModuleEntry);

    for (const auto &lib: libs) {
        for (size_t i = 0; i < qtModulesCount; ++i) {
            if (containsModule(qtModuleEntries[i].libraryName, lib) && qtModuleEntries[i].translation) {
                res.insert(qtModuleEntries[i].translation);
            }
        }
    }
    return res.values();
}

QString DeployCore::getAppVersion() {
    return APP_VERSION;
}

QString DeployCore::getAppVersionName() {
    return "*** Binary Box ***";
}

QString DeployCore::getQtVersion() {
#ifdef QT_VERSION_STR
    return QT_VERSION_STR;
#else
    return "without qt";
#endif
}

void DeployCore::printVersion() {
    std::cout << QString{"CQtDeployer: " + getAppVersion() + " " +  getAppVersionName()}.toStdString() << std::endl;
    std::cout << QString{"Qt: " +  getQtVersion()}.toStdString() << std::endl;
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

QStringList DeployCore::debugExtensions() {
    return {".debug", "d.dll", ".pdb"};
}

bool DeployCore::isDebugFile(const QString &file) {
    auto debug = debugExtensions();
    for (const auto& debugEx: debug) {
        if (file.contains(debugEx, ONLY_WIN_CASE_INSENSIATIVE)) {
            return true;
        }
    }

    return false;
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
        QuasarAppUtils::Params::log("is not standart qt repo");
        return static_cast<MSVCVersion>(res);
    }

    auto msvcPath = dir.absolutePath();

    if (!(dir.cdUp() && dir.cdUp())) {
        QuasarAppUtils::Params::log("is not standart qt repo");
        return static_cast<MSVCVersion>(res);
    }

    if (!msvcPath.contains("msvc")) {
        QuasarAppUtils::Params::log("vcredis not defined");
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
        QuasarAppUtils::Params::log("redist not findet!");
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

QtMajorVersion DeployCore::isQtLib(const QString &lib) {
    QFileInfo info(lib);
/*
 * Task https://github.com/QuasarApp/CQtDeployer/issues/422
 * All qt libs need to contains the Qt label.
*/
    QtMajorVersion isQt = QtMajorVersion::NoQt;

    if (!isLib(info)) {
        return isQt;
    }

    QString fileName = info.fileName();
    if (fileName.contains("Qt4", ONLY_WIN_CASE_INSENSIATIVE)) {
        isQt = QtMajorVersion::Qt4;
    } else if (fileName.contains("Qt5", ONLY_WIN_CASE_INSENSIATIVE)) {
        isQt = QtMajorVersion::Qt5;
    } else if (fileName.contains("Qt6", ONLY_WIN_CASE_INSENSIATIVE)) {
        isQt = QtMajorVersion::Qt6;
    }

    if (_config && !_config->qtDir.isQt(info.absoluteFilePath())) {
        return QtMajorVersion::NoQt;
    }

    if (isQt && QuasarAppUtils::Params::isEndable("noQt") &&
            !QuasarAppUtils::Params::isEndable("qmake")) {
        return QtMajorVersion::NoQt;
    }

    return isQt;
}

bool DeployCore::isExtraLib(const QString &lib) {
    QFileInfo info(lib);
    return _config->extraPaths.contains(info.absoluteFilePath());
}

bool DeployCore::isAlienLib(const QString &lib) {
    return lib.contains("/opt/", ONLY_WIN_CASE_INSENSIATIVE) ||
            lib.contains("/PROGRAM FILES", ONLY_WIN_CASE_INSENSIATIVE);
}

bool DeployCore::isAllowedLib(const QString &lib) {
    QFileInfo info(lib);
    return _config->allowedPaths.contains(info.absoluteFilePath());
}

QStringList DeployCore::Qt3rdpartyLibs(Platform platform) {

    QStringList result;

    result << QStringList {
              // Begin SQL LIBS
              // See task https://github.com/QuasarApp/CQtDeployer/issues/367

              "libpq",
              "mysqlclient"

              // End SQL LIBS
};

    if (platform & Platform::Win) {
        result << QStringList {
                  // Qml Gl driver wraper
                  "d3dcompiler_47",
                  "libEGL",
                  "libGLESv2",

                  // gcc runtime libs ow mingw
                  "libgcc_s_seh",
                  "libstdc++",
                  "libwinpthread",

                  // OpenglES libs
                  "opengl32sw",
    };
    }

    if (platform & Platform::Unix) {
        result << QStringList {
                  // Begin  Unicode libs

                  "libicudata",
                  "libicui18n",
                  "libicuio",
                  "libicule",
                  "libiculx",
                  "libicutest",
                  "libicutu",
                  "libicuuc",

                  // End Unicode libs

                  // xcb plugin
                  "libxcb-xinerama",

                  // qt GUI
                  "libpng",
                  "libjpeg"
    };
    }

    return result;
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

bool DeployCore::isSnap() {
    return QProcessEnvironment::systemEnvironment().value("SNAP").size();
}

QString DeployCore::snapRootFS() {
    return "/var/lib/snapd/hostfs";
}

QString DeployCore::transportPathToSnapRoot(const QString &path) {
    if (isSnap() && checkSystemBakupSnapInterface()) {

        if(QFileInfo(path).isWritable()) {
            return path;
        }

        if (path.size() && path[0] != "/") {
            auto absalutPath = QProcessEnvironment::systemEnvironment().value("PWD") + "/" + path;
            if (!absalutPath.contains(DeployCore::snapRootFS())) {
                return snapRootFS() + "/" + absalutPath;
            }
        }

        if (!path.contains(DeployCore::snapRootFS())) {
            return snapRootFS() + "/" + path;
        }
    }

    return path;
}

bool DeployCore::checkSystemBakupSnapInterface() {
    return QDir(DeployCore::snapRootFS()).entryList(QDir::AllEntries | QDir::NoDotAndDotDot).size();
}

QString DeployCore::systemLibsFolderName() {
    return "systemLibs";
}

QString DeployCore::getBasePackageName(const QStringList &list) {
    if (!list.size())
        return "";

    QString postfix = "Base";

    if (list.size() == 1)
        return list.first() + postfix;

    const auto &first = list[0];
    QString result;
    for (int charInxex = 0; charInxex < first.size(); ++charInxex) {
        for (int wordIndex = 1; wordIndex < list.size(); ++wordIndex) {
            if (list[wordIndex].size() <= charInxex || list[wordIndex][charInxex] != first[charInxex]) {
                return result + postfix;
            }
        }
        result += first[charInxex];
    }

    return result + postfix;
}

uint qHash(WinAPI i) {
    return static_cast<uint>(i);
}
