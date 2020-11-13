#include "deb.h"

#include <deployconfig.h>
#include <pathutils.h>
#include <packagecontrol.h>
#include <quasarapp.h>

Deb::Deb(FileManager *fileManager):
    iDistribution(fileManager)
{
    setLocation("tmp DEB");
}

bool Deb::deployTemplate(PackageControl &pkg) {
    // default template
    const DeployConfig *cfg = DeployCore::_config;

    for (auto it = cfg->packages().begin();
         it != cfg->packages().end(); ++it) {
        auto package = it.value();

        QString defaultPackageTempalte = ":/Templates/DEB/Distributions/Templates/deb";
        auto customTemplate = QuasarAppUtils::Params::getStrArg("deb", "");
        QHash<QString, QString> pakcagesTemplates;

        if (!customTemplate.isEmpty()) {
            QuasarAppUtils::Params::log("Using custom template for installer: " + customTemplate,
                                        QuasarAppUtils::Info);

            auto availablePacakages = QDir(customTemplate).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

            for (const auto& pkg: availablePacakages) {
                pakcagesTemplates.insert(pkg.fileName(), pkg.absoluteFilePath());
            }
        }

        TemplateInfo info;
        bool fDefaultPakcage;
        if (!collectInfo(it, cfg, info, fDefaultPakcage)) {
            return false;
        }

        auto local = location(info.Name);
        auto localData = dataLocation(info.Name);

        if (!pkg.movePackage(it.key(), localData)) {
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
            QuasarAppUtils::Params::log("permishens set fail", QuasarAppUtils::Warning);
        }

        outFiles.push_back(info.Name + ".deb");
        packageFolders.push_back(local);
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
    for (const auto& dir: packageFolders) {
        res.push_back({"dpkg-deb", QStringList{"--build", "--verbose"} << dir});
    }

    return res;
}

QStringList Deb::outPutFiles() const {
    return outFiles;
}

bool Deb::cb() const {
    const DeployConfig* cfg = DeployCore::_config;

    QString from = cfg->getTargetDir() + "/" +  getLocation() + "/";
    QString to = cfg->getTargetDir() + "/" +  getLocation() + "/../";

    for (const QString& file : outPutFiles()) {
        if(!moveData(from + file, to, "")) {
            return false;
        }
    }

    return true;
}

QString Deb::dataLocation(const QString &packageName) const {
    return location(packageName) + "/opt/" + packageName;
}

QString Deb::location(const QString &packageName) const {
    const DeployConfig* cfg = DeployCore::_config;

    return cfg->getTargetDir() + "/" + getLocation() + "/" + packageName;
}
