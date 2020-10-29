#include "deb.h"

#include <deployconfig.h>
#include <pathutils.h>
#include <packagecontrol.h>

Deb::Deb(FileManager *fileManager):
    iDistribution(fileManager)
{

}

bool Deb::deployTemplate(PackageControl &pkg) {
    // default template
    const DeployConfig *cfg = DeployCore::_config;

    for (auto it = cfg->packages().begin();
         it != cfg->packages().end(); ++it) {
        auto package = it.value();

        TemplateInfo info;
        info.Name = PathUtils::stripPath(it.key());
        bool fDefaultPakcage = cfg->getDefaultPackage() == info.Name;

        if (fDefaultPakcage) {
            QFileInfo targetInfo(*package.targets().begin());
            info.Name = targetInfo.baseName();
        }

        if (!package.name().isEmpty()) {
            info.Name = package.name();
        }

        auto location = cfg->getTargetDir() + "/" + getLocation() + "/" + info.Name;

        if (!pkg.movePackage(it.key(), location)) {
            return false;
        }

        auto arr = cfg->getTargetDir() + "/" + info.Name + ".zip";
        if (!zipWorker.compress(location, arr)) {
                return false;
        }

        outFiles.push_back(arr);
    }

    return true;
}

bool Deb::removeTemplate() const {

}

Envirement Deb::toolKitEnv() const {

}

QProcessEnvironment Deb::processEnvirement() const {

}

QString Deb::runCmd() {

}

QStringList Deb::runArg() const {

}

QStringList Deb::outPutFiles() const {

}
