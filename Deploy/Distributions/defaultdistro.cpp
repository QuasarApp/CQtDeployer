#include "defaultdistro.h"

#include <deployconfig.h>
#include <deploycore.h>

DefaultDistro::DefaultDistro() {

}

QString DefaultDistro::getConfig() const{
    return ":/Distro/Distributions/configures/C and C++.json";
}

void DefaultDistro::deployDefaultTemplate() const {}

Envirement DefaultDistro::toolKitLocation() const {
    return {};
}

QString DefaultDistro::runCmd() const {
    return "";
}

QStringList DefaultDistro::runArg() const {
    return {};
}
