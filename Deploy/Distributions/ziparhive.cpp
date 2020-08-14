#include "ziparhive.h"

#include <deploycore.h>
#include <pathutils.h>
#include <zipcompresser.h>
#include "deployconfig.h"
#include "quasarapp.h"


ZipArhive::ZipArhive(FileManager *fileManager)
    :iDistribution(fileManager) {
    setLocation("tmp zip");
}

bool ZipArhive::deployTemplate() {
    // default template
    const DeployConfig *cfg = DeployCore::_config;

    auto sortedMap = sortPackages(cfg->packages());

    ZipCompresser zipWorker;
    for (auto &it : sortedMap) {
        auto package = it.second;

        TemplateInfo info;
        info.Name = PathUtils::stripPath(it.first);

        if (info.Name.isEmpty()) {
            QFileInfo targetInfo(*package->targets().begin());
            info.Name = targetInfo.baseName();
        }

        if (!package->name().isEmpty()) {
            info.Name = package->name();
        }

        auto location = cfg->getTargetDir() + "/" + getLocation() + "/" +
                ((it.first.isEmpty())? "Application": info.Name);

        if (!moveData(cfg->getTargetDir() + "/" + it.first, location, getLocation())) {
            return false;
        }

        auto arr = cfg->getTargetDir() + info.Name + ".zip";
        zipWorker.compress(location, arr);

        outFiles.push_back(arr);
    }

    return true;
}

bool ZipArhive::removeTemplate() const {
    const DeployConfig *cfg = DeployCore::_config;

    registerOutFiles();
    return QDir(cfg->getTargetDir() + "/" + getLocation()).removeRecursively();

    return true;
}

Envirement ZipArhive::toolKitEnv() const {
    return {};
}

QProcessEnvironment ZipArhive::processEnvirement() const {
    return QProcessEnvironment::systemEnvironment();
}

QString ZipArhive::runCmd() {
    return "";
}

QStringList ZipArhive::runArg() const {
    return {};
}

QStringList ZipArhive::outPutFiles() const {
    return outFiles;
}

