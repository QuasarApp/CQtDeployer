#include "ziparhive.h"

#include <deploycore.h>
#include <packagecontrol.h>
#include <pathutils.h>
#include <zipcompresser.h>
#include "deployconfig.h"
#include "quasarapp.h"


ZipArhive::ZipArhive(FileManager *fileManager)
    :iDistribution(fileManager) {
    setLocation("defaultZIPTemplate");
}

bool ZipArhive::deployTemplate(PackageControl &pkg) {

    // default template
    const DeployConfig *cfg = DeployCore::_config;

    ZipCompresser zipWorker;

    auto list = pkg.availablePackages();
    for (auto it = list.begin();
         it != list.end(); ++it) {

        auto package = cfg->getDistroFromPackage(*it);

        TemplateInfo info;
        if (!collectInfo(package, info)) {
            return false;
        }

        auto local = location(package);
        auto dataLoc = dataLocation(package);

        if (!pkg.movePackage(*it, dataLoc)) {
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

QString ZipArhive::dataLocation(const DistroModule &module) const {
    return location(module) + "/" + releativeLocation(module);
}

QString ZipArhive::location(const DistroModule &module) const {
    const DeployConfig *cfg = DeployCore::_config;

    return cfg->getTargetDir() + "/" + getLocation() + "/" + module.key();
}
