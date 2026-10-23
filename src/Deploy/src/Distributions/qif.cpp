#include "qif.h"
#include "deploycore.h"
#include "deployconfig.h"

#include <QDebug>
#include <QDateTime>
#include <QFileInfo>
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

    cmd.arguments += "-v";

#ifdef Q_OS_MACOS
    // The binarycreator creates the installer on the macOS hosts as an
    // .app bundle (a directory, issue #738), but the output of the qtifw
    // option must be a single file with the ".run" extension that can be
    // executed like the Qt installer (issue #884): the second command
    // packs the bundle into a self-extracting .run file - a POSIX shell
    // header followed by the gzip-compressed tar archive of the bundle -
    // and removes the bundle afterwards (the same pattern as the .run
    // files of the Qt installer on the Linux hosts).
    const QString bundle = QFileInfo(installerFile()).fileName();
    const QString run = QFileInfo(runFile()).fileName();
    const QString base = bundle.endsWith(QStringLiteral(".app"), Qt::CaseInsensitive)
        ? bundle.chopped(QStringLiteral(".app").size())
        : bundle;

    // The shell header of the .run file: it unpacks the embedded archive
    // into a temporary directory, starts the installer bundle and removes
    // the temporary files when the installer exits. "@CQT_RUN_DATA_LINE@"
    // is replaced with the line number where the archive data starts
    // (the line after the last header line).
    QString header = QStringLiteral(
        "#!/bin/sh\n"
        "# @CQT_RUN_NAME@ - CQtDeployer Qt Installer Framework installer (macOS).\n"
        "# This self-extracting archive unpacks the installer application\n"
        "# bundle into a temporary directory, starts it and removes the\n"
        "# temporary files when the installer exits.\n"
        "tmpDir=\"$(mktemp -d \"${TMPDIR:-/tmp}/CQtDeployer.XXXXXX\")\"\n"
        "trap 'rm -rf \"$tmpDir\"' EXIT INT TERM\n"
        "tail -n +@CQT_RUN_DATA_LINE@ \"$0\" | tar -xzf - -C \"$tmpDir\" || exit 1\n"
        "if command -v open >/dev/null 2>&1; then\n"
        "    open -W \"$tmpDir/@CQT_BUNDLE_NAME@\"\n"
        "    exit $?\n"
        "fi\n"
        "\"$tmpDir/@CQT_BUNDLE_NAME@/Contents/MacOS/@CQT_RUN_BASE_NAME@\"\n"
        "exit $?\n"
    );
    const int dataLine = header.count('\n') + 1;
    header.replace(QStringLiteral("@CQT_RUN_NAME@"), run);
    header.replace(QStringLiteral("@CQT_RUN_DATA_LINE@"), QString::number(dataLine));
    header.replace(QStringLiteral("@CQT_BUNDLE_NAME@"), bundle);
    header.replace(QStringLiteral("@CQT_RUN_BASE_NAME@"), base);

    // The packing command: the header (a quoted heredoc, so the content
    // is written verbatim) + the tar archive of the bundle are written to
    // the .run file, the file gets the executable permission and the
    // bundle is removed (it is embedded into the .run file now).
    QString script = QStringLiteral(
        "set -e\n"
        "cd \"@CQT_TARGET_DIR@\"\n"
        "{\n"
        "cat <<'CQTDEPLOYER_RUN_HEADER'\n"
    );
    script += header;
    script += QStringLiteral(
        "CQTDEPLOYER_RUN_HEADER\n"
        "tar -czf - \"@CQT_BUNDLE_NAME@\"\n"
        "} > \"@CQT_RUN_NAME@\"\n"
        "chmod +x \"@CQT_RUN_NAME@\"\n"
        "rm -rf \"@CQT_BUNDLE_NAME@\"\n"
    );
    script.replace(QStringLiteral("@CQT_TARGET_DIR@"), DeployCore::_config->getTargetDir());
    script.replace(QStringLiteral("@CQT_BUNDLE_NAME@"), bundle);
    script.replace(QStringLiteral("@CQT_RUN_NAME@"), run);

    SystemCommandData wrap;
    wrap.command = QStringLiteral("/bin/sh");
    wrap.arguments << QStringLiteral("-c") << script;

    return {cmd, wrap};
#else
    return {cmd};
#endif
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
    // On the macOS hosts the final artifact is the self-extracting .run
    // file (the .app bundle of the binarycreator is packed into it,
    // issue #884); on the other platforms the installer file itself is
    // the final artifact.
    return {runFile()};
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

QString QIF::installerFile() const {
#ifdef Q_OS_LINUX
    QString sufix = ".run";
#elif defined(Q_OS_MACOS)
    // On the macOS hosts the final artifact is the self-extracting .run
    // file (see runFile()): the .app bundle of the binarycreator carries
    // the .run name (issue #884).
    QString sufix = ".run";
#else
    QString sufix = ".exe";
#endif
    QString name;
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

QString QIF::runFile() const {
#ifdef Q_OS_MACOS
    // The output of the qtifw option must be a single file with the ".run"
    // extension that can be executed like the Qt installer (issue #884):
    // on the macOS hosts the .app bundle of the binarycreator (installerFile)
    // is packed into such a self-extracting .run file.
    QString name = QFileInfo(installerFile()).fileName();
    if (name.endsWith(QStringLiteral(".app"), Qt::CaseInsensitive)) {
        name.chop(QStringLiteral(".app").size());
    }
    if (!name.endsWith(QStringLiteral(".run"), Qt::CaseInsensitive)) {
        name += QStringLiteral(".run");
    }
    return DeployCore::_config->getTargetDir() + "/" + name;
#else
    return installerFile();
#endif
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
