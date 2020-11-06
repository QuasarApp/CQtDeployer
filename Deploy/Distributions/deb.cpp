#include "deb.h"

#include <deployconfig.h>
#include <pathutils.h>
#include <packagecontrol.h>
#include <QProcess>

Deb::Deb(FileManager *fileManager):
    iDistribution(fileManager)
{
    setLocation("tmp DEB");
}

bool Deb::deployTemplate(PackageControl &pkg) {
    // default template
    const DeployConfig *cfg = DeployCore::_config;

    for (auto it = cfg->packages().begin();
         it != cfg->packages().end(); ++it) {
        auto package = it.value();

        TemplateInfo info;

        if (!collectInfo(it, cfg, info)) {
            return false;
        };

        auto local = location(info.Name);

        if (!pkg.movePackage(it.key(), local)) {
            return false;
        }

        if (!unpackDir("qrc:/Templates/DEB/Distributions/Templates/deb",
                       local, info, {""})) {
            return false;
        }
    }

    return true;
}

bool Deb::removeTemplate() const {

}

Envirement Deb::toolKitEnv() const {
    Envirement result;
    result.addEnv(QProcessEnvironment::systemEnvironment().value("PATH"));

    return result;
}

QProcessEnvironment Deb::processEnvirement() const {
    return QProcessEnvironment::systemEnvironment();
}

QString Deb::runCmd() {
    return "dpkg-deb";
}

QStringList Deb::runArg() const {
    return {"--build"};
}

QStringList Deb::outPutFiles() const {

}

QString Deb::dataLocation(const QString &packageName) const {
    return location(packageName);
}

QString Deb::location(const QString &packageName) const {
    const DeployConfig* cfg = DeployCore::_config;

    return cfg->getTargetDir() + "/" + getLocation() + "/" + packageName;
}
