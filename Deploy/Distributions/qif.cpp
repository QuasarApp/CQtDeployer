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
    result.addEnv(AppPath);

//Installer
    result.addEnvRec(AppPath + "../../QIF/", 2);

    return result;
}

QString QIF::getConfig() const {
    return ":/Distro/Distributions/configures/QIF.json";
}

QString QIF::runCmd() {
    QString base = "binarycreator";

    if (binarycreator.isEmpty())
        binarycreator = findProcess(toolKitEnv().concatEnv(), base);

    return binarycreator;
}

bool QIF::deployTemplate() {
    auto customTemplate = QuasarAppUtils::Params::getStrArg("qif", "");
    const DeployConfig *cfg = DeployCore::_config;

    if (customTemplate.isEmpty()) {
        // default template

        int number = 0;
        for (auto it = cfg->prefixes().cbegin(); it != cfg->prefixes().cend(); ++it) {
            auto package = it.value();

            TemplateInfo info;
            info.Name = (it.key().isEmpty())? "Application" + QString::number(number) : PathUtils::stripPath(it.key());
            if (!package.name().isEmpty()) {
                info.Name = package.name();
            }

            auto location = cfg->getTargetDir() + "/" + getLocation() + "/packages/" + info.Name;

            info.Description = "This package contains the " + info.Name;
            if (!package.description().isEmpty())
                info.Description = package.description();

            info.Version = "1.0";
            if (!package.version().isEmpty())
                info.Version = package.version();

            info.ReleaseData = QDate::currentDate().toString("yyyy-MM-dd");
            if (!package.releaseData().isEmpty())
                info.ReleaseData = package.releaseData();

            info.Icon = "";
            if (!package.icon().isEmpty()) {
                info.Icon = package.icon();
                if (!copyFile(info.Icon, location + "/icons/")) {
                    return false;
                }
            }

            info.Publisher = "Company";
            if (!package.publisher().isEmpty())
                info.Publisher = package.publisher();

            QString cmdArray = "[";
            for (const auto &target :it.value().targets()) {
                auto info =  QFileInfo(target);
                if (info.suffix().compare("exe", ONLY_WIN_CASE_INSENSIATIVE) || info.suffix().isEmpty()) {
                    cmdArray += "\"" + info.fileName() + "\"";
                }
            }
            cmdArray += "]";

            info.Custom = {{"[\"array\", \"of\", \"cmds\"]", cmdArray},
                           {"$LOCAL_ICON", info.Name + "/icons/" + QFileInfo(info.Icon).fileName()}};


            if (!unpackDir(":/Templates/QIF/Distributions/Templates/qif/packages/default", location, info)) {
                return false;
            }

            if (!moveData(cfg->getTargetDir() + "/" + info.Name, location + "/data/")) {
                return false;
            }

            generalInfo = info;

        }

        auto configLocation = cfg->getTargetDir() + "/" + getLocation() + "/config/";

        if (!unpackDir(":/Templates/QIF/Distributions/Templates/qif/config/",
                       configLocation, generalInfo)) {
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

