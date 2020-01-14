#include "qif.h"
#include "quasarapp.h"
#include "deploycore.h"
#include "deployconfig.h"

#include <QDateTime>

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

bool QIF::deployTemplate() const {
    auto customTemplate = QuasarAppUtils::Params::getStrArg("qif", "");
    const DeployConfig *cfg = DeployCore::_config;

    if (customTemplate.isEmpty()) {
        // default template

        for (auto it = cfg->prefixes().cbegin(); it != cfg->prefixes().cend(); ++it) {
            auto location = cfg->getTargetDir() + "/" + getLocation() + "/" + it.key();
            auto package = it.value();

            TemplateInfo info;
            info.Name = it.key();
            if (!package.name().isEmpty())
                info.Name = package.name();

            info.Description = "This package contains the " + info.Name;
            if (!package.description().isEmpty())
                info.Description = package.description();

            info.Version = "1.0";
            if (!package.version().isEmpty())
                info.Version = package.version();

            info.ReleaseData = QDate::currentDate().toString();
            if (!package.releaseData().isEmpty())
                info.ReleaseData = package.releaseData();

            info.Icon = "";
            if (!package.icon().isEmpty())
                info.Icon = package.icon();

            if (!unpackDir(":/Templates/QIF/Distributions/Templates/qif", location, info)) {
                return false;
            }

        }

        return true;
    }

    // custom template

    return true;
}

QStringList QIF::runArg() const {
    return {};
}

