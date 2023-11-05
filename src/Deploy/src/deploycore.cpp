/*
 * Copyright (C) 2018-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "deploycore.h"
#include "defines.h"
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
    { QtWebViewModule, "webview", "QtXWebView", nullptr },
    { QtVirtualKeyboard, "virtualkeyboard", "QtXVirtualKeyboard", nullptr },
    { QtShaderToolsModule, "shadertools", "QtXShaderTools", nullptr }
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

    QuasarAppUtils::Params::log("Current module " + QString::number(module),
                                QuasarAppUtils::Debug);

    auto mod = getQtModule(path);
    QuasarAppUtils::Params::log("Add new module from path " + path  +
                                " module value " + QString::number(mod),
                                QuasarAppUtils::Debug);


    module = static_cast<DeployCore::QtModule>(
                static_cast<quint64>(module) | static_cast<quint64>(mod));


}

bool DeployCore::isGui(DeployCore::QtModule module) {
    DeployCore::QtModule guiModules =
            static_cast<DeployCore::QtModule>(DeployCore::QtGuiModule |
                                              DeployCore::QtOpenGLModule |
                                              DeployCore::QtQmlModule |
                                              DeployCore::QtQuickModule);

    return guiModules & module;
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
    QRegularExpression erfexp(QString(moduleLibrary).replace("QtX", "Qt[4,5,6]"));
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

QuasarAppUtils::OptionsDataList DeployCore::avilableOptions() {
    QString group = "Part 1 Boolean options";
    QuasarAppUtils::OptionsDataList help = {};
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"init"}, "",
                            "will initialize cqtdeployer.json file (configuration file).",
                            "'cqtdeployer init' - for initialize base package configuration. "
                            "'cqtdeployer -init multi' - for initialize multi package configuration "
                            "'cqtdeployer -init single' - for initialize singel package configuration"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"clear"}, "",
                            "Deletes deployable files of the previous session."
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"force-clear"}, "",
                            "Deletes the destination directory before deployment."
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"noStrip"}, "",
                            "Skips strip step"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"noTranslations"}, "",
                            "Skips the translations files. It doesn't work without qmake."
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"noOverwrite"}, "",
                            "Prevents replacing existing files."
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"noCheckRPATH"}, "",
                            "Disables automatic search of paths to qmake in executable files."
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"noCheckPATH"}, "",
                            "Disables automatic search of paths to qmake in system PATH."
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"noRecursiveiIgnoreEnv"}, "",
                            "Disables recursive ignore for ignoreEnv option."
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"v", "version"}, "",
                            "Shows compiled version"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"h", "help"}, "",
                            "Show all help or help of the selected options."
                            " You can add any another option to a command line for show help about using options"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"qif","-qif"}, "",
                            "Create the QIF installer for deployment programm"
                            " You can specify the path to your own installer template.",
                            "Examples: cqtdeployer -qif path/to/myCustom/qif."
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"qifFromSystem"}, "",
                            "force use system binarycreator tool of qif from path or qt"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"zip"}, "",
                            "Create the ZIP arhive for deployment programm"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"deb", "-deb"}, "",
                            "Create the deb package for deployment programm"
                            " You can specify the path to your own debian template.",
                            "cqtdeployer -deb path/to/myCustom/DEBIAN."
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"deploySystem"}, "",
                            "Deploys all libraries."
                            " Not recomendet because there may be conflicts with system libraries"
                            " (on snap version you need to turn on permission)"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"noQt"}, "",
                            "Ignors the error of initialize of a qmake. Use only if your application does not use the qt framework."
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"allowEmptyPackages"}, "",
                            "Allows configure the empty packages."
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"getDefaultTemplate"}, "",
                            "Extracts defaults deb or qif templates."
                            " All templates extract into targetDirectory."
                            " For change target directory use the targetDir option.",
                            "cqtdeployer -bin myExecutable getDefaultTemplate qif deb."
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"noHashSum"}, "",
                            "This option disable computation of a packages hash sum"
                        }});

    group = "Part 2 Deploy options";

    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-bin"}, "{list,params}",
                            "Files to deploy or folders that contain files to deploy.",
                            "-bin ~/my/project/bin/,~/my/project/bin.exe,~/my/project/runtimeLinking/lib.dll."
                            " For files: These files will be unconditional copied to the destination directory,"
                            " regardless of their format or suffix."
                            " For folders:"
                            " CCQtDeployer will enter these folders and non-recursively copy all executable files to the destination directory."
                            " Then, CQtDeployer will extract all dependencies of the copied files and search dependencies in system environments and libDir paths."
                            "**Note**: If CQtDeployer can't find required file then"
                            " CQtDeployer try find required file in the system PATH enviroment."
                        }});

    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-binPrefix"}, "{prefixPath}",
                            "Sets prefix path for bin option.",
                            "-bin path/MyExecutable is some as -bin MyExecutable -binPrefix path"
                        }});

    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-confFile"}, "{params}",
                            "The path to the json file with all deployment configurations. Using this file,"
                            " you can add the necessary options, thereby simplifying the command invocation in the console."
                            " However, the parameters in Kansol have a higher priority than in the file."
                            " For more info about this flag see https://github.com/QuasarApp/CQtDeployer/wiki/DeployConfigFileEn"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-qmlDir"}, "{params}",
                            "Sets path to Qml data dir",
                            "-qmlDir ~/my/project/qml"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-qmake"}, "{params}",
                            "Sets path to the qmake executable.",
                            "-qmake ~/Qt/bin/qmake or -qmake ~/Qt/bin/qmake.exe"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-ignore"}, "{list,params}",
                            "Sets the list of libs to ignore.",
                            "-ignore libicudata.so.56,libicudata2.so.56"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-ignoreEnv"}, "{list,params}",
                            "Sets the list of the environment to ignore.",
                            "-ignoreEnv /bad/dir,/my/bad/Dir"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-libDir"}, "{list,params}",
                            "Sets additional paths for extra libs of an app.",
                            "-libDir ~/myLib,~/newLibs"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-extraLibs"}, "{list,params}",
                            "Sets the mask of the library name for forced copying.",
                            "\"-extraLib mySql\" - forces to copy all libraries whose names contain mySql to the project folder."
                            " This option is case-insensitive on Windows and case-sensitive on other platforms."
                            " This option will only search libraries in system environments similar to **deploySystem**."
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-customScript"}, "{scriptCode}",
                            "Insert extra code inTo All run script.",
                            "",
                            " This option will be removed into next release cqtdeployer."
                            " Please use the runScript option"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-recursiveDepth"}, "{params}",
                            "Sets the Depth of recursive search of libs and depth for ignoreEnv option (default 0)"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-installDirQIFW"}, "{params}",
                            "Sets install target directory for installers (by default it is /home path)"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-runScript"}, "{list,parems}",
                            "forces cqtdeployer swap default run script to new from the arguments of option."
                            " This option copy all content from input file and insert all code into runScript.sh or .bat",
                            "cqtdeployer -runScript \"myTargetMame;path/to/my/myCustomLaunchScript.sh,myTargetSecondMame;path/to/my/mySecondCustomLaunchScript.sh\""
                        }});

    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-platform"}, "{platforms,list}",
                            "Force deploy only one selected platforms. "
                            "If this option is enabled then CQtDeployer will deploy only binaries of a selected platform. Supported values: "
                            "[win_x86 win_x86_64 win_arm win_arm64 linux_x86 linux_x86_64 linux_ARM linux_ARM64]"
                        }});


    group = "Part 3 Control of packages options";

    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-targetPackage"}, "{package;tar1,package;tar2}",
                            "Creates a new package and adds 'tar1 and tar2' to it. "
                            "If you want configure the package that do not have any targets use the allowEmptyPackages option."
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-qmlOut"}, "{package;path,path}",
                            "Sets path to qml out directory"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-libOut"}, "{package;path,path}",
                            "Sets path to libraries out directory"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-trOut"}, "{package;path,path}",
                            "Sets path to Sets path to translations out directory"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-pluginOut"}, "{package;path,path}",
                            "Sets path to plugins out directory"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-binOut"}, "{package;path,path}",
                            "Sets path to binary out directory"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-recOut"}, "{package;path,path}",
                            "Sets path to recurses out directory"
                        }});

    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-extraDataOut"}, "{package;path,path}",
                            "Sets path to extra data files out directory. By Default it is root dir of the distribution."
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-name"}, "{package;val,val}",
                            "Sets name for a package."
                            "If this if you do not specify a package, the value will be assigned to the default package ("")"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-description"}, "{package;val,val}",
                            "Sets description for a package"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-deployVersion"}, "{package;val,val}",
                            "Sets version for a package"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-releaseDate"}, "{package;val,val}",
                            "Sets release date for a package"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-publisher"}, "{package;val,val}",
                            "Sets publisher for a package"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-homePage"}, "{package;val,val}",
                            "Sets the home page url for a package"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-prefix"}, "{package;val,val}",
                            "Sets the prefix for the package relatively a target directory "
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-extraData"}, "{package;val,val}",
                            "Adds the extra files or directories like a target. The selected directory will be copy to the extraDataOut location with save own structure."
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-tr"}, "{package;val,val}",
                            "Adds qm files into the translations folder."
                        }});

    group = "Part 4 Control of target options";

    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-icon"}, "{target;val,val}",
                            "Sets path to icon for a targets"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-disableRunScript"}, "{target;val,val}",
                            "Disables a generation of run script for selected targets"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-disableShortCut"}, "{target;val,val}",
                            "Disables a generation of shortcut for selected targets"
                        }});

    help.insert(group, {QuasarAppUtils::OptionData{
                           {"-extraDepends"}, "{target;val,val}",
                           "Adds extra dependencies for target, if target is skipped then for rest of all targets",
                           "Example -extraDepends libssl.so or -targetPackage packageName;myExecutable -extraDepends packageName;libssl.so"
                       }});

    group = "Part 5 Plugins Control Options";

    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-extraPlugin"}, "{package;val1;val2,SingeleVal}",
                            "Sets an additional path to third-party application plug-in"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-enablePlugins"}, "{package;val1;val2,SingeleVa}",
                            "Enables additional plugins for distribution."
                            " By default disabled next plugins: " + PluginsParser::defaultForbidenPlugins().join(',') + " if you want enable"
                            " it then use '-enablePlugins " + PluginsParser::defaultForbidenPlugins().join(',') + "' option"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-disablePlugins"}, "{package;val1;val2,SingeleVal}",
                            "Disables plugins for distribution. "
                            "You can disable any plugin of your Qt build, just see the yourQtFolder/plugins forlder for available plugins."
                            " Example if you want disable qxcb plugin: -disablePlugins qxcb."
                            " Note that the name of the plugin is indicated without its extension"
                        }});

    group = "Part 6 QtInstallFramework options";

    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-qifStyle"}, "{path/to/style.css}",
                            "Sets the path to the CSS style file or sets the default style."
                            " Available styles: quasar, quasarDark"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-qifBanner"}, "{path/to/banner.png}",
                            "Sets path to the banner png file."
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-qifLogo"}, "{path/to/logo.png}",
                            "Sets path to the logo png file."
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-qifOut"}, "{nameOfOutputInstallerFile}",
                            "Sets name of output qifw installer. Note: on Windows, the exe suffix will be added to the installer automatically."
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-qifConfig"}, "{path/to/config.xml}",
                            "Sets a custom path to the configure file of the qt ifw installer. By default it is qif/config/config.xml. Note This path sets releative target folder (sets by TargetDir option)."
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-qifPackages"}, "{path/to/packagesFodoler}",
                            "Sets a custom path to the packages directories. By default it is qif/packages. Note This path sets releative target folder (sets by TargetDir option)."
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-qifResources"}, "{path/to/resources1.qrc,path/to/resources2.qrc}",
                            "Sets a custom path to the resources files. By default this option is skipped. Note This path sets releative target folder (sets by TargetDir option)."
                        }});

    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-targetDir"}, "{params}",
                            "Sets target directory(by default it is the path to the first deployable file)"
                        }});

    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-qifArchiveFormat"}, "[7z|zip|tar|tar.gz|tar.bz2|tar.xz]",
                            "Sets the format used when packaging new component data archives."
                            " If you omit this option, the 7z format will be used as a default. "
                        }});

    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-binarycreator"}, "{binarycreator command}",
                            "Sets new binarycreator command.",
                            "cqtdeployer -bin my.exe qifw -binarycreator 'wine path/to/binarycreator.exe'"
                        }});

    group = "Part 7 Deb package options";

    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-debOut"}, "{package;nameOfOutputDebFile,nameOfOutputDebFile}",
                            "Sets name of the output debian file. This option can be work with multiple packages"
                        }});

    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-installDirDeb"}, "{params}",
                            "Sets install target directory fordebian package (by default it is /opt path)"
                        }});


    group = "Part 8 zip package options";

    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-zipOut"}, "{package;nameOfOutputZipFile,nameOfOutputZipFile}",
                            "Sets name of the output zip arrhive. This option can be work with multiple packages"
                        }});

    return help;
}

void DeployCore::help() {

    auto localHelp = QuasarAppUtils::Params::getHelpOfInputOptions();

    if(!localHelp.isEmpty()) {
        QuasarAppUtils::Help::print(localHelp);
        return;
    }

    // help extra data;
    QuasarAppUtils::Help::Charters help = {
        {
            "Part 0 General", {
                {"CQtDeployer version", getAppVersion()},
                {"Usage", "cqtdeployer <-bin [params]> [options]"},
            }
        },
        {
            "Support", {
                {"Support for you", "If you have any questions or problems with cqtdeployer you can write to us about the problem on the GitHub page: https://github.com/QuasarApp/CQtDeployer/issues"},
                {"Support for us", "If you liked cqtdeployer, then you can join any patreon plan of your choice for support us: https://www.patreon.com/QuasarApp"},
            }
        }
    };

    help += QuasarAppUtils::Params::getHelp();

    QuasarAppUtils::Help::print(help);

    return;
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
    return CQT_DEPLOYER_VERSION;
}

QString DeployCore::getAppVersionName() {
    if (isSnap()) {
        return "*** Cool Core (snap) ***";
    }
    return "*** Cool Core ***";
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

QString DeployCore::findProcess(const QString &env, const QString& proc, bool ignoreSymLinks) {
    auto list = env.split(DeployCore::getEnvSeparator());

    auto findEntries = QDir::NoDotAndDotDot | QDir::Files;
    if (ignoreSymLinks) {
        findEntries = findEntries | QDir::NoSymLinks;
    }

    for (const auto& path : list) {
        auto files = QDir(path).entryInfoList(findEntries);

        for (const auto& bin : files) {
            if (bin.baseName().compare(proc, DeployCore::getCaseSensitivity()) == 0 && bin.isExecutable()) {
                return bin.absoluteFilePath();
            }
        }
    }

    // working only for the snap version of cqtdeployer ...
    if (isSnap()) {
        for (const auto& path : list) {
            auto files = QDir(transportPathToSnapRoot(path)).entryInfoList(findEntries);

            for (const auto& bin : files) {
                if (bin.baseName().compare(proc, DeployCore::getCaseSensitivity()) == 0) {
                    return bin.absoluteFilePath();
                }
            }
        }
    }

    return "";
}

QStringList DeployCore::debugExtensions() {
    // from 6.5 windows version of Qt use a "gebug" extension too.
    return {".debug", ".pdb"};
}

bool DeployCore::isDebugFile(const QString &file) {
    auto debug = debugExtensions();
    for (const auto& debugEx: debug) {
        if (file.contains(debugEx, Qt::CaseInsensitive)) {
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
        QuasarAppUtils::Params::log("is not a standard qt repo", QuasarAppUtils::Debug);
        return static_cast<MSVCVersion>(res);
    }

    auto msvcPath = dir.absolutePath();

    if (!(dir.cdUp() && dir.cdUp())) {
        QuasarAppUtils::Params::log("is not a standard qt repo", QuasarAppUtils::Debug);
        return static_cast<MSVCVersion>(res);
    }

    if (!msvcPath.contains("msvc")) {
        QuasarAppUtils::Params::log("vcredist not defined", QuasarAppUtils::Debug);
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
    else if (version == "2022") {
        res |= MSVC_22;
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

    if (msvc == MSVCVersion::MSVC_Unknown) {
        return "";
    }

    QDir dir = _qtbinDir;

    if (!(dir.cdUp() && dir.cdUp() && dir.cdUp() && dir.cd("vcredist"))) {
        QuasarAppUtils::Params::log("vcredist not found!");
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
    if (msvc & MSVCVersion::MSVC_13) {
        return "msvc2013";
    } else if (msvc & MSVCVersion::MSVC_15) {
        return "msvc2015";
    } else if (msvc & MSVCVersion::MSVC_17) {
        return "msvc2017";
    } else if (msvc & MSVCVersion::MSVC_19) {
        return "msvc2019";
    } else if (msvc & MSVCVersion::MSVC_22) {
        return "msvc2022";
    }

    return "";
}

QString DeployCore::getMSVCVersion(MSVCVersion msvc) {
    if (msvc & MSVCVersion::MSVC_x32) {
        return "x86";
    } else if (msvc & MSVCVersion::MSVC_x64) {
        return "x64";
    }

    return "";
}

QtMajorVersion DeployCore::isQtLib(const QString &lib) {
    QFileInfo info(lib);

    QtMajorVersion isQt = isQtLibName(lib);

    if (_config && !_config->qtDir.isQt(info.absoluteFilePath())) {
        return QtMajorVersion::NoQt;
    }

    return isQt;
}

QtMajorVersion DeployCore::isQtLibName(const QString &lib) {
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
    if (fileName.contains("Qt4", getCaseSensitivity(fileName))) {
        isQt = QtMajorVersion::Qt4;
    } else if (fileName.contains("Qt5", getCaseSensitivity(fileName))) {
        isQt = QtMajorVersion::Qt5;
    } else if (fileName.contains("Qt6", getCaseSensitivity(fileName))) {
        isQt = QtMajorVersion::Qt6;
    }

    if (isQt && QuasarAppUtils::Params::isEndable("noQt")) {
        return QtMajorVersion::NoQt;
    }

    return isQt;
}

bool DeployCore::isExtraLib(const QString &lib) {
    QFileInfo info(lib);
    return _config->extraPaths.contains(info.absoluteFilePath());
}

bool DeployCore::isAlienLib(const QString &lib) {
    return lib.contains("/opt/", Qt::CaseSensitive) ||
            lib.contains("/PROGRAM FILES", Qt::CaseInsensitive);
}

bool DeployCore::isAllowedLib(const QString &lib) {
    QFileInfo info(lib);
    return _config->allowedPaths.contains(info.absoluteFilePath());
}

QStringList DeployCore::Qt3rdpartyLibs(Platform platform) {

    QStringList result;

    result << QStringList {
              // SQL LIBS
              // See task https://github.com/QuasarApp/CQtDeployer/issues/367
              "libpq",
              "mysqlclient",

              // SLL Libs
              // See task https://github.com/QuasarApp/CQtDeployer/issues/620
              "libcrypto",
              "libssl",
              "libeay32",
              "ssleay32",

};

    if (platform & Platform::Win) {
        result << QStringList {
                  // Qml Gl driver wraper
                  "d3dcompiler_47",
                  "libEGL",
                  "libGLESv2",

                  // gcc runtime libs ow mingw
                  "libgcc_s_seh",
                  "libgcc_s_dw2",
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

QString DeployCore::platformToString(Platform platform) {
    int platformVal = 1;
    QString result;

    QHash<int, QString> platformsMap = {
        {Platform::Win32, "win_x86"},
        {Platform::Win64, "win_x86_64"},
        {Platform::Win_ARM_32, "win_arm"},
        {Platform::Win_ARM_64, "win_arm64"},
        {Platform::Unix_x86_32, "linux_x86"},
        {Platform::Unix_x86_64, "linux_x86_64"},
        {Platform::Unix_ARM_32, "linux_ARM"},
        {Platform::Unix_ARM_64, "linux_ARM64"},
        {Platform::WebGl, "WebGl"},
        {Platform::WebRemote, "WebRemote"},
        {Platform::GeneralFile, "GeneralFile"}
    };

    while (platformVal <= Platform::GeneralFile) {

        if (platformVal & platform) {
            result.push_back((result.size()? ", " : "") + platformsMap.value(platformVal, " Unknown"));
        }

        platformVal = platformVal << 1;


    }

    return result;
}

Platform DeployCore::getPlatformFromString(const QString &platformName) {

    if (platformName == "auto") {
        return Platform(0);
    }

    QHash<QString, Platform> platformsMap = {
        {"win_x86", Platform::Win32},
        {"win_x86_64", Platform::Win64},
        {"win_arm", Platform::Win_ARM_32},
        {"win_arm64", Platform::Win_ARM_64},
        {"linux_x86", Platform::Unix_x86_32},
        {"linux_x86_64", Platform::Unix_x86_64},
        {"linux_ARM", Platform::Unix_ARM_32},
        {"linux_ARM64", Platform::Unix_ARM_64},
        {"WebGl", Platform::WebGl},
        {"WebRemote", Platform::WebRemote},
        {"GeneralFile", Platform::GeneralFile}
    };

    return platformsMap.value(platformName, Platform(0));
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

        if (path.size() && path[0] != QString("/")) {
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

void DeployCore::printInternalError(const char * function, const char* file, int line ) {
    QuasarAppUtils::Params::log(QString("Internal error ocurred in %0 (%1:%2).").arg(function, file).arg(line),
                                QuasarAppUtils::Error);
    QuasarAppUtils::Params::log(QString("If you see this message please create a new issue"
                                        " about this problem on the official github page"
                                        " https://github.com/QuasarApp/CQtDeployer/issues/new/choose. "),
                                QuasarAppUtils::Error);
}

QFileInfo DeployCore::findItem(const QString &bin) {
    auto prefixes = QuasarAppUtils::Params::getArg("binPrefix").
            split(DeployCore::getSeparator(0), splitbehavior);

    for (const QString& prefix :std::as_const(prefixes)) {
        QFileInfo info(prefix + "/" + bin);

        if (info.exists()) {
            return info;
        }
    }

    return QFileInfo(bin);
}

Qt::CaseSensitivity DeployCore::getCaseSensitivity(const QString &checkedFile) {

    if (checkedFile.isEmpty()) {
#ifdef Q_OS_WIN
        return Qt::CaseInsensitive;
#else
        return Qt::CaseSensitive;
#endif
    }

    QString sufix = QFileInfo(checkedFile).completeSuffix();
    if (sufix.contains("dll", Qt::CaseInsensitive) ||
            sufix.contains("exe", Qt::CaseInsensitive)) {
        return Qt::CaseInsensitive;
    }

    return Qt::CaseSensitive;
}



QString DeployCore::getPlatformLibPrefix(Platform plarform) {

    if (plarform & Platform::Unix_x86_64) {
        return "x86_64-linux-gnu";
    } else if (plarform & Platform::Unix_ARM_64) {
        return "aarch64-linux-gnu";
    } else if (plarform & Platform::Unix_x86_32) {
        return "x86_32-linux-gnu";
    } else if (plarform & Platform::Unix_ARM_32) {
        return "arm-linux-gnu";
    } else if (plarform & Platform::Win64) {
        return "";
    } else if (plarform & Platform::Win32) {
        return "";
    } else if (plarform & Platform::Win_ARM_64) {
        return "";
    } else if (plarform & Platform::Win_ARM_32) {
        return "";
    }

    // not supported
    return "";
}

int DeployCore::qtVersionToString(QtMajorVersion qtVersion) {
    if (qtVersion & QtMajorVersion::Qt6) {
        return 6;
    } else if (qtVersion & QtMajorVersion::Qt5) {
        return 5;
    } else if (qtVersion & QtMajorVersion::Qt4) {
        return 4;
    }

    return 0;
}

bool DeployCore::isDebianQt(const QString &qtRoot) {
    return qtRoot.contains("/usr/lib/") || qtRoot.contains("/usr/bin");
}

QString DeployCore::systemLibsFolderName() {
    return "systemLibs";
}

QString DeployCore::getLibCoreName(const QFileInfo &info) {
    auto baseName = info.baseName();
    QString result;    if (baseName.left(3) == "lib") {
        result = baseName.mid(3);
    } else {
        result = baseName;
    }

    return result;
}

uint qHash(WinAPI i) {
    return static_cast<uint>(i);
}
