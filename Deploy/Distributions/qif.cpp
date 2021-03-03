#include "qif.h"
#include "quasarapp.h"
#include "deploycore.h"
#include "deployconfig.h"

#include <QDateTime>
#include <QProcess>
#include <deploycore.h>
#include <packagecontrol.h>
#include <pathutils.h>

QIF::QIF(FileManager *fileManager)
    :iDistribution(fileManager){

    setLocation("defaultQIFWTemplate");

};

Envirement QIF::toolKitEnv() const {
    Envirement result;

    if (QuasarAppUtils::Params::isEndable("qifFromSystem")) {

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

        result.addEnv(QProcessEnvironment::systemEnvironment().value("PATH"));


        return result;
    }



    // SNAP

    QString AppPath = QuasarAppUtils::Params::getStrArg("appPath", "");
    result.addEnv(AppPath + "/../QIF/");

    //Installer
    result.addEnvRec(AppPath + "/../../QIF/", 2);

    return result;
}

QList<SystemCommandData> QIF::runCmd() {
    QString base = "binarycreator";

    SystemCommandData cmd;

    if (binarycreator.isEmpty())
        binarycreator = DeployCore::findProcess(toolKitEnv().concatEnv(), base);

    if (binarycreator.isEmpty()) {
        cmd.command = base;
    } else {
        cmd.command = binarycreator;
    }

    auto location = DeployCore::_config->getTargetDir() + "/" + getLocation();

    cmd.arguments = QStringList{
        "-c",
        location + "/config/config.xml",
        "-p",
        location + "/packages/",
        "-v",
        installerFile()
    };

    return {cmd};
}

bool QIF::deployTemplate(PackageControl &pkg) {
    if (!initDefaultConfiguratuin()) {

        QuasarAppUtils::Params::log("Fail to init rhe default configuration of the qif installer.",
                                    QuasarAppUtils::Error);
        return false;
    }

    auto customTemplate = QuasarAppUtils::Params::getStrArg("qif", "");
    const DeployConfig *cfg = DeployCore::_config;

    QStringList sufixes = {
        "js", "qs", "xml"
    };

    QString defaultPackageTempalte = ":/Templates/QIF/Distributions/Templates/qif/packages/default";
    QString defaultConfigCustomDesigne =  ":/Templates/QIF/Distributions/Templates/qif/config custom designe/";
    QString defaultConfig = ":/Templates/QIF/Distributions/Templates/qif/config/";
    QHash<QString, QString> pakcagesTemplates;

    if (!customTemplate.isEmpty()) {
        QuasarAppUtils::Params::log("Using custom template for installer: " + customTemplate,
                                    QuasarAppUtils::Info);

        auto availablePacakages = QDir(customTemplate + "/packages").entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

        for (const auto& pkg: availablePacakages) {
            pakcagesTemplates.insert(pkg.fileName(), pkg.absoluteFilePath());
        }

        defaultConfigCustomDesigne =  customTemplate + "/config";
        defaultConfig = customTemplate + "/config";
    }

    auto list = pkg.availablePackages();
    for (auto it = list.begin();
         it != list.end(); ++it) {

        if (!deployPackage(cfg->getDistroFromPackage(*it), sufixes, pakcagesTemplates, defaultPackageTempalte, pkg)) {
            return false;
        }
    }

    auto configLocation = cfg->getTargetDir() + "/" + getLocation() + "/config/";

    auto qifStyle = getStyle(QuasarAppUtils::Params::getStrArg("qifStyle", ""));
    auto qifBanner = QuasarAppUtils::Params::getStrArg("qifBanner", "");
    auto qifLogo = QuasarAppUtils::Params::getStrArg("qifLogo", "");

    auto configTemplate = defaultConfig;
    if (qifStyle.size() || qifBanner.size() || qifLogo.size()) {
        configTemplate = defaultConfigCustomDesigne;
    }

    if (!unpackDir(configTemplate,
                   configLocation, generalInfo, sufixes)) {
        return false;
    }

    if (qifStyle.size() && !copyFile(qifStyle, configLocation + "/style.css", true)) {
        return false;
    }

    if (qifBanner.size() && !copyFile(qifBanner, configLocation + "/banner.png", true)) {
        return false;
    }

    if (qifLogo.size() && !copyFile(qifLogo, configLocation + "/logo.png", true)) {
        return false;
    }

    return true;
}

bool QIF::removeTemplate() const {
    const DeployConfig *cfg = DeployCore::_config;

    registerOutFiles();
    return QDir(cfg->getTargetDir() + "/" + getLocation()).removeRecursively();

}

QProcessEnvironment QIF::processEnvirement() const {
    return QProcessEnvironment::systemEnvironment();
}

QStringList QIF::outPutFiles() const {
    return {installerFile()};
}

QString QIF::dataLocation(const DistroModule &module) const {
    return location(module) + "/data/" + releativeLocation(module);
}

QString QIF::location(const DistroModule &module) const {

    const DeployConfig *cfg = DeployCore::_config;
    return cfg->getTargetDir() + "/" + getLocation() + "/packages/" + module.key();
}

QString QIF::getStyle(const QString& input) const {
    QDir resurces(":/Styles/Distributions/Templates/qif/Styles");
    auto list = resurces.entryInfoList(QDir::Files);
    for (const auto& style : list) {
        if (input == style.baseName()) {
            return style.absoluteFilePath();
        }
    }

    QFileInfo f(input);

    if (f.isFile()) {
        return f.absoluteFilePath();
    }

    QuasarAppUtils::Params::log(input +  " not exits",
                                QuasarAppUtils::Error);

    return "";
}

QString QIF::installerFile() const {
#ifdef Q_OS_LINUX
    QString sufix = ".run";
#else
    QString sufix = ".exe";
#endif

    return DeployCore::_config->getTargetDir() + "/Installer" + generalInfo.Name + sufix;
}

bool QIF::deployPackage(const DistroModule& dist,
                        const QStringList sufixes,
                        const QHash<QString, QString>& pakcagesTemplates,
                        const QString& defaultPackageTempalte,
                        PackageControl &pkg) {

    auto package = dist;

    TemplateInfo info;
    if (!collectInfoWithDeployIcons(package, info)) {
        return false;
    }

    auto localData = dataLocation(package);
    auto local = location(package);

    if (!unpackDir(pakcagesTemplates.value(package.key(), defaultPackageTempalte),
                   local, info, sufixes)) {
        return false;
    }

    if (!pkg.movePackage(dist.key(), localData)) {
        return false;
    }

    if (package.isDefaultModule())
        generalInfo = info;

    return true;
}

bool QIF::initDefaultConfiguratuin() {
    const DeployConfig *cfg = DeployCore::_config;

    // init default configuration
    return collectInfo(DistroModule{cfg->getDefaultPackage()}, generalInfo);
}

