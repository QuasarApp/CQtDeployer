/*
 * Copyright (C) 2018-2026 QuasarApp.
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
#include <qaplatformutils.h>

#include <LIEF/MachO.hpp>

//QString DeployCore::qtDir = "";
//QStringList DeployCore::extraPaths = QStringList();


DeployConfig* DeployCore::_config = nullptr;

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

    qDebug() << "Current module " + QString::number(module);

    auto mod = getQtModule(path);
    qDebug() << "Add new module from path " + path  +
                                " module value " + QString::number(mod);


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
    if (lib.contains(erfexp)) {
        return true;
    }

    // macOS frameworks do not contain the Qt major version in the library
    // name (QtCore.framework instead of libQt6Core.dylib), so the
    // version-less module name is matched as well (issue #166).
    return lib.contains(QString(moduleLibrary).replace("QtX", "Qt"));
}

bool DeployCore::isMacOSFrameworkBinary(const QFileInfo &file) {
    if (!file.isFile() || !file.completeSuffix().isEmpty()) {
        return false;
    }

    auto root = macOSFrameworkRoot(file.absoluteFilePath());
    return !root.isEmpty();
}

std::unique_ptr<LIEF::MachO::FatBinary>
DeployCore::parseMachOFile(const QString &file) {
    QFileInfo info(file);
    if (!info.isFile()) {
        return nullptr;
    }

    // The LIEF magic pre-filter runs before the full file parsing, because
    // the LIEF MachO parser logs an error message for every non Mach-O file
    // and the parsed files are checked in the loops of the dependency scan
    // and the deploy step (issue #166).
    if (!LIEF::MachO::is_macho(info.absoluteFilePath().toStdString())) {
        return nullptr;
    }

    // Full file parsing on the LIEF backend (issue #166). The LIEF MachO
    // parser accepts the thin (32/64-bit, both byte orders) and the
    // fat/fat64 binaries and rejects every other file, so the file suffix
    // is not required. The quick configuration skips the dyld opcode parsing
    // that is not needed - the load commands are always parsed.
    return LIEF::MachO::Parser::parse(
                info.absoluteFilePath().toStdString(),
                LIEF::MachO::ParserConfig::quick());
}

bool DeployCore::isMacOBinary(const LIEF::MachO::FatBinary *binary) {
    if (binary == nullptr || binary->empty()) {
        return false;
    }

    // LIEF 1.0.0 keeps a slice in the parse result even when its header
    // could not be read (truncated file) - such slices have an UNKNOWN
    // header magic and the file is not a valid Mach-O binary.
    for (const auto &slice : *binary) {
        if (slice.header().magic() == LIEF::MachO::MACHO_TYPES::UNKNOWN) {
            return false;
        }
    }

    return true;
}

bool DeployCore::isMacOBinary(const QFileInfo &file) {
    return isMacOBinary(parseMachOFile(file.absoluteFilePath()).get());
}

QStringList DeployCore::machORPaths(const LIEF::MachO::FatBinary *binary) {
    QStringList rpaths;
    QSet<QString> seen;

    if (!isMacOBinary(binary)) {
        return rpaths;
    }

    // The parser handles the thin (both byte orders) and the fat/fat64
    // binaries, so the entries are collected from all architecture slices.
    for (const LIEF::MachO::Binary &slice : *binary) {
        // The LIEF accessor is checked first, so this method keeps working
        // without the raw data fallback if a future LIEF release maps
        // LC_RPATH to the real 0x1C01 constant of the macOS ABI.
        for (const LIEF::MachO::RPathCommand &rpath : slice.rpaths()) {
            auto path = QString::fromStdString(rpath.path());
            if (!path.isEmpty() && !seen.contains(path)) {
                seen.insert(path);
                rpaths << path;
            }
        }

        // The LIEF 1.0.0 submodule maps LC_RPATH to 0x8000001C instead of
        // the macOS ABI constant 0x1C01, so the real LC_RPATH commands are
        // parsed as unknown commands. LIEF keeps the raw command bytes in
        // data(), and the path string follows the 8 byte command header in
        // both byte orders, so only the string itself is read here.
        for (const LIEF::MachO::LoadCommand &cmd : slice.commands()) {
            if (!LIEF::MachO::UnknownCommand::classof(&cmd)
                    || static_cast<const LIEF::MachO::UnknownCommand &>(cmd)
                       .original_command() != 0x1C01u) { // LC_RPATH
                continue;
            }

            auto raw = cmd.data();
            if (raw.size() <= 8) {
                continue;
            }

            auto path = QString();
            for (const auto *byte = raw.data() + 8; *byte != 0; ++byte) {
                path.append(static_cast<QChar>(*byte));
            }

            if (!path.isEmpty() && !seen.contains(path)) {
                seen.insert(path);
                rpaths << path;
            }
        }
    }

    return rpaths;
}

QStringList DeployCore::machORPaths(const QString &file) {
    return machORPaths(parseMachOFile(file).get());
}

QString DeployCore::macOSFrameworkRoot(const QString &path) {
    QFileInfo info(path);
    auto full = info.isDir() ? info.absoluteFilePath() : info.absolutePath();

    if (info.isDir() && info.fileName().endsWith(".framework", Qt::CaseSensitive)) {
        return info.absoluteFilePath();
    }

    // Walk up the path components and return the first directory that has
    // the .framework suffix. Works with relative paths (resolved against
    // the current working dir) and with non existing files.
    QDir dir(full);
    while (!dir.isRoot()) {
        if (dir.dirName().endsWith(".framework", Qt::CaseSensitive)) {
            return dir.absolutePath();
        }
        dir.cdUp();
    }

    return {};
}

QString DeployCore::macOSFrameworkBinary(const QString &frameworkDir) {
    QFileInfo fw(frameworkDir);
    if (!fw.isDir() || !fw.fileName().endsWith(".framework", Qt::CaseSensitive)) {
        return {};
    }

    auto baseName = QFileInfo(fw.fileName()).baseName();

    // Prefer the binary of the real version directory (for example A). The
    // Versions/Current symlink points to that directory, but the dyld
    // install names of the Qt frameworks are built from the real version
    // directory (QtCore.framework/Versions/A/QtCore), so the reference that
    // is used for the deployment layout must use the real path (issue #166).
    QDir versions(fw.absoluteFilePath() + "/Versions");
    for (const auto &v : versions.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot
                                                | QDir::System)) {
        if (QFileInfo(v.absoluteFilePath()).isSymLink()) {
            continue;
        }
        auto candidate = v.absoluteFilePath() + "/" + baseName;
        if (QFileInfo(candidate).isFile()) {
            return candidate;
        }
    }

    // Fallback: the Versions/Current symlink (frameworks without a real
    // version directory) or the bundle root itself.
    auto current = fw.absoluteFilePath() + "/Versions/Current/" + baseName;
    if (QFileInfo(current).isFile()) {
        return current;
    }

    auto rootBin = fw.absoluteFilePath() + "/" + baseName;
    if (QFileInfo(rootBin).isFile()) {
        return rootBin;
    }

    return {};
}

QString DeployCore::normalizeMacOSLibName(const QString &name) {
    auto n = name;

    // Remove the dyld specific prefixes.
    static const QStringList dyldPrefixes = {
        "@rpath/", "@loader_path/", "@executable_path/", "@relocated/"
    };
    for (const auto &prefix : dyldPrefixes) {
        if (n.startsWith(prefix, Qt::CaseSensitive)) {
            n = n.mid(prefix.size());
            break;
        }
    }

    // Framework references keep the full Name.framework/... tail,
    // because this is the layout that must exist in the deployed dir.
    auto fwSuffix = n.indexOf(".framework", Qt::CaseSensitive);
    if (fwSuffix != -1) {
        auto start = n.lastIndexOf('/', fwSuffix);
        return n.mid(start + 1);
    }

    // Plain dylibs are referenced by the file name only.
    return QFileInfo(n).fileName();
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
                            {"qtifw","-qtifw"}, "",
                            "Create the QIF installer for deployment programm"
                            " You can specify the path to your own installer template.",
                            "Examples: cqtdeployer -qtifw path/to/myCustom/qifw."
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"qif","-qif"}, "",
                            "Create the QIF installer for deployment programm"
                            " (DEPRECATED, use the qtifw option).",
                            "",
                            "The 'qif' option has been renamed to 'qtifw'. "
                            "Please use the 'qtifw' option instead. "
                            "The 'qif' option will be removed in a future release."
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
                            {"appimage"}, "",
                            "Create the AppImage package for deployment programm (issue #898). "
                            "The AppImage packages are created with the appimagetool, which is "
                            "bundled within the CQtDeployer package (the AppImage folder next to "
                            "the executable) or installed on the system (for example the appimage "
                            "package of the Debian distributions). "
                            "This option is available only on the Linux host platforms. "
                            "For more info see documentation page "
                            "https://quasarapp.github.io/CQtDeployer/md/AppImage/"
                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"deb", "-deb"}, "",
                            "Create the deb package for deployment programm"
                            " You can specify the path to your own debian template.",
                            " cqtdeployer -deb path/to/myCustom/templates"
                            " Where: the custom path should point to a folder with your packages templates, and template name should match with real package name."
                            " For Exmaple: cqtdeployer -deb path/to/tempalates -name appnameOrPackageName "
                            " path/to/tempalates/appnameOrPackageName/DEBIAN"
                            " To get more information see documentation page "
                            "https://quasarapp.ddns.net:3031/docs/QuasarApp/CQtDeployer/latest/md_md_2en_2DEB.html"

                        }});
    help.insert(group, {QuasarAppUtils::OptionData{
                            {"dmg"}, "",
                            "Create the DMG package for deployment programm (issue #899). "
                            "The DMG packages are created with the hdiutil tool "
                            "of the macOS system (the same way the deb backend "
                            "uses the dpkg-deb tool of the Linux system). "
                            "The DMG volume contains the deployed package (the "
                            "ready-to-use application), the application icon, "
                            "the README file with the name/description/version/"
                            "publisher of the package and the Applications "
                            "symbolic link of the standard drag-to-install UX. "
                            "The name of the volume is the name of the package "
                            "(the name option), the name of the output file is "
                            "set by the dmgOut option. "
                            "This option is available only on the macOS host "
                            "platforms. "
                            "For more info see documentation page "
                            "https://quasarapp.github.io/CQtDeployer/md/DMG/"
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
                            "Extracts defaults deb or qtifw templates."
                            " All templates extract into targetDirectory."
                            " For change target directory use the targetDir option.",
                            "cqtdeployer -bin myExecutable getDefaultTemplate qtifw deb."
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
                            {"-qtRootDir"}, "{params}",
                            "Sets the root path of the Qt installation. "
                            "CQtDeployer will try to detect the qmake or qmake6 "
                            "executable from this path and use it to parse the Qt "
                            "module locations. If the qmake executable is not found "
                            "the Qt paths are reconstructed from the root path.",
                            "-qtRootDir ~/Qt/6.5.2/gcc_64 or "
                            "-qtRootDir D:/Qt/6.5.2/mingw_64"
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
                            "\"-extraLibs mySql\" - forces to copy all libraries whose names contain mySql to the project folder."
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
                            {"-qifSign"}, "{code-signing-identity}",
                            "Sets the code signing identity of the Qt Installer Framework "
                            "binarycreator (the -s option). Available only on the macOS hosts: "
                            "the generated app bundle of the installer is signed with the "
                            "specified identity (issue #865). Use the '-' identity value for "
                            "the ad-hoc signature (no signing certificate required).",
                            "cqtdeployer -bin myApp qtifw -qifSign 'Developer ID Application: "
                            "QuasarApp (ABC1234)'"
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

    group = "Part 9 AppImage package options";

    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-appimageOut"}, "{package;nameOfOutputAppImageFile,nameOfOutputAppImageFile}",
                            "Sets name of the output AppImage file. This option can be work with multiple packages"
                        }});

    group = "Part 10 DMG package options";

    help.insert(group, {QuasarAppUtils::OptionData{
                            {"-dmgOut"}, "{package;nameOfOutputDmgFile,nameOfOutputDmgFile}",
                            "Sets name of the output DMG file. This option can be work with multiple packages"
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
    if (QuasarAppUtils::PlatformUtils::isSnap()) {
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

    auto findEntries = QDir::NoDotAndDotDot | QDir::Files | QDir::Hidden;
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
    if (QuasarAppUtils::PlatformUtils::isSnap()) {
        for (const auto& path : list) {
            auto files = QDir(QuasarAppUtils::PlatformUtils::transportPathToSnapRoot(path)).
                         entryInfoList(findEntries);

            for (const auto& bin : files) {
                if (bin.baseName().compare(proc, DeployCore::getCaseSensitivity()) == 0) {
                    return bin.absoluteFilePath();
                }
            }
        }
    }

    return "";
}

QString DeployCore::findQmakeInQtRoot(const QString &qtRootDir) {
    QFileInfo rootInfo(qtRootDir);
    if (!rootInfo.isDir()) {
        return "";
    }

    // Qt 5 kits provide the `qmake` executable and Qt 6 kits the `qmake6`
    // executable. On Windows the executables have the .exe suffix
    // (issue #788).
    static const QStringList qmakeNames = {"qmake", "qmake6",
                                           "qmake.exe", "qmake6.exe"};

    // The qmake executable lives in <qtRootDir>/bin. If the given
    // directory has no bin sub-directory, the user most likely passed
    // the bin directory itself, so it is checked as a fallback.
    const QStringList dirs = {rootInfo.absoluteFilePath() + "/bin",
                              rootInfo.absoluteFilePath()};

    for (const auto &dirPath : dirs) {
        QDir dir(dirPath);
        if (!dir.exists()) {
            continue;
        }

        // First pass: executable qmake files (the normal case).
        // Second pass: any regular qmake file (for example a qmake of
        // the snap root FS, that cannot be executed directly).
        for (int pass = 0; pass < 2; ++pass) {
            for (const auto &name : qmakeNames) {
                auto file = QFileInfo(dir.filePath(name));
                if (!file.isFile()) {
                    continue;
                }
                if (pass == 0 && !file.isExecutable()) {
                    continue;
                }
                return file.absoluteFilePath();
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
            || file.completeSuffix().contains("dll", Qt::CaseInsensitive)
            || file.completeSuffix().contains("dylib", Qt::CaseInsensitive)
            // macOS framework binaries have no suffix
            // (QtCore.framework/Versions/A/QtCore), see issue #166.
            || isMacOSFrameworkBinary(file);
}

MSVCVersion DeployCore::getMSVC(const QString &_qtBin) {
    int res = MSVCVersion::MSVC_Unknown;

    QDir dir = QFileInfo(_qtBin).absoluteFilePath();

    if (!dir.cdUp()) {
        qDebug() << "is not a standard qt repo";
        return static_cast<MSVCVersion>(res);
    }

    auto msvcPath = dir.absolutePath();

    if (!(dir.cdUp() && dir.cdUp())) {
        qDebug() << "is not a standard qt repo";
        return static_cast<MSVCVersion>(res);
    }

    if (!msvcPath.contains("msvc")) {
        qDebug() << "vcredist not defined";
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
        qDebug() << "vcredist not found!";
        return "";
    }

    auto infoList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden);

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

    if (!isQt) {
        // macOS Qt frameworks do not contain the major version in the
        // library name (QtCore.framework instead of libQt6Core.dylib), so
        // the version of the configured Qt installation is used as a
        // fallback (issue #166).
        auto frameworkRoot = macOSFrameworkRoot(info.absoluteFilePath());
        if (!frameworkRoot.isEmpty() && _config &&
            _config->qtDir.getQtVersion() != QtMajorVersion::NoQt) {
            isQt = _config->qtDir.getQtVersion();
        }
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
        {Platform::MinGW_Msvcrt, "MinGW_Msvcrt"},
        {Platform::MinGW_Ucrt, "MinGW_Ucrt"},
        {Platform::MSVC, "MSVC"},
        {Platform::Unix_x86_32, "linux_x86"},
        {Platform::Unix_x86_64, "linux_x86_64"},
        {Platform::Unix_ARM_32, "linux_ARM"},
        {Platform::Unix_ARM_64, "linux_ARM64"},
        {Platform::Mac_X86_64, "mac_x86_64"},
        {Platform::Mac_ARM_64, "mac_arm64"},
        {Platform::WebGl, "WebGl"},
        {Platform::WebRemote, "WebRemote"},
        {Platform::GeneralFile, "GeneralFile"}
    };

    while (platformVal <= Platform::Mac_ARM_64) {

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
        {"win_x86_msvcrt", Platform::Win32_MinGW_Msvcrt},
        {"win_x86_64_msvcrt", Platform::Win64_MinGW_Msvcrt},
        {"win_x86_ucrt", Platform::Win32_MinGW_Ucrt},
        {"win_x86_64_ucrt", Platform::Win64_MinGW_Ucrt},
        {"win_x86_msvc", Platform::Win32_MSVC},
        {"win_x86_64_msvc", Platform::Win64_MSVC},
        {"linux_x86", Platform::Unix_x86_32},
        {"linux_x86_64", Platform::Unix_x86_64},
        {"linux_ARM", Platform::Unix_ARM_32},
        {"linux_ARM64", Platform::Unix_ARM_64},
        {"mac_x86_64", Platform::Mac_X86_64},
        {"mac_arm64", Platform::Mac_ARM_64},
        {"WebGl", Platform::WebGl},
        {"WebRemote", Platform::WebRemote},
        {"GeneralFile", Platform::GeneralFile}
    };

    return platformsMap.value(platformName, Platform(0));
}

namespace {
    // The concrete platform types that can be the members of the package
    // platform pool. The Platform enum is a bit mask, so the pool is a
    // single Platform type that is a result of the | bit operation over
    // these concrete values (issue #855, #889).
    const Platform packagePoolPlatforms[] = {
        Platform::Win32, Platform::Win64,
        Platform::Win_ARM_32, Platform::Win_ARM_64,
        Platform::Win32_MinGW_Msvcrt, Platform::Win64_MinGW_Msvcrt,
        Platform::Win32_MinGW_Ucrt, Platform::Win64_MinGW_Ucrt,
        Platform::Win32_MSVC, Platform::Win64_MSVC,
        static_cast<Platform>(Platform::Win_ARM_32 | Platform::MinGW_Msvcrt),
        static_cast<Platform>(Platform::Win_ARM_32 | Platform::MinGW_Ucrt),
        static_cast<Platform>(Platform::Win_ARM_32 | Platform::MSVC),
        static_cast<Platform>(Platform::Win_ARM_64 | Platform::MinGW_Msvcrt),
        static_cast<Platform>(Platform::Win_ARM_64 | Platform::MinGW_Ucrt),
        static_cast<Platform>(Platform::Win_ARM_64 | Platform::MSVC),
        Platform::Unix_x86_32, Platform::Unix_x86_64,
        Platform::Unix_ARM_32, Platform::Unix_ARM_64,
        Platform::Mac_X86_64, Platform::Mac_ARM_64,
        Platform::WebGl, Platform::WebRemote,
        Platform::GeneralFile
    };

    // The single platform comparison: the library must be fully compatible
    // with the platform, see issue #870. A plain (non flavored) Windows
    // library is compatible with a flavored platform of the same base
    // platform (issue #889): a library without a MinGW/MSVC runtime marker
    // in its import table (for example the plain C library libzip.dll) has
    // no runtime of its own and runs next to every Windows runtime set of
    // the same bitness, so a dependency that only has a generic candidate
    // must not be dropped by the comparison. Flavored candidates keep the
    // strict exact-match semantics, so a foreign flavored runtime set
    // (issue #870) is never a substitute for the library of the target
    // toolchain.
    bool isPlatformCompatibleSingle(Platform candidate, Platform wanted) {
        if (candidate == wanted) {
            return true;
        }

        const int winBase = Win32 | Win64 | Win_ARM_32 | Win_ARM_64;
        const int winFlavor = MinGW_Msvcrt | MinGW_Ucrt | MSVC;

        return (candidate & ~winBase) == 0 &&   // plain Windows library
               (wanted & winFlavor) != 0 &&      // flavored pool platform
               (wanted & winBase) == candidate;  // same base platform
    }
}

bool DeployCore::isPlatformCompatible(Platform candidate, Platform wanted) {
    // `wanted` is the pool of the platform types of the whole package (app
    // bundle) - a single Platform bit mask (the Platform enum is a bit mask,
    // so the pool is accessed with the & bit operation and grown with the |
    // bit operation, issue #889). The candidate is compatible when it is
    // compatible with at least one platform of the pool; a single platform
    // is just a pool with one element.
    for (const auto &platform : packagePoolPlatforms) {
        if ((wanted & platform) == platform &&
            isPlatformCompatibleSingle(candidate, platform)) {
            return true;
        }
    }

    return false;
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

void DeployCore::printInternalError(const char * function, const char* file, int line ) {
    qCritical() << QString("Internal error ocurred in %0 (%1:%2).").arg(function, file).arg(line);
    qCritical() << QString("If you see this message please create a new issue"
                                        " about this problem on the official github page"
                                        " https://github.com/QuasarApp/CQtDeployer/issues/new/choose. ");
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
    } else if (plarform & Platform::Mac) {
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
