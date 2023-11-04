#include "deb.h"

#include <deployconfig.h>
#include <pathutils.h>
#include <packagecontrol.h>
#include <quasarapp.h>

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
        QHash<QString, QString> pakcagesTemplates;

        if (!customTemplate.isEmpty()) {
            QuasarAppUtils::Params::log("Using custom template for debian pacakge: " + customTemplate,
                                        QuasarAppUtils::Info);

            auto availablePacakages = QDir(customTemplate).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

            for (const auto& pkg: availablePacakages) {
                pakcagesTemplates.insert(pkg.fileName(), pkg.absoluteFilePath());
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

        if (!unpackDir(pakcagesTemplates.value(info.Name, defaultPackageTempalte),
                       local, info, {""}, replace)) {
            return false;
        }

        if (!QFile::setPermissions(local +  "/DEBIAN",  static_cast<QFile::Permission>(0x7775))) {
            QuasarAppUtils::Params::log("Failed to set permissions", QuasarAppUtils::Warning);
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
    }

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
