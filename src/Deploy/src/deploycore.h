/*
 * Copyright (C) 2018-2026 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef DEPLOYUTILS_H
#define DEPLOYUTILS_H

#include <QStringList>
#include <QDebug>
#include <QFileInfo>
#include "deploy_global.h"
#include <params.h>

enum MSVCVersion: int {
    MSVC_Unknown =  0x0000,
    MSVC_x64 =      0x0001,
    MSVC_x32 =      0x0002,
    MSVC_13 =       0x0010,
    MSVC_15 =       0x0020,
    MSVC_17 =       0x0040,
    MSVC_19 =       0x0080,
    MSVC_22 =       0x0100,
};

/**
 * @brief The QtMajorVersion enum
 */
enum QtMajorVersion: int {
    NoQt = 0x0,
    Qt4 = 0x01,
    Qt5 = 0x02,
    Qt6 = 0x04
};

struct DEPLOYSHARED_EXPORT QtModuleEntry {
    quint64 module;
    const char *option;
    const char *libraryName;
    const char *translation;
};


enum Platform {
    UnknownPlatform = 0x0000,
    // Windows
    Win32           = 0x0001,
    Win64           = 0x0002,
    Win_ARM_32      = 0x0004,
    Win_ARM_64      = 0x0008,
    Win             = Win32 | Win64,
    // MinGW runtime flavor (issue #855). MinGW binaries can be linked against
    // the MSVC CRT (they import msvcrt.dll) or against the UCRT (they import
    // api-ms-win-crt-* modules). Mixing the two runtime sets in one deployed
    // bundle breaks the app (STATUS_ENTRYPOINT_NOT_FOUND / 0xc0000139).
    MinGW_Msvcrt    = 0x0800,
    MinGW_Ucrt      = 0x1000,
    Win32_MinGW_Msvcrt = Win32 | MinGW_Msvcrt,
    Win64_MinGW_Msvcrt = Win64 | MinGW_Msvcrt,
    Win32_MinGW_Ucrt   = Win32 | MinGW_Ucrt,
    Win64_MinGW_Ucrt   = Win64 | MinGW_Ucrt,
    // MSVC runtime flavor (issue #855). Binaries built with the MSVC
    // toolchain import the MSVC CRT (msvcr*.dll) and form a separate
    // platform family, so MSVC applications never resolve MinGW runtime
    // libraries (libstdc++/libgcc/libwinpthread) and vice versa. UCRT
    // MSVC binaries (vcruntime140.dll / api-ms-win-crt-* imports) cannot
    // be told apart from other UCRT binaries by their import table, so
    // they stay in the generic family.
    MSVC              = 0x2000,
    Win32_MSVC        = Win32 | MSVC,
    Win64_MSVC        = Win64 | MSVC,

    // Unix
    Unix_x86_32     = 0x0010,
    Unix_x86_64     = 0x0020,
    Unix_x86        = Unix_x86_32 | Unix_x86_64,
    Unix_ARM_32     = 0x0040,
    Unix_ARM_64     = 0x0080,
    Unix_ARM        = Unix_x86_32 | Unix_x86_64,
    Unix            = Unix_x86 | Unix_ARM,

    // macOS (issue #553). Mach-O binaries are parsed with LIEF.
    Mac_X86_64      = 0x4000,
    Mac_ARM_64      = 0x8000,
    Mac             = Mac_X86_64 | Mac_ARM_64,

    // Other

    // Web
    WebGl           = 0x0100,
    WebRemote       = 0x0200,

    GeneralFile     = 0x0400
};

/**
 * @brief The LibPriority enum This is library priority.
 * The lower the priority of the topics, the library is the most suitable for distribution.
 */
enum LibPriority : int {
    // ================= General Libraryes
    /// This is qt libraryes.
    QtLib = 0x0,
    /// This is user libraryes.
    ExtraLib,
    /// This is qt 3dParty and user libraryes. (distribution-safe libraries)
    AllowedLib,

    // ================= System Libraryes
    /// This is rest of all libraryes (frm system and another).
    SystemLib,
    /// This is libraryes from another proggram distributions.
    AlienLib,
    /// This is General Files.
    NotFile = 0xF,
};

enum class WinAPI : quint8 {
    NoWinAPI    = 0x00,
    Other       = 0x01,
    Core        = 0x02,
    Devices     = 0x04,
    Eventing    = 0x08,
    Crt         = 0x10,
    Security    = 0x20,
    Base        = 0x40
};

uint qHash (WinAPI i);

enum class RunMode: int {
    Info,
    Deploy,
    Clear,
    Init,
    Template
};

class Extracter;
class DeployConfig;

class DEPLOYSHARED_EXPORT DeployCore
{

private:
    static QString getMSVCName(MSVCVersion msvc);
    static QString getMSVCVersion(MSVCVersion msvc);

public:
    enum QtModule : quint64
    {
        NONE                      = 0x0000000000000000,
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
        Qt3DExtrasModule          = 0x0008000000000000,
        QtVirtualKeyboard         = 0x0010000000000000,
        // Qt6
        QtOpenGLWidgetsModule     = 0x0010000000000000,
        QtSvgWidgetsModule        = 0x0020000000000000,
        QtShaderToolsModule       = 0x0040000000000000
    };

    DeployCore() = delete;


    static QtModuleEntry qtModuleEntries[];

    static const DeployConfig * _config;

    static MSVCVersion getMSVC(const QString & _qtBin);
    static QString getVCredist(const QString & _qtBin);

    /**
     * @brief isQtLib This method check full path of the library. If the @a lib contains only name then this method retun QtMajorVersion::NoQt enum. For validate @a lib by name only use the DeployCore::isQtLibName method.
     * @param lib This is library full path..
     * @return major version of the Qt.
     */
    static QtMajorVersion isQtLib(const QString &lib);

    /**
     * @brief isQtLib This method check name of the library.
     * @param lib This is library full path..
     * @return major version of the Qt.
     */
    static QtMajorVersion isQtLibName(const QString &lib);

    static bool isExtraLib(const QString &lib);
    static QChar getSeparator(int lvl);
    static bool isAlienLib(const QString &lib);

    /**
     * @brief isAllowedLib This method checks the library if the library is allowed or not, allowet libraryes is added with extraLibs method.
     * @param lib This is library fuul path
     * @return true if lirary is allowed
     */
    static bool isAllowedLib(const QString &lib);

    /**
     * @brief QtThreethepartyLibs This method return list of 3rdparty libraryes of qt for selected platform.
     * @param platform This is OS name.
     * @return list of 3rdparty libs.
     * @note This method is hardcode.
     * @note See Task https://github.com/QuasarApp/CQtDeployer/issues/422 of the CQtDeployer project.
     */
    static QStringList Qt3rdpartyLibs(Platform platform);

    /**
     * @brief platformToString This method convert platform value to string value.
     * @param platform This is input platform value.
     * @return String name of the @a platform
     */
    static QString platformToString(Platform platform);

    /**
     * @brief getPlatformFromString This method return platform enum value form string.
     * @param platformName This is string platform value.
     * @return platform enum value form string.
     */
    static Platform getPlatformFromString(const QString &platformName);

    /**
     * @brief isPlatformCompatible This method returns true if a library with the
     * @a candidate platform can satisfy the dependencies of a library with the
     * @a wanted platform.
     *
     * Runtime flavored platforms (Win32/Win64_MinGW_Msvcrt, Win32/Win64_MinGW_Ucrt
     * and Win32/Win64_MSVC) only match the identical platform or a generic
     * (non flavored) platform of the same bitness, and never another runtime
     * flavor. This prevents mixing incompatible runtime sets in one deployed
     * bundle (issue #855), while keeping generic Windows libraries compatible
     * with MinGW and MSVC applications.
     */
    static bool isPlatformCompatible(Platform candidate, Platform wanted);

    static char getEnvSeparator();

    static LibPriority getLibPriority(const QString &lib);

    /**
     * @brief containsModule This method compare lib name and module of qt.
     *  The Qt major version is not required in the library name, because
     *  macOS frameworks do not contain it (QtCore.framework instead of
     *  libQt6Core.dylib) (issue #166).
     * @param moduleLibrary this is name of module library
     * @param lib This is library name
     * @return true if library has some module that as muduleIndex
     */
    static bool containsModule(const QString &moduleLibrary, const QString &lib);
    static DeployCore::QtModule getQtModule(const QString& path);
    static void addQtModule(DeployCore::QtModule& module, const QString& path);

    /**
     * @brief isConsole This method return true if the librarye depends of the gui libraryes.
     * @return true if the librarye is gui application.
     */
    static bool isGui(DeployCore::QtModule module);
    static RunMode getMode();
    static void help();
    static QuasarAppUtils::OptionsDataList avilableOptions();

    static QStringList extractTranslation(const QSet<QString> &libs);
    static QString getAppVersion();
    static QString getAppVersionName();

    static QString getQtVersion();
    static void printVersion();
    static int find(const QString &str, const QStringList &list);
    static bool isLib(const QFileInfo &file);
    static bool isExecutable(const QFileInfo &file);
    static bool isContainsArraySeparators(const QString& val,
                                          int lastLvl = 2);
    static QString findProcess(const QString& env, const QString& proc, bool ignoreSymLinks = false);

    static QStringList debugExtensions();
    static bool isDebugFile(const QString& file);

    /**
     * @brief getLibCoreName This method remove platfomr specificly prefixes and sufixes of the librarry.
     *  Example : getLibCoreName(libTest.so) return Test
     * @param info This is information about checked library.
     * @return return core name of the library.
     */
    static QString getLibCoreName(const QFileInfo& info);

    /**
     * @brief systemLibsFolderName This method return name of the systems librares.
     * @return Name of folder with system libraryes.
     * @note see https://github.com/QuasarApp/CQtDeployer/issues/396
     */
    static QString systemLibsFolderName();

    /**
     * @brief printInternalError This function print message of the internal error.
     * @param function This is function name.
     * @param file This is file name of code.
     * @param line This is line number of the code.
     */
    static void printInternalError(const char *function, const char* file, int line);

    /**
     * @brief findItem This method search input file in prefixes and return absolute path to the found file. If file is not exists the return empty string.
     * @param file This is file path. If the file path si absalute path then return @a file value.
     * @return file info of the found file.
     */
    static QFileInfo findItem(const QString &file);

    /**
     * @brief getCaseSensitivity This method return case sensitivity for a @a checkedFile. Usually return Qt::CaseSensiativy exept windows binaryes files like a dll and exe.
     * @param checkedFile This is checked file. By default empty value.
     * @return Qt CaseSensitivity value
     */
    static Qt::CaseSensitivity getCaseSensitivity(const QString& checkedFile = "");

    /**
     * @brief getPlatformLibPrefix This method return string value of the @a platform
     * @param plarform This is input platform
     * @return string value of the @a platfrom. If platform not supported strings implementations the return empty string.
     */
    static QString getPlatformLibPrefix(Platform plarform);

    /**
     * @brief qtVersionToString return integer interpritation of the Qt major version
     * @return integer implementation of major qt version.
     * @note return 0 if the @a qtVersion is invalid
     */
    static int qtVersionToString(QtMajorVersion qtVersion);

    /**
     * @brief isDebianQt this method return true if the @a qtRoot is debian system qt.
     * @param qtRoot path to qt root dir.
     * @return true if the @a qtRoot is debian system qt else false.
     */
    static bool isDebianQt(const QString& qtRoot);

    /**
     * @brief isMacOSFrameworkBinary This method return true if the @a file is
     *  a binary of the macOS .framework bundle. macOS libraries are stored in
     *  the Name.framework/Versions/<V>/Name layout, so the real library file
     *  has no file suffix and the bundle name (with the .framework suffix)
     *  lives in one of the parent directories (issue #166).
     * @param file This is the file that will be checked.
     * @return true if the file is a macOS framework binary else false.
     */
    static bool isMacOSFrameworkBinary(const QFileInfo &file);

    /**
     * @brief isMacOBinary This method return true if the @a file is a Mach-O
     *  binary (32/64-bit thin binaries in both byte orders and fat/universal
     *  binaries). The check is based on the magic number in the first bytes
     *  of the file, so the file does not need to be parseable by LIEF and the
     *  file suffix is not required (issue #166).
     * @param file This is the file that will be checked.
     * @return true if the file is a Mach-O binary else false.
     */
    static bool isMacOBinary(const QFileInfo &file);

    /**
     * @brief machORPaths This method return the list of the runtime search
     *  paths (LC_RPATH entries) of all architecture slices of the Mach-O
     *  binary @a file (issue #166). The entries are read directly from the
     *  binary, because the LIEF submodule of CQtDeployer maps LC_RPATH to a
     *  different load command constant than the macOS ABI (0x1C01), so the
     *  LIEF rpath() accessor does not find the real entries.
     * @param file This is the path to the Mach-O binary.
     * @return The list of the LC_RPATH entries (empty if the file is not a
     *  Mach-O binary or has no rpaths).
     */
    static QStringList machORPaths(const QString &file);

    /**
     * @brief macOSFrameworkRoot This method return the absolute path of the
     *  .framework bundle that contains the @a path. The path can point to the
     *  bundle itself, to the binary inside the bundle or to any other file of
     *  the bundle (Info.plist, resources, symlinks, ...).
     * @param path This is a file or directory path that is checked.
     * @return Absolute path to the .framework bundle or an empty string if the
     *  path is not part of a macOS framework (issue #166).
     */
    static QString macOSFrameworkRoot(const QString &path);

    /**
     * @brief macOSFrameworkBinary This method return the path of the real
     *  library binary of the .framework bundle (issue #166). If the bundle
     *  uses the Name.framework/Versions/<V>/Name layout the binary of the
     *  real version directory (for example Versions/A) is returned, because
     *  the dyld install names of the Qt frameworks are based on that path.
     * @param frameworkDir This is the path to the .framework bundle.
     * @return Absolute path to the framework binary or an empty string if the
     *  bundle is invalid or the binary is not found.
     */
    static QString macOSFrameworkBinary(const QString &frameworkDir);

    /**
     * @brief macOSFrameworkReference This method return the dyld reference of
     *  the library that is located at @a path relative to the directory that
     *  contains the bundle (issue #166).
     *  Examples :
     *   /opt/qt/lib/QtCore.framework/Versions/A/QtCore -> QtCore.framework/Versions/A/QtCore
     *   /opt/qt/lib/libfoo.dylib -> libfoo.dylib
     * @param path This is the path to the library file.
     * @return dyld reference of the library (file name or
     *  Name.framework/Versions/<V>/Name).
     */
    static QString macOSFrameworkReference(const QString &path);

    /**
     * @brief normalizeMacOSLibName This method normalize a Mach-O install name
     *  (LC_ID_DYLIB / LC_LOAD_DYLIB) to the name under which the library is
     *  registered in the deployment environment (issue #166).
     *  The dyld specific prefixes (@rpath/, @loader_path/, @executable_path/,
     *  @relocated/) and absolute path prefixes are removed. Framework
     *  references keep the full Name.framework/... tail, because this is the
     *  layout that must exist in the deployed directory.
     *  Examples :
     *   @rpath/libQt6Core.6.dylib -> libQt6Core.6.dylib
     *   /opt/homebrew/lib/QtCore.framework/Versions/A/QtCore -> QtCore.framework/Versions/A/QtCore
     *   QtCore.framework/Versions/A/QtCore -> QtCore.framework/Versions/A/QtCore
     * @param name This is the install name of the Mach-O library.
     * @return normalized library name.
     */
    static QString normalizeMacOSLibName(const QString &name);
};

#define internalError() DeployCore::printInternalError(__FUNCTION__, __FILE__, __LINE__)
#endif // DEPLOYUTILS_H
