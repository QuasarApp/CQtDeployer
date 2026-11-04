#include "dmg.h"

#include <deployconfig.h>
#include <deploycore.h>
#include <packagecontrol.h>
#include <pathutils.h>
#include <quasarapp.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>

namespace {

// The DMG template folder of the Deploy resources (the same
// ":/Templates/DMG/..." prefix pattern as the deb and the qif
// templates, see the DeployResources.qrc).
const QString DmgTemplateDir = ":/Templates/DMG/Distributions/Templates/dmg";

// The icon formats of the macOS hosts (the Finder of the macOS hosts
// renders the png and the icns icons of the DMG volume, the other
// formats - for example the ico icons of the Windows targets - are
// replaced with the default icon of the CQtDeployer).
const QStringList DmgIconSuffixes = {"png", "icns"};

/**
 * @brief sanitizeVolumeName This method returns the @a name sanitized
 *  for the -volname argument of the hdiutil: the HFS+ volume names do
 *  not allow the "/" and the ":" characters, so they are replaced with
 *  the "-" character.
 */
QString sanitizeVolumeName(const QString &name) {
    QString res = name;
    res.replace('/', '-');
    res.replace(':', '-');
    return res;
}

} // namespace

Dmg::Dmg(FileManager *fileManager)
    :iDistribution(fileManager) {
    setLocation("defaultDmgTemplate");
}

bool Dmg::deployTemplate(PackageControl &pkg) {
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

        const QString appFolder = info.Name.isEmpty() ? package.key() : info.Name;
        const QString root = location(package);

        if (!QDir().mkpath(root)) {
            qCritical() << "Failed to create the root folder of the DMG: " + root;
            return false;
        }

        // The icon: the icon of the first target (in the sorted order
        // of the package) that requires a shortcut, the default icon of
        // the CQtDeployer as the fallback. The icon file is placed in
        // the root of the DMG volume next to the application folder
        // (the same way the AppImage backend places the icon in the
        // root of the AppDir, issue #898).
        QString icon;
        for (const auto &target: package.targets()) {
            const auto targetObject = cfg->targets().value(target);

            if (!targetObject.isValid()) {
                continue;
            }

            if (targetObject.getShortCut()) {
                icon = targetObject.getIcon();
                break;
            }
        }

        if (icon.isEmpty()) {
            icon = ":/shared/Distributions/Templates/Icon.png";
        }

        QFileInfo iconInfo(icon);
        if (!DmgIconSuffixes.contains(iconInfo.suffix(), Qt::CaseInsensitive)) {
            qWarning() << "The icon " + icon + " is not supported by the Finder "
                           "of the macOS hosts (the png and the icns icons are "
                           "supported), the default icon is used.";
            icon = ":/shared/Distributions/Templates/Icon.png";
            iconInfo = QFileInfo(icon);
        }

        if (!copyFile(icon, root + "/" + appFolder + "." + iconInfo.suffix(), true)) {
            qCritical() << "Failed to copy the icon: " + icon;
            return false;
        }

        // The deployed data of the package is moved to the application
        // folder of the DMG volume (the ready-to-use application: the
        // executable, the libraries and the run script of the targets).
        if (!pkg.movePackage(*it, dataLocation(package))) {
            return false;
        }

        // The README file of the DMG volume: the name, the description,
        // the version, the publisher and the homepage of the package
        // (the same placeholder mechanism as the deb control template).
        if (!unpackDir(DmgTemplateDir, root, info, {""})) {
            return false;
        }

        // The Applications symbolic link of the standard drag-to-install
        // UX of the macOS DMG files (the user drags the application
        // folder into the Applications folder of the DMG window). The
        // symbolic link is a cosmetic feature, so a failure of its
        // creation is not fatal (issue #899).
        if (!QFile::link("/Applications", root + "/Applications")) {
            qWarning() << "Failed to create the Applications symbolic link of the "
                           "DMG volume: " + root;
        }

        // The name of the output DMG file: the dmgOut value of the
        // package (<name>.dmg by default), the .dmg suffix is appended
        // when it is missing (the hdiutil appends it too, so the
        // outPutFiles of the distribution must match the real file).
        QString out = info.dmgOut;
        if (!out.endsWith(".dmg", Qt::CaseInsensitive)) {
            out += ".dmg";
        }

        inouts.push_back({root, cfg->getTargetDir() + "/" + out,
                          sanitizeVolumeName(appFolder)});
    }

    return true;
}

bool Dmg::deployRawTemplate(PackageControl &pkg) {
    const DeployConfig *cfg = DeployCore::_config;

    auto list = pkg.availablePackages();
    for (auto it = list.begin();
         it != list.end(); ++it) {

        auto package = cfg->getDistroFromPackage(*it);
        auto local = location(package);

        if (!QDir().mkpath(local)) {
            qCritical() << "Failed to create the root folder of the DMG: " + local;
            return false;
        }

        // The placeholders of the templates are not replaced in the
        // getDefaultTemplate mode (the same behavior as the deb
        // templates).
        if (!unpackDir(DmgTemplateDir, local, {}, {""})) {
            return false;
        }
    }

    return true;
}

bool Dmg::removeTemplate() const {
    const DeployConfig *cfg = DeployCore::_config;

    registerOutFiles();
    return QDir(cfg->getTargetDir() + "/" + getLocation()).removeRecursively();
}

Envirement Dmg::toolKitEnv() const {
    Envirement result;

    // The hdiutil is a standard tool of the macOS system (/usr/bin/hdiutil),
    // so the system PATH is enough (the same way the deb backend relies
    // on the system PATH for the dpkg-deb tool).
    result.addEnv(QProcessEnvironment::systemEnvironment().value("PATH"));

    return result;
}

bool Dmg::isAvailable() const {
#ifdef Q_OS_MACOS
    auto tool = DeployCore::findProcess(toolKitEnv().concatEnv(), "hdiutil");
    return !tool.isEmpty();
#else
    return false;
#endif
}

QProcessEnvironment Dmg::processEnvirement() const {
    return QProcessEnvironment::systemEnvironment();
}

QList<SystemCommandData> Dmg::runCmd() {
    QList<SystemCommandData> res;
    for (const auto &inout: std::as_const(inouts)) {

        auto tool = DeployCore::findProcess(toolKitEnv().concatEnv(), "hdiutil");
        if (tool.isEmpty()) {
            tool = "hdiutil";
        }

        SystemCommandData cmd;
        // The hdiutil create command of the DMG (the man page of the
        // macOS system): the -volname value is the name of the volume
        // (the name of the package), the -srcfolder value is the root
        // folder of the volume, the -ov flag overwrites the existing
        // image and the UDBZ format is the compressed UDIF of the
        // standard macOS DMG files (issue #899).
        cmd.command = tool;
        cmd.arguments = {"create",
                         "-volname", inout.volumeName,
                         "-srcfolder", inout.input,
                         "-ov",
                         "-format", "UDBZ",
                         "-o", inout.output};

        res.push_back(cmd);
    }

    return res;
}

QStringList Dmg::outPutFiles() const {
    QStringList result;
    for (const auto &inout: std::as_const(inouts)) {
        result.push_back(inout.output);
    }
    return result;
}

QString Dmg::dataLocation(const DistroModule &module) const {
    // The deployed data is moved to the application folder of the DMG
    // volume (the root of the volume is the folder of the DMG window:
    // the application folder + the icon + the README + the Applications
    // symbolic link).
    const QString appFolder = module.name().isEmpty() ? module.key()
                                                      : module.name();
    return location(module) + "/" + appFolder;
}

QString Dmg::location(const DistroModule &module) const {
    const DeployConfig *cfg = DeployCore::_config;

    auto name = getName(module);

    if (name.isEmpty())
        return cfg->getTargetDir() + "/" + getLocation() + "/" + module.key();

    return cfg->getTargetDir() + "/" + getLocation() + "/" + name;
}

