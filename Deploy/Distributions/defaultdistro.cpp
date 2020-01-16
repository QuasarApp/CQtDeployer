#include "defaultdistro.h"

#include <deployconfig.h>
#include <deploycore.h>

DefaultDistro::DefaultDistro(FileManager *fileManager)
    :iDistribution(fileManager){

};


QString DefaultDistro::getConfig() const{
    return ":/Distro/Distributions/configures/C and C++.json";
}

bool DefaultDistro::deployTemplate() const {
    return true;
}

Envirement DefaultDistro::toolKitEnv() const {
    return {};
}

QString DefaultDistro::runCmd() {
    return "";
}

QStringList DefaultDistro::runArg() const {
    return {};
}

bool DefaultDistro::removeTemplate() const {
    return true;
}

QProcessEnvironment DefaultDistro::processEnvirement() const {
    return QProcessEnvironment::systemEnvironment();
}
