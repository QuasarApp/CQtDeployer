#include "defaultdistro.h"

#include <deployconfig.h>
#include <deploycore.h>

DefaultDistro::DefaultDistro() = default;

QString DefaultDistro::getConfig() const{
    return ":/Distro/Distributions/configures/C and C++.json";
}

void DefaultDistro::deployTemplate() const {}

Envirement DefaultDistro::toolKitLocation() const {
    return {};
}

QString DefaultDistro::runCmd() const {
    return "";
}

QStringList DefaultDistro::runArg() const {
    return {};
}
