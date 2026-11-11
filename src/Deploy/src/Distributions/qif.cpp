#include "qif.h"
#include "deploycore.h"
#include "deployconfig.h"

#include <QDebug>
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

    // Flat package layout: the QIF tools are bundled next to the
    // executable, e.g. cqt/CQtDeployer + cqt/QIF (issue #872).
    result.addEnv(AppPath + "/QIF/");

    //Installer nad zip
#ifdef Q_OS_LINUX
    result.addEnvRec(AppPath + "/../../QIF/", 2);
#endif

#ifdef Q_OS_WIN32
    result.addEnvRec(AppPath + "/../QIF/", 2);
#endif

    return result;
}

bool QIF::isAvailable() const {
    // An explicit binarycreator argument (qmake option or the
    // binarycreator parameter) always takes priority.
    if (!QuasarAppUtils::Params::getArg("binarycreator").isEmpty()) {
        return true;
    }

    // The "qifFromSystem" mode resolves the Qt Installer Framework from
    // the user's Qt installation at deploy time, when the deploy
    // configuration is available.
    if (QuasarAppUtils::Params::isEndable("qifFromSystem")) {
        return true;
    }

    auto binarycreator = DeployCore::findProcess(toolKitEnv().concatEnv(),
                                                 "binarycreator");

    return !binarycreator.isEmpty();
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

    // The code signing of the generated app bundle of the macOS hosts
    // (issue #865): the Mach-O rewrite of the deployed binaries
    // (issue #166) invalidates the original code signature, so the
    // installer bundle is signed with the identity of the qifSign
    // option. The Qt Installer Framework binarycreator signs the bundle
    // with the -s (signing identity) option, but its command line parser
    // rejects the identity values that start with the "-" character (the
    // "startsWith('-')" guard of the binarycreator main function of the
    // qtproject/installer-framework repository, present in all the
    // released versions including the latest): the ad-hoc codesign
    // identity "-" cannot be passed to the binarycreator, so the bundle
    // with the ad-hoc identity is signed by the cqtdeployer with the
    // codesign tool after the bundle is created - the same
    // "codesign --force --deep --sign <identity> <bundle>" command the
    // binarycreator runs internally (issue #865). The "-" identity value
    // creates the ad-hoc signature without the signing certificate.
    // The qifSign option is given in two forms (issue #865, #908): the
    // single qifSign option without the identity value ("cqtdeployer ...
    // qtifw qifSign") and the "-qifSign <identity>" option with the
    // value. The single option without the value creates the ad-hoc
    // signature: the "-" identity of the codesign tool (no signing
    // certificate required, the only signing form of the macOS CI hosts
    // without the Developer ID certificate). The command line parser
    // rejects the identity values with the leading "-" character (the
    // single "-" token), so the ad-hoc identity is not passed as the
    // option value (the team lead decision of the QuasarAppLib PR #68,
    // the issue #67 of the QuasarAppLib repository).
    QString signIdentity = QuasarAppUtils::Params::getArg("qifSign");
    if (QuasarAppUtils::Params::isEndable("qifSign") && signIdentity.isEmpty()) {
        // The single qifSign option without the identity value: the
        // ad-hoc signature (issue #908).
        signIdentity = QStringLiteral("-");
    }
#ifdef Q_OS_MACOS
    if (signIdentity.size() && !signIdentity.startsWith(QLatin1Char('-'))) {
        // The identity without the leading "-" character is passed to
        // the -s (signing identity) option of the binarycreator
        // (issue #865).
        cmd.arguments.push_back("-s");
        cmd.arguments.push_back(signIdentity);
    }
#else
    // The -s (signing identity) option of the binarycreator is available
    // on the macOS hosts only, the option is ignored on the other
    // platforms (issue #865).
    if (signIdentity.size()) {
        qWarning() << "The qifSign option is available on the macOS hosts only, "
                    "the option is ignored on the current platform (issue #865).";
    }
#endif

    cmd.arguments += "-v";

    QList<SystemCommandData> commands = {cmd};

#ifdef Q_OS_MACOS
    if (signIdentity.size() && signIdentity.startsWith(QLatin1Char('-'))) {
        // The identity with the leading "-" character (the ad-hoc "-"
        // value of the codesign tool) cannot be passed to the binarycreator
        // (its command line parser rejects it, issue #865), the bundle is
        // signed by the codesign step after the binarycreator.
        SystemCommandData signCmd;
        signCmd.command = QStringLiteral("codesign");
        signCmd.arguments = {
            QStringLiteral("--force"),
            QStringLiteral("--deep"),
            QStringLiteral("--sign"),
            signIdentity,
            installerFile()
        };
        commands.append(signCmd);
    }
#endif

    return commands;
}

bool QIF::deployTemplate(PackageControl &pkg) {
    if (!initDefaultConfiguratuin()) {

        qCritical() << "Failed to init the default configuration of qif installer.";
        return false;
    }

    // The 'qif' option was renamed to 'qtifw' (issue #738); the old name is
    // still accepted for backward compatibility.
    auto customTemplate = QuasarAppUtils::Params::getArg(
        "qtifw", QuasarAppUtils::Params::getArg("qif", ""));
    const DeployConfig *cfg = DeployCore::_config;

    QStringList sufixes = {
        "js", "qs", "xml"
    };

    QString defaultPackageTempalte = ":/Templates/QIF/Distributions/Templates/qif/packages/default";
    QString defaultConfigCustomDesigne =  ":/Templates/QIF/Distributions/Templates/qif/config custom designe/";
    QString defaultConfig = ":/Templates/QIF/Distributions/Templates/qif/config/";
    QHash<QString, QString> pakcagesTemplates;

    if (!customTemplate.isEmpty()) {
        qInfo() << "Using custom template for installer: " + customTemplate;

        auto availablePacakages = QDir(customTemplate + "/packages").entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden);

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

    if (!addLicense(pkg)) {
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

    if (!addLicense(pkg)) {
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
    auto list = resurces.entryInfoList(QDir::Files | QDir::Hidden);
    for (const auto& style : list) {
        if (input == style.baseName()) {
            qInfo() << QString("Use the %0 installer style").arg(style.baseName());
            return style.absoluteFilePath();
        }
    }

    QFileInfo f(input);

    if (f.isFile()) {
        qInfo() << "Use custom installer style";
        return f.absoluteFilePath();
    }

    if (input.size()) {
        qCritical() << QString("The %0 style is not exits").arg(input);
    }

    qInfo() << "Use a default installer style";

    return "";
}

bool QIF::addLicense(PackageControl &pkg) const {
    // The license file of the -license option (issue #456): the installer
    // shows the license as an agreement before the installation starts.
    const QString license = QuasarAppUtils::Params::getArg("license");
    if (license.isEmpty()) {
        return true;
    }

    QFileInfo licenseInfo(license);
    if (!licenseInfo.isFile()) {
        qCritical() << QString("The license file is not found: %0 (issue #456).").arg(license);
        return false;
    }

    const DeployConfig *cfg = DeployCore::_config;

    // The Qt Installer Framework (issue #456) reads the license agreement
    // from the meta directory of the packages: the file value of the
    // License element of the Licenses block of the package.xml is the path
    // relative to the meta directory of the package. The config.xml of the
    // installer does not support the license agreement elements in the
    // Qt Installer Framework versions 4.x: the binarycreator tool rejects
    // the unknown elements of the config.xml (the Settings parser of the
    // Qt Installer Framework runs in the strict mode).
    bool added = false;
    const auto list = pkg.availablePackages();
    for (const auto &package : list) {

        const QString metaDir = location(cfg->getDistroFromPackage(package)) + "/meta/";
        const QString packageXml = metaDir + "package.xml";
        if (!QFile::exists(packageXml)) {
            qWarning() << QString("The package.xml of the package is not found, "
                                  "the license is not added into the package: %0 (issue #456).")
                                .arg(packageXml);
            continue;
        }

        // The license file is copied into the meta directory of the
        // package with the original file name (issue #456): the file value
        // of the License element of the package.xml is the path relative
        // to the meta directory of the Qt Installer Framework.
        const QString licenseTarget = metaDir + licenseInfo.fileName();
        if (!copyFile(license, licenseTarget, true)) {
            qCritical() << QString("Failed to copy the license file into the meta "
                                   "directory of the package: %0 (issue #456).").arg(licenseTarget);
            return false;
        }

        QFile file(packageXml);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qCritical() << QString("Failed to open the package file of the installer: %0 (issue #456).").arg(packageXml);
            return false;
        }

        QString xml = QString::fromUtf8(file.readAll());
        file.close();

        if (xml.contains("<Licenses>")) {
            // The custom package template already contains the license
            // configuration: the license file is copied into the meta
            // directory of the package only (issue #456).
            qInfo() << "The package.xml of the installer already contains the "
                       "Licenses element, the license file is copied only (issue #456): " + packageXml;
            added = true;
            continue;
        }

        // The Licenses element is inserted into the package.xml before the
        // end tag of the Package element (issue #456): the name value of
        // the License element is the title of the license agreement (the
        // base name of the license file) and the file value is the license
        // file of the meta directory of the package.
        const QString licensesBlock =
            "    <Licenses>\n"
            "        <License name=\"" + licenseInfo.completeBaseName()
            + "\" file=\"" + licenseInfo.fileName() + "\" />\n"
            "    </Licenses>\n";

        const int endTag = xml.lastIndexOf("</Package>");
        if (endTag < 0) {
            qCritical() << QString("The end tag of the Package element is not found "
                                   "in the package file: %0 (issue #456).").arg(packageXml);
            return false;
        }

        xml.insert(endTag, licensesBlock);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
            qCritical() << QString("Failed to open the package file of the installer for writing: %0 (issue #456).").arg(packageXml);
            return false;
        }

        file.write(xml.toUtf8());
        file.close();

        qInfo() << "The license agreement was added into the package.xml of the "
                   "installer (issue #456): " + packageXml;
        added = true;
    }

    if (!added) {
        qWarning() << "The license file is not added into the installer: "
                     "no package.xml of the packages of the installer template was found (issue #456).";
    }

    return true;
}

QString QIF::installerFile() const {
#ifdef Q_OS_LINUX
    QString sufix = ".run";
#else
    QString sufix = ".exe";
#endif
    QString name;
    // The name of the installer is taken from the qifOut value of the
    // deployment configuration. The name must match the platform: the
    // binarycreator of the Windows hosts appends the ".exe" suffix to
    // the output name when it is missing, so the platform-specific
    // suffix is part of the qifOut value (the INSTALLER_FULL_NAME
    // naming of the self-deploy config).
    const QString qifOut = QuasarAppUtils::Params::getArg("qifOut");
    if (qifOut.size()) {
        name = qifOut;
    } else {
        name = "Installer" + generalInfo.Name + sufix;
    }
#ifdef Q_OS_MACOS
    // The Qt Installer Framework binarycreator creates the installer on
    // the macOS hosts as an .app bundle and expects the output name to end
    // with the ".app" suffix: a name without the suffix (for example
    // "Installer.exe") leaves an empty "<name>.app" directory in the target
    // directory without the installer content (observed on the macOS CI
    // hosts, issue #738).
    if (!name.endsWith(QStringLiteral(".app"), Qt::CaseInsensitive)) {
        name += QStringLiteral(".app");
    }
#endif
    return DeployCore::_config->getTargetDir() + "/" + name;
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
