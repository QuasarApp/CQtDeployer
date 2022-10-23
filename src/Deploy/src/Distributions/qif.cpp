#include "qif.h"
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

    QString AppPath = QuasarAppUtils::Params::getCurrentExecutableDir();
    result.addEnv(AppPath);

    // SNAP
    result.addEnv(AppPath + "/../QIF/");

    //Installer nad zip
#ifdef Q_OS_LINUX
    result.addEnvRec(AppPath + "/../../QIF/", 2);
#endif

#ifdef Q_OS_WIN32
    result.addEnvRec(AppPath + "/../QIF/", 2);
#endif

    return result;
}

QList<SystemCommandData> QIF::runCmd() {
    SystemCommandData cmd;

    QString binarycreator = QuasarAppUtils::Params::getArg("binarycreator");

    if (binarycreator.isEmpty()) {
        binarycreator = DeployCore::findProcess(toolKitEnv().concatEnv(), "binarycreator");
        if (binarycreator.isEmpty()) {
            cmd.command = "binarycreator";
        } else {
            cmd.command = binarycreator;
        }
    } else {
        auto commandsList = binarycreator.split(' ');
        cmd.command = commandsList.first();
        cmd.arguments = commandsList.mid(1,-1);
    }

    auto location = DeployCore::_config->getTargetDir() + "/" + getLocation();

    cmd.arguments += QStringList{
        "-c",
        QuasarAppUtils::Params::getArg("qifConfig", location + "/config/config.xml"),
        "-p",
        QuasarAppUtils::Params::getArg("qifPackages", location + "/packages/")
    };

    QString resources = QuasarAppUtils::Params::getArg("qifResources");
    if (resources.size()) {
        cmd.arguments.push_back("-r");
        cmd.arguments.push_back(resources);
    }

    cmd.arguments.push_back(installerFile());

    QString customFormat = QuasarAppUtils::Params::getArg("qifArchiveFormat");
    if (customFormat.size()) {
        cmd.arguments.push_back("--af");
        cmd.arguments.push_back(customFormat);
    }

    cmd.arguments += "-v";

    return {cmd};
}

bool QIF::deployTemplate(PackageControl &pkg) {
    if (!initDefaultConfiguratuin()) {

        QuasarAppUtils::Params::log("Failed to init the default configuration of qif installer.",
                                    QuasarAppUtils::Error);
        return false;
    }

    auto customTemplate = QuasarAppUtils::Params::getArg("qif", "");
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

    auto qifStyle = getStyle(QuasarAppUtils::Params::getArg("qifStyle", ""));
    auto qifBanner = QuasarAppUtils::Params::getArg("qifBanner", "");
    auto qifLogo = QuasarAppUtils::Params::getArg("qifLogo", "");

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

bool QIF::deployRawTemplate(PackageControl &pkg) {
    const DeployConfig *cfg = DeployCore::_config;

    QString defaultPackageTempalte = ":/Templates/QIF/Distributions/Templates/qif/packages/default";
    QString defaultConfig = ":/Templates/QIF/Distributions/Templates/qif/config/";

    auto configLocation = cfg->getTargetDir() + "/" + getLocation() + "/config/";


    auto list = pkg.availablePackages();
    for (auto it = list.begin();
         it != list.end(); ++it) {

        auto package = cfg->getDistroFromPackage(*it);
        auto local = location(package);

        if (!unpackDir(defaultPackageTempalte,
                       local, {}, {})) {
            return false;
        }
    }

    if (!unpackDir(defaultConfig,
                   configLocation, {}, {})) {
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
            QuasarAppUtils::Params::log(QString("Use the %0 installer style").arg(style.baseName()),
                                        QuasarAppUtils::Info);
            return style.absoluteFilePath();
        }
    }

    QFileInfo f(input);

    if (f.isFile()) {
        QuasarAppUtils::Params::log("Use custom installer style",
                                    QuasarAppUtils::Info);
        return f.absoluteFilePath();
    }

    if (input.size()) {
        QuasarAppUtils::Params::log(QString("The %0 style is not exits").arg(input),
                                    QuasarAppUtils::Error);
    }

    QuasarAppUtils::Params::log("Use a default installer style",
                                QuasarAppUtils::Info);

    return "";
}

QString QIF::installerFile() const {
#ifdef Q_OS_LINUX
    QString sufix = ".run";
#else
    QString sufix = ".exe";
#endif
    QString qifOut = QuasarAppUtils::Params::getArg("qifOut");
    if (qifOut.size()) {
        return DeployCore::_config->getTargetDir() + "/" + qifOut;
    }

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
    return collectInfo(cfg->getDistroFromPackage(cfg->getDefaultPackage()), generalInfo);
}
