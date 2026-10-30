#include "appimage.h"

#include <deployconfig.h>
#include <deploycore.h>
#include <packagecontrol.h>
#include <pathutils.h>
#include <quasarapp.h>

#include <QDir>
#include <QFile>
#include <QSysInfo>

namespace {

// The AppImage template folder of the Deploy resources (the same
// ":/Templates/AppImage/..." prefix pattern as the deb and the qif
// templates, see the DeployResources.qrc).
const QString AppImageTemplateDir = ":/Templates/AppImage/Distributions/Templates/appimage";

// The icon formats supported by the AppImage tool (the icon is looked up
// by the desktop file Icon= name with the .png/.svg/.svgz/.xpm
// extensions, see the appimagetool).
const QStringList AppImageIconSuffixes = {"png", "svg", "svgz", "xpm"};

/**
 * @brief isElfFile This method returns true if the @a file is an ELF
 *  binary (the 4 bytes magic check).
 */
bool isElfFile(const QString &file) {
    QFile f(file);
    if (!f.open(QIODevice::ReadOnly)) {
        return false;
    }

    return f.read(4) == QByteArray("\x7f" "ELF", 4);
}

} // namespace

AppImage::AppImage(FileManager *fileManager)
    :iDistribution(fileManager) {
    setLocation("defaultAppImageTemplate");
}

bool AppImage::deployTemplate(PackageControl &pkg) {
    // default template
    const DeployConfig *cfg = DeployCore::_config;

    auto list = pkg.availablePackages();
    for (auto it = list.begin();
         it != list.end(); ++it) {

        auto package = cfg->getDistroFromPackage(*it);

        TemplateInfo info;
        if (!collectInfo(package, info)) {
            return false;
        }

        const QString appDir = location(package);

        if (!QDir().mkpath(appDir)) {
            qCritical() << "Failed to create the AppDir: " + appDir;
            return false;
        }

        // The main executable of the AppImage: the first target (in the
        // sorted order of the package) that requires a shortcut, the
        // first target of the package as the fallback.
        const auto targets = package.targets();
        QString mainTarget;
        QString icon;
        for (const auto &target: targets) {
            const auto targetObject = cfg->targets().value(target);

            if (!targetObject.isValid()) {
                continue;
            }

            if (targetObject.getShortCut()) {
                mainTarget = target;
                icon = targetObject.getIcon();
                break;
            }
        }

        if (mainTarget.isEmpty() && targets.size()) {
            mainTarget = *targets.begin();
        }

        if (mainTarget.isEmpty()) {
            qCritical() << QString("The package '%0' has no targets, the AppImage package cannot be created.")
                                .arg(package.key());
            return false;
        }

        if (icon.isEmpty()) {
            icon = ":/shared/Distributions/Templates/Icon.png";
        }

        // The deployed data is moved to the root of the AppDir (the root
        // layout of the AppImage spec), so the binaries of the package
        // are available under the binOutDir of the package (relative to
        // the root of the AppDir).
        QString execDir = package.getBinOutDir().trimmed();
        while (execDir.startsWith('/')) {
            execDir.remove(0, 1);
        }
        while (execDir.endsWith('/')) {
            execDir.chop(1);
        }
        const QString exec = (execDir.isEmpty() ? QString() : execDir + "/")
                             + QFileInfo(mainTarget).baseName();

        info.Custom["$EXEC"] = exec;
        info.Custom["$ICON"] = info.Name;

        // The icon: the AppImage tool supports only the
        // png/svg/svgz/xpm icons, the other formats (for example the
        // ico icons of the Windows targets) are replaced with the
        // default icon of the CQtDeployer.
        QFileInfo iconInfo(icon);
        if (!AppImageIconSuffixes.contains(iconInfo.suffix(), Qt::CaseInsensitive)) {
            qWarning() << "The icon " + icon + " is not supported by the AppImage tool "
                           "(the png/svg/svgz/xpm icons are supported), the default icon is used.";
            icon = ":/shared/Distributions/Templates/Icon.png";
            iconInfo = QFileInfo(icon);
        }

        if (!copyFile(icon, appDir + "/" + info.Name + "." + iconInfo.suffix(), true)) {
            qCritical() << "Failed to copy the icon: " + icon;
            return false;
        }

        if (!pkg.movePackage(*it, dataLocation(package))) {
            return false;
        }

        if (!unpackDir(AppImageTemplateDir, appDir, info, {"", "desktop"})) {
            return false;
        }

        // The AppImage tool expects the desktop file with the name of the
        // application: <name>.desktop in the root of the AppDir.
        if (!QFile::rename(appDir + "/app.desktop", appDir + "/" + info.Name + ".desktop")) {
            qCritical() << "Failed to rename the desktop file of the AppDir: " + appDir;
            return false;
        }

        if (!QFile::setPermissions(appDir + "/AppRun", QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner |
                                           QFile::ReadGroup | QFile::ExeGroup | QFile::ReadOther | QFile::ExeOther)) {
            qWarning() << "Failed to set the executable permissions of the AppRun: " + appDir + "/AppRun";
        }

        // The appimagetool detects the architecture of the AppImage from
        // the ELF binaries of the AppDir. If the package contains no ELF
        // binary (for example the "noQt" general file targets) the
        // architecture of the current host is passed to the tool with
        // the ARCH environment variable (see the runCmd method).
        const bool needArch = !isElfFile(mainTarget);

        inouts.push_back({appDir, cfg->getTargetDir() + "/" + info.appimageOut, needArch});
    }

    return true;
}

bool AppImage::deployRawTemplate(PackageControl &pkg) {
    const DeployConfig *cfg = DeployCore::_config;

    auto list = pkg.availablePackages();
    for (auto it = list.begin();
         it != list.end(); ++it) {

        auto package = cfg->getDistroFromPackage(*it);
        auto local = location(package);

        if (!QDir().mkpath(local)) {
            qCritical() << "Failed to create the AppDir: " + local;
            return false;
        }

        // The placeholders of the templates are not replaced in the
        // getDefaultTemplate mode (the same behavior as the deb
        // templates).
        if (!unpackDir(AppImageTemplateDir, local, {}, {"", "desktop"})) {
            return false;
        }
    }

    return true;
}

bool AppImage::removeTemplate() const {
    const DeployConfig *cfg = DeployCore::_config;

    registerOutFiles();
    return QDir(cfg->getTargetDir() + "/" + getLocation()).removeRecursively();
}

Envirement AppImage::toolKitEnv() const {
    Envirement result;

    // If the appimagetool is already installed on the system (for
    // example as the "appimage" package of the Debian distributions)
    // the system tool is used, so the system PATH is searched before
    // the bundled tools (issue #898).
    result.addEnv(QProcessEnvironment::systemEnvironment().value("PATH"));

    const QString appPath = QuasarAppUtils::Params::getCurrentExecutableDir();
    result.addEnv(appPath);

    // SNAP and the QIFW installer layout: the AppImage folder is placed
    // one level above the executable (the "QIF" package of the
    // self-deploy configuration, the extraData "./AppImage" entry).
    result.addEnv(appPath + "/../AppImage/");

    // Flat package layout (issue #872) and the development build: the
    // AppImage tools are bundled next to the executable (the copyAppImage
    // target of the CMake build).
    result.addEnv(appPath + "/AppImage/");

    // The same fallback search as the QIF distribution.
#ifdef Q_OS_LINUX
    result.addEnvRec(appPath + "/../../AppImage/", 2);
#endif

#ifdef Q_OS_WIN32
    result.addEnvRec(appPath + "/../AppImage/", 2);
#endif

    return result;
}

bool AppImage::isAvailable() const {
    auto tool = DeployCore::findProcess(toolKitEnv().concatEnv(), "appimagetool");
    return !tool.isEmpty();
}

QProcessEnvironment AppImage::processEnvirement() const {
    return QProcessEnvironment::systemEnvironment();
}

QList<SystemCommandData> AppImage::runCmd() {
    QList<SystemCommandData> res;
    for (const auto &inout: std::as_const(inouts)) {

        auto tool = DeployCore::findProcess(toolKitEnv().concatEnv(), "appimagetool");
        if (tool.isEmpty()) {
            tool = "appimagetool";
        }

        SystemCommandData cmd;
        if (inout.needArch) {
            // The AppDir contains no ELF binary, so the appimagetool
            // cannot detect the architecture of the AppImage from the
            // content of the AppDir: the architecture of the current
            // host is passed with the ARCH environment variable of the
            // appimagetool.
            cmd.command = "env";
            cmd.arguments = {QString("ARCH=%0").arg(QSysInfo::currentCpuArchitecture()),
                             tool, inout.appDir, inout.output};
        } else {
            cmd.command = tool;
            cmd.arguments = {inout.appDir, inout.output};
        }

        res.push_back(cmd);
    }

    return res;
}

QStringList AppImage::outPutFiles() const {
    QStringList result;
    for (const auto &inout: std::as_const(inouts)) {
        result.push_back(inout.output);
    }
    return result;
}

QString AppImage::dataLocation(const DistroModule &module) const {
    // The deployed data is moved to the root of the AppDir (the root
    // layout of the AppImage spec).
    return location(module);
}

QString AppImage::location(const DistroModule &module) const {
    const DeployConfig *cfg = DeployCore::_config;

    auto name = getName(module);

    if (name.isEmpty())
        return cfg->getTargetDir() + "/" + getLocation() + "/" + module.key();

    return cfg->getTargetDir() + "/" + getLocation() + "/" + name;
}

