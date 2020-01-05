#include "qif.h"
#include "quasarapp.h"

QIF::QIF(){}

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

    if (customTemplate.size()) {

    } else {

    }
}

QStringList QIF::runArg() const {
    return {};
}

