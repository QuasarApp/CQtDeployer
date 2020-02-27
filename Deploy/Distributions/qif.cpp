#include "qif.h"
#include "quasarapp.h"
#include "deploycore.h"
#include "deployconfig.h"

#include <QDateTime>
#include <QProcess>
#include <pathutils.h>

QIF::QIF(FileManager *fileManager)
    :iDistribution(fileManager){

};

Envirement QIF::toolKitEnv() const {
    Envirement result;
    result.addEnv(QProcessEnvironment::systemEnvironment().value("PATH"));

// BASE
    const DeployConfig *cfg = DeployCore::_config;
    auto basePATH = cfg->qtDir.getBins() + "/../../../Tools/QtInstallerFramework/";
    QDir QifDir(basePATH);
    auto list = QifDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    QMap<double, QString> sortedItems;
    for (const auto& i : list) {
        sortedItems.insert(i.toDouble(), i);
    }

    if (sortedItems.size()) {
        basePATH += ("/" + sortedItems.last() + "/bin");
        result.addEnv(basePATH);
    }


// SNAP

    QString AppPath = QuasarAppUtils::Params::getStrArg("appPath", "");
    result.addEnv(AppPath + "/../QIF/");

//Installer
    result.addEnvRec(AppPath + "/../../QIF/", 2);

    return result;
}

QString QIF::getConfig() const {
    return ":/Distro/Distributions/configures/QIF.json";
}

QString QIF::runCmd() {
    QString base = "binarycreator";

    if (binarycreator.isEmpty())
        binarycreator = DeployCore::findProcess(toolKitEnv().concatEnv(), base);

    if (binarycreator.isEmpty())
        return base;

    return binarycreator;
}

bool QIF::deployTemplate() {
    auto customTemplate = QuasarAppUtils::Params::getStrArg("qif", "");
    const DeployConfig *cfg = DeployCore::_config;

    QStringList sufixes = {
        "js", "qs", "xml"
    };

    if (customTemplate.isEmpty()) {
        // default template

        auto sortedMap = sortPackages(cfg->packages());

        for (auto &it : sortedMap) {
            auto package = it.second;

            TemplateInfo info;
            info.Name = PathUtils::stripPath(it.first);

            if (!package->name().isEmpty()) {
                info.Name = package->name();
            }

            auto location = cfg->getTargetDir() + "/" + getLocation() + "/packages/" +
                    ((it.first.isEmpty())? "Application": info.Name);

            auto locationData = location + "/data/" + info.Name;

            info.Description = "This package contains the " + info.Name;
            if (!package->description().isEmpty())
                info.Description = package->description();

            info.Version = "1.0";
            if (!package->version().isEmpty())
                info.Version = package->version();

            info.ReleaseData = QDate::currentDate().toString("yyyy-MM-dd");
            if (!package->releaseData().isEmpty())
                info.ReleaseData = package->releaseData();

            info.Icon = "icons/Icon.png";
            if (package->icon().isEmpty()) {
                if (!copyFile(":/Templates/QIF/Distributions/Templates/qif/Icon.png",
                              locationData + "/icons/")) {
                    return false;
                }
            } else {
                QFileInfo iconInfo(package->icon());
                info.Icon = info.Name + "/icons/" + iconInfo.fileName();
                if (!copyFile(package->icon(), locationData + "/icons/")) {
                    return false;
                }
            }

            info.Publisher = "Company";
            if (!package->publisher().isEmpty())
                info.Publisher = package->publisher();

            QString cmdArray = "[";
            for (const auto &target :it.second->targets()) {
                auto fileinfo =  QFileInfo(target);
                if (fileinfo.suffix().compare("exe", ONLY_WIN_CASE_INSENSIATIVE) || fileinfo.suffix().isEmpty()) {
                    cmdArray += "\"" + info.Name + "/" + fileinfo.fileName() + "\"";
                }
            }
            cmdArray += "]";

            info.Custom = {{"[\"array\", \"of\", \"cmds\"]", cmdArray},
                           {"$LOCAL_ICON", info.Name + "/icons/" + QFileInfo(info.Icon).fileName()}};


            if (info.Name.isEmpty()) {
                info.Name = "Application";
            }

            if (!unpackDir(":/Templates/QIF/Distributions/Templates/qif/packages/default",
                           location, info, sufixes)) {
                return false;
            }

            if (!moveData(cfg->getTargetDir() + "/" + it.first, locationData, getLocation())) {
                return false;
            }

            generalInfo = info;

        }

        auto configLocation = cfg->getTargetDir() + "/" + getLocation() + "/config/";

        if (!unpackDir(":/Templates/QIF/Distributions/Templates/qif/config/",
                       configLocation, generalInfo, sufixes)) {
            return false;
        }


        return true;
    }

    // custom template

    return true;
}

QStringList QIF::runArg() const {

    auto location = DeployCore::_config->getTargetDir() + "/" + getLocation();
#ifdef Q_OS_LINUX
    QString sufix = ".run";
#else
    QString sufix = ".exe";
#endif

    return {
        "-c", location + "/config/config.xml",
        "-p", location + "/packages/",
        "-v",
        DeployCore::_config->getTargetDir() + "/" + generalInfo.Name + sufix
    };
}

bool QIF::removeTemplate() const {
    auto customTemplate = QuasarAppUtils::Params::getStrArg("qif", "");
    const DeployConfig *cfg = DeployCore::_config;

    if (customTemplate.isEmpty()) {
        return QDir(cfg->getTargetDir() + "/" + getLocation()).removeRecursively();
    }

    return true;
}

QProcessEnvironment QIF::processEnvirement() const {
    return QProcessEnvironment::systemEnvironment();
}

