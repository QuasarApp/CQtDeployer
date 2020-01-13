#include "qif.h"
#include "quasarapp.h"
#include "deploycore.h"
#include "deployconfig.h"

QIF::QIF()= default;

Envirement QIF::toolKitLocation() const {
// SNAP
    Envirement result;

    QString AppPath = QuasarAppUtils::Params::getStrArg("appPath", "");
    result.addEnv(AppPath);

//Installer
    result.addEnvRec(AppPath + "../../QIF/", 2);

    return result;
}

QString QIF::getConfig() const {
    return ":/Distro/Distributions/configures/QIF.json";
}

QString QIF::runCmd() const {
    return "binarycreator";
}

void QIF::deployTemplate() const {
    auto customTemplate = QuasarAppUtils::Params::getStrArg("qif", "");
    const DeployConfig *cfg = DeployCore::_config;

    if (customTemplate.isEmpty()) {
        // default template

        for (auto it = cfg->prefixes().cbegin(); it != cfg->prefixes().cend(); ++it) {
            auto location = cfg->getTargetDir() + "/" + it.key();

        }


    } else {

    }
}

QStringList QIF::runArg() const {
    return {};
}

