#include "ziparhive.h"

#include <deploycore.h>
#include <packagecontrol.h>
#include <pathutils.h>
#include <zipcompresser.h>
#include "deployconfig.h"
#include "quasarapp.h"


ZipArhive::ZipArhive(FileManager *fileManager)
    :iDistribution(fileManager) {
    setLocation("tmp zip");
}

bool ZipArhive::deployTemplate(PackageControl &pkg) {
    // default template
    const DeployConfig *cfg = DeployCore::_config;

    ZipCompresser zipWorker;
    for (auto it = cfg->packages().begin();
         it != cfg->packages().end(); ++it) {
        auto package = it.value();

        TemplateInfo info;

        if (!collectInfo(it, cfg, info)) {
            return false;
        }

        auto local = location(info.Name);

        if (!pkg.movePackage(it.key(), local)) {
            return false;
        }

        auto arr = cfg->getTargetDir() + "/" + info.Name + ".zip";
        if (!zipWorker.compress(local, arr)) {
                return false;
        }

        outFiles.push_back(arr);
    }

    return true;
}

bool ZipArhive::removeTemplate() const {
    const DeployConfig *cfg = DeployCore::_config;

    registerOutFiles();
    return QDir(cfg->getTargetDir() + "/" + getLocation()).removeRecursively();

}

Envirement ZipArhive::toolKitEnv() const {
    return {};
}

QProcessEnvironment ZipArhive::processEnvirement() const {
    return QProcessEnvironment::systemEnvironment();
}

QList<SystemCommandData> ZipArhive::runCmd() {
    return {};
}

QStringList ZipArhive::outPutFiles() const {
    return outFiles;
}

QString ZipArhive::dataLocation(const QString &packageName) const {
    return location(packageName);
}

QString ZipArhive::location(const QString &packageName) const {
    const DeployConfig *cfg = DeployCore::_config;

    return cfg->getTargetDir() + "/" + getLocation() + "/" + packageName;
}
