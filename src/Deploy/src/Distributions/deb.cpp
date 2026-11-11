#include "deb.h"

#include <deployconfig.h>
#include <pathutils.h>
#include <packagecontrol.h>
#include <quasarapp.h>
#include <QFileInfo>

Deb::Deb(FileManager *fileManager):
    iDistribution(fileManager)
{
    setLocation("defaultDEBTemplate");
}

bool Deb::deployTemplate(PackageControl &pkg) {
    // default template
    const DeployConfig *cfg = DeployCore::_config;

    auto list = pkg.availablePackages();
    for (auto it = list.begin();
         it != list.end(); ++it) {
        auto package = cfg->getDistroFromPackage(*it);

        QString defaultPackageTempalte = ":/Templates/DEB/Distributions/Templates/deb";
        auto customTemplate = QuasarAppUtils::Params::getArg("deb", "");
        QHash<QString, QString> packagesTemplates;

        if (!customTemplate.isEmpty()) {

            qInfo() << "Using custom template for debian package: " + customTemplate;

            const auto availablePackages = QDir(customTemplate).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden);

            for (const auto& pkg: availablePackages) {
              packagesTemplates.insert(pkg.fileName(), pkg.absoluteFilePath());
            }
        }

        TemplateInfo info;
        if (!collectInfoWithDeployIcons(package, info)) {
            return false;
        }

        auto local = location(package);
        auto localData = dataLocation(package);

        if (!pkg.movePackage(*it, localData)) {
            return false;
        }

        QHash<QString, QString> replace = {
            {"default", info.Name}
        };

        if (!unpackDir(
                packagesTemplates.value(info.Name, defaultPackageTempalte),
                       local, info, {""}, replace)) {
            return false;
        }

        if (!QFile::setPermissions(local +  "/DEBIAN",  static_cast<QFile::Permission>(0x7775))) {

            qWarning() << "Failed to set permissions";
        }

        if (!addLicense(local, info.Name)) {
            return false;
        }

        inouts.push_back({local, cfg->getTargetDir() + "/" + info.debOut});
    }

    return true;
}

bool Deb::deployRawTemplate(PackageControl &pkg) {
    const DeployConfig *cfg = DeployCore::_config;

    QString defaultPackageTempalte = ":/Templates/DEB/Distributions/Templates/deb";

    auto list = pkg.availablePackages();
    for (auto it = list.begin();
         it != list.end(); ++it) {

        auto package = cfg->getDistroFromPackage(*it);
        auto local = location(package);

        TemplateInfo info;
        if (!collectInfo(package, info)) {
            return false;
        }

        QHash<QString, QString> replace = {
            {"default", info.Name}
        };

        if (!unpackDir(defaultPackageTempalte,
                       local, {}, {}, replace)) {
            return false;
        }

        if (!addLicense(local, info.Name)) {
            return false;
        }
    }

    return true;
}

bool Deb::addLicense(const QString &packageLocation, const QString &packageName) const {
    // The license file of the -license option (issue #456): the license
    // text is stored in the standard location of the Debian policy: the
    // usr/share/doc/<package>/copyright file of the package (the same way
    // the Debian packages of the system store the full license text).
    const QString license = QuasarAppUtils::Params::getArg("license");
    if (license.isEmpty()) {
        return true;
    }

    QFileInfo licenseInfo(license);
    if (!licenseInfo.isFile()) {
        qCritical() << QString("The license file is not found: %0 (issue #456).").arg(license);
        return false;
    }

    const QString licenseTarget = packageLocation + "/usr/share/doc/" + packageName
        + "/copyright";
    // The copyFile() helper of the iDistribution creates the parent
    // directories of the target file.
    if (!copyFile(license, licenseTarget, true)) {
        qCritical() << QString("Failed to copy the license file into the debian package: %0 (issue #456).")
                    .arg(licenseTarget);
        return false;
    }

    qInfo() << "The license was added into the debian package (issue #456): " + licenseTarget;

    return true;
}

bool Deb::removeTemplate() const {
    const DeployConfig *cfg = DeployCore::_config;

    registerOutFiles();
    return QDir(cfg->getTargetDir() + "/" + getLocation()).removeRecursively();
}

Envirement Deb::toolKitEnv() const {
    Envirement result;
    result.addEnv(QProcessEnvironment::systemEnvironment().value("PATH"));

    return result;
}

QProcessEnvironment Deb::processEnvirement() const {
    return QProcessEnvironment::systemEnvironment();
}

QList<SystemCommandData> Deb::runCmd() {
    QList<SystemCommandData> res;
    for (const auto& inout: std::as_const(inouts)) {
        res.push_back({"dpkg-deb", QStringList{"--build", "--verbose"} << inout.input << inout.output});
    }

    return res;
}

QStringList Deb::outPutFiles() const {
    QStringList result;
    for (const auto& inout: std::as_const(inouts)) {
        result.push_back(inout.output);
    }
    return result;
}

QString Deb::dataLocation(const DistroModule &module) const {
    return location(module) + module.installDirDEB() + "/" + releativeLocation(module);
}

QString Deb::location(const DistroModule &module) const {
    const DeployConfig* cfg = DeployCore::_config;

    auto name = getName(module);

    if (name.isEmpty())
        return cfg->getTargetDir() + "/" + getLocation() + "/" + module.key();

    return cfg->getTargetDir() + "/" + getLocation() + "/" + name;
}

QString Deb::releativeLocation(const DistroModule &module) const {

    if (!module.prefix().isEmpty())
        return module.prefix();

    auto name = getName(module);
    if (!name.isEmpty())
        return name;

    return module.key();
}
