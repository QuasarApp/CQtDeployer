#include "defaultdistro.h"

#include <deployconfig.h>
#include <deploycore.h>
#include <packagecontrol.h>
#include <pathutils.h>

DefaultDistro::DefaultDistro(FileManager *fileManager)
    :iDistribution(fileManager){

};

bool DefaultDistro::deployTemplate(PackageControl & ctrl) {
    // default template
    const DeployConfig *cfg = DeployCore::_config;

    auto list = ctrl.availablePackages();

    for (auto it = list.begin();
         it != list.end(); ++it) {
        auto package = cfg->getDistroFromPackage(*it);

        QString targetLocation;
        if (package.isDefaultModule()) {
            targetLocation = cfg->getTargetDir();
        } else {
            targetLocation = cfg->getTargetDir() + "/" + releativeLocation(package);
        }

        if (!ctrl.copyPackage(*it, targetLocation)) {
            return false;
        }
    }

    return true;
}

Envirement DefaultDistro::toolKitEnv() const {
    return {};
}

QList<SystemCommandData> DefaultDistro::runCmd() {
    return {};
}

bool DefaultDistro::removeTemplate() const {
    return true;
}

QProcessEnvironment DefaultDistro::processEnvirement() const {
    return QProcessEnvironment::systemEnvironment();
}

QStringList DefaultDistro::outPutFiles() const {
    return {};
}

QString DefaultDistro::dataLocation(const DistroModule &) const {
    return "";
}

QString DefaultDistro::location(const DistroModule &) const {
    return "";
}

QString DefaultDistro::releativeLocation(const DistroModule &module) const {

    if (module.prefix().isEmpty()) {
        return module.key();
    }

    return module.prefix();
}
