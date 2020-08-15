#include "defaultdistro.h"

#include <deployconfig.h>
#include <deploycore.h>

DefaultDistro::DefaultDistro(FileManager *fileManager)
    :iDistribution(fileManager){

};

bool DefaultDistro::deployTemplate(PackageControl &pkg) {
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

QStringList DefaultDistro::outPutFiles() const {
    return {};
}
