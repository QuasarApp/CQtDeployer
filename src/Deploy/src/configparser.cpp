//#
//# Copyright (C) 2018-2024 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#include "configparser.h"
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include "dependenciesscanner.h"
#include "deploycore.h"
#include "filemanager.h"
#include "packing.h"
#include "pathutils.h"
#include "pluginsparser.h"
#include "defines.h"
#include <cassert>
#include <qaglobalutils.h>
#include <Distributions/deb.h>
#include <Distributions/defaultdistro.h>
#include <Distributions/qif.h>
#include <Distributions/ziparhive.h>
#include <qaplatformutils.h>


/**
 * this function init packages of project
 * inputParamsList - list of parameters
 *                   patern : value;package
 * mainContainer - container for insert data, usually it is packages map.
 * seterFunc - this is method of item of mainConteiner for set value from inputParamsList
 * important : package in inputParamsList must be second.
 */

static QString defaultPackage = "";

template<typename Container, typename Adder>
bool parsePackagesPrivate(Container& mainContainer,
                          const QStringList &inputParamsList,
                          Adder adder) {

    for (const auto& str: inputParamsList) {
        auto paramsList = str.split(DeployCore::getSeparator(1));
        auto first = paramsList.value(0, "");
        if (paramsList.size() == 1)
            (valueLink(mainContainer, defaultPackage, DistroModule{defaultPackage}).*adder)(first);

        else {
            bool skipError = QuasarAppUtils::Params::isEndable("allowEmptyPackages");
            first = PathUtils::fullStripPath(first);
            if (!skipError && !mainContainer.contains(first)) {
                return false;
            }

            for (int i = 1; i < paramsList.size(); ++i) {
                (valueLink(mainContainer, first, DistroModule{first}).*adder)(paramsList[i]);
            }
        }
    }

    return true;
}

template <typename Adder>
void parseTargetPrivate(DeployConfig& conf,
                        const QStringList &inputParams,
                        Adder adder) {

    auto &cointainer = conf.targetsEdit();

    for (const auto &targetPair: inputParams) {
        auto pair = targetPair.split(DeployCore::getSeparator(1), splitbehavior);

        if (pair.size() == 1) {
            QuasarAppUtils::Params::log(QString("Set new default property for all tagets: " + pair.value(0)),
                                        QuasarAppUtils::Debug);
            for (auto& editableTarget: cointainer) {
                (editableTarget.*adder)(pair.value(0));
            }

            continue;
        }

        const auto targetsMap = conf.getTargetsListByFilter(pair.value(0));

        if (pair.value(0).isEmpty() || targetsMap.isEmpty()) {

            QuasarAppUtils::Params::log(QString("You try sets property for the not exits target."
                                                " target: %0").arg(pair.value(0)),
                                        QuasarAppUtils::Warning);

            continue;
        }

        for (const auto &target: targetsMap) {
            QuasarAppUtils::Params::log(QString("Set new property for %0 taget.").
                                        arg(pair.value(0), pair.value(1)),
                                        QuasarAppUtils::Debug);
            (target->*adder)(pair.value(1));
        }
    }
}

template <typename Enabler>
bool enableOptionFotTargetPrivate(DeployConfig& conf,
                        const QStringList &inputParams,
                        Enabler enabler) {

    for (const auto &iconPair: inputParams) {
        auto pair = iconPair.split(DeployCore::getSeparator(1), splitbehavior);

        if (pair.size() != 1) {
            QuasarAppUtils::Params::log(QString("Failed parese list of option values, This option support only single leve list. "
                                                " Example: use -Option val1,val2,val3 "),
                                        QuasarAppUtils::Error);
            return false;
        }

        const auto targetsMap = conf.getTargetsListByFilter(pair.value(0));

        if (!targetsMap.size()) {
            QuasarAppUtils::Params::log(QString("Not found any targets for the %0 selector").
                                        arg(pair.value(0)),
                                        QuasarAppUtils::Warning);
        }

        for (const auto &target: targetsMap) {
            QuasarAppUtils::Params::log(QString("Set new property for %0 taget.").
                                        arg(pair.value(0)),
                                        QuasarAppUtils::Debug);
            (target->*enabler)();
        }
    }

    return true;
}

bool ConfigParser::parseParams() {

    auto path = QuasarAppUtils::Params::getArg("confFile");
    bool createFile = !QFile::exists(path) &&
            QuasarAppUtils::Params::isEndable("confFile");

    if (path.isEmpty() &&
            QuasarAppUtils::Params::size() <= 0) {
        path = DEFAULT_COFIGURATION_FILE;
    }

    if (QFile::exists(path)) {
        if (!loadFromFile(path)) {
            QuasarAppUtils::Params::log("Failed to parse json file : " + path,
                                        QuasarAppUtils::Error);
            return false;
        }
    }

    auto distro = getDistribution();
    _packing->setDistribution(distro);

    switch (DeployCore::getMode()) {
    case RunMode::Info: {
        QuasarAppUtils::Params::log("Print info ...",
                                    QuasarAppUtils::Info);

        if (!parseInfoMode()) {
            QuasarAppUtils::Params::log("Show info is failed!",
                                        QuasarAppUtils::Error);
            return false;
        }
        break;
    }
    case RunMode::Clear: {
        QuasarAppUtils::Params::log("Clear ...",
                                    QuasarAppUtils::Info);

        if (!parseClearMode()) {
            QuasarAppUtils::Params::log("Clear failed!",
                                        QuasarAppUtils::Error);
            return false;
        }
        break;
    }

    case RunMode::Init: {
        QuasarAppUtils::Params::log("Init ...",
                                    QuasarAppUtils::Info);
        if (!parseInitMode()) {
            QuasarAppUtils::Params::log("init is failed!",
                                        QuasarAppUtils::Error);
            return false;
        }
        break;
    }

    case RunMode::Deploy: {
        QuasarAppUtils::Params::log("Deploy ...",
                                    QuasarAppUtils::Info);

        if (!parseDeployMode(true)) {
            QuasarAppUtils::Params::log("Deploy failed!",
                                        QuasarAppUtils::Error);
            return false;
        }
        break;
    }

    case RunMode::Template: {

        QuasarAppUtils::Params::log("Extract defaults Templates ...",
                                    QuasarAppUtils::Info);

        if (!parseDeployMode(false)) {
            QuasarAppUtils::Params::log("Extract defaults Templates is failed!",
                                        QuasarAppUtils::Error);
            return false;
        }

        break;
    }

    }

    DeployCore::_config = &_config;

    if (createFile && !createFromDeploy(path)) {
        QuasarAppUtils::Params::log("Failed to create a deploy config file in " + path,
                                    QuasarAppUtils::Error);
    }

    return true;
}

const DeployConfig *ConfigParser::config() const {
    return &_config;
}

// FIX ME. if package contains the path separators then package rewrite to RelativeLink of configFile location
QJsonValue ConfigParser::writeKeyArray(int separatorLvl, const QString &parameter,
                                       const QString &confFileDir) const {

    auto list = parameter.split(DeployCore::getSeparator(separatorLvl));

    if (DeployCore::isContainsArraySeparators(parameter)) {
        QJsonArray array;

        for (const auto &i: list) {
            array.push_back(writeKeyArray(separatorLvl + 1, i, confFileDir));
        }

        return array;
    }

    if (list.size() && list.first().isEmpty()) {
        return QJsonValue(true);
    }

    auto val = list.first();

    if (PathUtils::isPath(val)) {

        val = PathUtils::getRelativeLink(
                    QFileInfo(confFileDir).absoluteFilePath(),
                    QFileInfo(val).absoluteFilePath());

    }

    return val;

}

void ConfigParser::writeKey(const QString& key, QJsonObject& obj,
                            const QString& confFileDir) const {
    obj[key] = writeKeyArray(0, QuasarAppUtils::Params::getArg(key), confFileDir);
}

QString ConfigParser::readKeyArray(int separatorLvl, const QJsonArray &array,
                                   const QString& confFileDir) const {

    QStringList list;

    for (const QJsonValue &i : array) {

        if (i.isArray()) {
            list.push_back(readKeyArray(separatorLvl + 1, i.toArray(), confFileDir));
        } else {
            QString val = i.toString();

            if (i.type() == QJsonValue::Double) {
                val = QString::number(i.toDouble(0), 'f');
            }

            if (!val.isEmpty()) {
                if (PathUtils::isReleativePath(val)) {
                    list.push_back(QFileInfo(confFileDir + '/' + val).absoluteFilePath());
                } else {
                    list.push_back(val);
                }
            }
        }
    }

    return list.join(DeployCore::getSeparator(separatorLvl));
}

void ConfigParser::readKey(const QString& key, const QJsonObject& obj,
                           const QString& confFileDir) const {

    if (!QuasarAppUtils::Params::isEndable(key)) {
        auto type = obj[key].type();

        switch (type) {
        case QJsonValue::Array: {
            auto array = obj[key].toArray();
            QuasarAppUtils::Params::setArg(key, readKeyArray(0, array, confFileDir));
            break;
        }
        case QJsonValue::Double: {
            readString(key,
                       QString::number(obj[key].toDouble(0), 'f'),
                       confFileDir);

            break;
        }
        case QJsonValue::String: {
            readString(key,
                       obj[key].toString(),
                       confFileDir);
            break;
        }
        default: {
            auto value = obj[key].toBool(true);
            QuasarAppUtils::Params::setEnable(key, value);
            break;
        }
        }

    }
}

void ConfigParser::readString(const QString &key, const QString &val,
                              const QString& confFileDir) const
{
    if (PathUtils::isReleativePath(val)) {
        QuasarAppUtils::Params::setArg(key, QFileInfo(confFileDir + '/' + val).absoluteFilePath());
    } else {
        QuasarAppUtils::Params::setArg(key, val);
    }
}

bool ConfigParser::createFromDeploy(const QString& confFile) const {
    QJsonObject obj;

    auto info = QFileInfo(confFile);

    const auto keys = QuasarAppUtils::Params::getUserParamsMap().keys();
    for (const auto &key :keys) {
        if (key != "confFile")
            writeKey(key, obj, info.absolutePath());
    }

    if (!QFile::exists(info.absolutePath()) &&
            !QDir("/").mkpath(info.absolutePath())) {
        return false;
    }

    QJsonDocument doc(obj);

    QFile file(confFile);

    if (file.open(QIODevice::WriteOnly| QIODevice::Truncate)) {
        file.write(doc.toJson());
        file.close();

        return true;
    }

    return false;

}

bool ConfigParser::loadFromFile(const QString& confFile) {
    QFile file(confFile);
    QString confFilePath = QFileInfo(confFile).absolutePath();

    if (file.open(QIODevice::ReadOnly)) {
        auto doc = QJsonDocument::fromJson(file.readAll());

        if (!doc.isObject()) {
            return false;
        }

        auto obj = doc.object();
        const auto keys = obj.keys();
        for (const auto &key: keys) {
            readKey(key, obj, confFilePath);
        }

        file.close();

        return true;
    }

    return false;
}

bool ConfigParser::initDistroStruct() {

    if (!initPackages()) {
        return false;
    }

    auto &mainDistro = _config.packagesEdit();

#ifdef Q_OS_LINUX

    auto binOut = QuasarAppUtils::Params::getArg("binOut").
            split(DeployCore::getSeparator(0), splitbehavior);
    auto libOut = QuasarAppUtils::Params::getArg("libOut").
            split(DeployCore::getSeparator(0), splitbehavior);

#else
    auto binOut = QuasarAppUtils::Params::getArg("binOut").
            split(DeployCore::getSeparator(0), splitbehavior);
    auto libOut = QuasarAppUtils::Params::getArg("libOut").
            split(DeployCore::getSeparator(0), splitbehavior);
#endif

    auto qmlOut = QuasarAppUtils::Params::getArg("qmlOut").
            split(DeployCore::getSeparator(0), splitbehavior);
    auto trOut = QuasarAppUtils::Params::getArg("trOut").
            split(DeployCore::getSeparator(0), splitbehavior);
    auto pluginOut = QuasarAppUtils::Params::getArg("pluginOut").
            split(DeployCore::getSeparator(0), splitbehavior);
    auto recOut = QuasarAppUtils::Params::getArg("recOut").
            split(DeployCore::getSeparator(0), splitbehavior);
    auto extraDataOut = QuasarAppUtils::Params::getArg("extraDataOut").
            split(DeployCore::getSeparator(0), splitbehavior);

    auto name = QuasarAppUtils::Params::getArg("name").
            split(DeployCore::getSeparator(0), splitbehavior);
    auto description = QuasarAppUtils::Params::getArg("description").
            split(DeployCore::getSeparator(0), splitbehavior);
    auto deployVersion = QuasarAppUtils::Params::getArg("deployVersion").
            split(DeployCore::getSeparator(0), splitbehavior);
    auto releaseDate = QuasarAppUtils::Params::getArg("releaseDate").
            split(DeployCore::getSeparator(0), splitbehavior);
    auto publisher = QuasarAppUtils::Params::getArg("publisher").
            split(DeployCore::getSeparator(0), splitbehavior);

    auto homepage = QuasarAppUtils::Params::getArg("homePage").
            split(DeployCore::getSeparator(0), splitbehavior);

    auto prefix = QuasarAppUtils::Params::getArg("prefix").
            split(DeployCore::getSeparator(0), splitbehavior);

    auto extraData = QuasarAppUtils::Params::getArg("extraData").
            split(DeployCore::getSeparator(0), splitbehavior);

    auto trData = QuasarAppUtils::Params::getArg("tr").
            split(DeployCore::getSeparator(0), splitbehavior);

    auto installDirDeb = QuasarAppUtils::Params::getArg("installDirDeb").
            split(DeployCore::getSeparator(0), splitbehavior);

    auto debOut = QuasarAppUtils::Params::getArg("debOut").
            split(DeployCore::getSeparator(0), splitbehavior);

    auto zipOut = QuasarAppUtils::Params::getArg("zipOut").
            split(DeployCore::getSeparator(0), splitbehavior);

// init distro stucts for all targets
    if (binOut.size() && !parsePackagesPrivate(mainDistro, binOut, &DistroModule::setBinOutDir)) {
        packagesErrorLog("binOut");
        return false;
    }

    if (libOut.size() && !parsePackagesPrivate(mainDistro, libOut, &DistroModule::setLibOutDir)) {
        packagesErrorLog("libOut");
        return false;
    }

    if (qmlOut.size() && !parsePackagesPrivate(mainDistro, qmlOut, &DistroModule::setQmlOutDir)) {
        packagesErrorLog("qmlOut");
        return false;
    }

    if (trOut.size() && !parsePackagesPrivate(mainDistro, trOut, &DistroModule::setTrOutDir)) {
        packagesErrorLog("trOut");
        return false;
    }

    if (pluginOut.size() && !parsePackagesPrivate(mainDistro, pluginOut, &DistroModule::setPluginsOutDir)) {
        packagesErrorLog("pluginOut");
        return false;
    }

    if (recOut.size() && !parsePackagesPrivate(mainDistro, recOut, &DistroModule::setResOutDir)) {
        packagesErrorLog("recOut");
        return false;
    }

    if (extraDataOut.size() && !parsePackagesPrivate(mainDistro, extraDataOut, &DistroModule::setExtraDataOutDir)) {
        packagesErrorLog("extraDataOut");
        return false;
    }

    if (name.size() && !parsePackagesPrivate(mainDistro, name, &DistroModule::setName)) {
        packagesErrorLog("name");
        return false;
    }

    if (description.size() && !parsePackagesPrivate(mainDistro, description, &DistroModule::setDescription)) {
        packagesErrorLog("description");
        return false;
    }

    if (deployVersion.size() && !parsePackagesPrivate(mainDistro, deployVersion, &DistroModule::setVersion)) {
        packagesErrorLog("deployVersion");
        return false;
    }

    if (releaseDate.size() && !parsePackagesPrivate(mainDistro, releaseDate, &DistroModule::setReleaseData)) {
        packagesErrorLog("releaseDate");
        return false;
    }

    if (publisher.size() && !parsePackagesPrivate(mainDistro, publisher, &DistroModule::setPublisher)) {
        packagesErrorLog("Publisher");
        return false;
    }

    if (homepage.size() && !parsePackagesPrivate(mainDistro, homepage, &DistroModule::setHomePage)) {
        packagesErrorLog("homePage");
        return false;
    }

    if (prefix.size() && !parsePackagesPrivate(mainDistro, prefix, &DistroModule::setPrefix)) {
        packagesErrorLog("prefix");
        return false;
    }

    if (extraData.size() && !parsePackagesPrivate(mainDistro, extraData, &DistroModule::addExtraData)) {
        packagesErrorLog("extraData");
        return false;
    }

    if (trData.size() && !parsePackagesPrivate(mainDistro, trData, &DistroModule::addTranslation)) {
        packagesErrorLog("tr");
        return false;
    }

    if (installDirDeb.size() && !parsePackagesPrivate(mainDistro, installDirDeb, &DistroModule::setInstallDirDEB)) {
        packagesErrorLog("installDirDeb");
        return false;
    }

    if (debOut.size() && !parsePackagesPrivate(mainDistro, debOut, &DistroModule::setDebOut)) {
        packagesErrorLog("debOut");
        return false;
    }

    if (zipOut.size() && !parsePackagesPrivate(mainDistro, zipOut, &DistroModule::setZipOut)) {
        packagesErrorLog("zipOut");
        return false;
    }

    return true;
}

bool ConfigParser::initPackages() {
    defaultPackage = "Application";
    QSet<QString> configuredTargets;

    if (QuasarAppUtils::Params::isEndable("targetPackage")) {
        auto tar_packages_array = QuasarAppUtils::Params::getArg("targetPackage", "").
                split(DeployCore::getSeparator(0));


        for (auto& str: tar_packages_array) {
            auto paramsList = str.split(DeployCore::getSeparator(1));
            auto package = PathUtils::fullStripPath(paramsList.value(0, ""));

            for (int i = 1; i < paramsList.size(); ++i) {
                auto targetPattern = paramsList.value(i);
                auto list = _config.getTargetsListByFilter(targetPattern);

                if (!list.size()) {
                    auto warning = QString("You create the %0 package with the %1 pattern, "
                                           "but no matches were found for this pattern. ").
                            arg(package, targetPattern);
                    QuasarAppUtils::Params::log(warning, QuasarAppUtils::Warning);
                    continue;
                }

                for (auto it = list.begin(); it != list.end(); ++it) {
                    if (!configuredTargets.contains(it.key())) {
                        configuredTargets.insert(it.key());
                        it.value()->setPackage(package);
                    }
                }

                _config.packagesEdit().insert(package, DistroModule{package});
            }

            if (paramsList.size() < 2) {
                defaultPackage = package;
            }
        }

        QuasarAppUtils::Params::log(
                    "The default package is " + defaultPackage,
                     QuasarAppUtils::Info);
    }

    // init default packages
    bool fDefaultPackage = false;
    for (auto it = _config.targetsEdit().begin(); it != _config.targetsEdit().end(); ++it) {
        if (!configuredTargets.contains(it.key())) {
            configuredTargets.insert(it.key());
            it.value().setPackage(defaultPackage);
            fDefaultPackage = true;
        }
    }

    if (fDefaultPackage) {
        _config.packagesEdit().insert(defaultPackage, DistroModule{defaultPackage});
    }

    _config.setDefaultPackage(defaultPackage);

    return true;
}

bool ConfigParser::initQmlInput() {

    auto qmlDir = QuasarAppUtils::Params::getArg("qmlDir").
            split(DeployCore::getSeparator(0), splitbehavior);


// init distro stucts for all targets
    _config.deployQml = qmlDir.size();

    if (qmlDir.size() && !parsePackagesPrivate(_config.packagesEdit(), qmlDir, &DistroModule::addQmlInput)) {
        packagesErrorLog("qmlDir");
        return false;
    }

    return true;
}

void ConfigParser::packagesErrorLog(const QString &option) {
    QuasarAppUtils::Params::log(QString("Failed to set the %0 option, because you are trying to set it for an uninitialized package."
                                        " Use the 'targetPackage' flag to init this package. "
                                        "Or, if you want to configure an empty package, "
                                        "use the allowEmptyPackages option to disable this error message.").
                                arg(option),
                                QuasarAppUtils::Error);
}

void ConfigParser::initCustomPlatform() {
    const auto platforms = QuasarAppUtils::Params::getArg("platform").
            split(DeployCore::getSeparator(0), splitbehavior);

    Platform customPlatform = Platform::UnknownPlatform;

    for (const auto& platform: platforms) {
        customPlatform = customPlatform | DeployCore::getPlatformFromString(platform);
    }

    _config.setCustomPlatform(customPlatform);

}

bool ConfigParser::parseDeployMode(bool checkBin) {

    if (QuasarAppUtils::Params::isEndable("deploySystem-with-libc")) {
        QuasarAppUtils::Params::log("You are using a deprecated option \"deploySystem-with-libc\"."
                                    " In this version this option is no different from \"deploySystem\"."
                                    " Please use the deploySystem option.", QuasarAppUtils::Warning);
        QuasarAppUtils::Params::setEnable("deploySystem", true );
    }

    if (!checkSnapPermisions()) {
        return false;
    }

    setTargetDir();

    initCustomPlatform();

    auto bin = QuasarAppUtils::Params::getArg("bin").
            split(DeployCore::getSeparator(0), splitbehavior);

    if (bin.size() && !setTargets(bin)) {

        QuasarAppUtils::Params::log("Failed to set targets",
                                    QuasarAppUtils::Warning);
    }

    auto xData = QuasarAppUtils::Params::getArg("extraData").
            split(DeployCore::getSeparator(0), splitbehavior);


    if (checkBin && !(_config.targets().count() || xData.count())) {
        QuasarAppUtils::Params::log("Failed to initialize targets or extra data!",
                                    QuasarAppUtils::Error);

        QuasarAppUtils::Params::log("Use bin or extraData options and check input paths.",
                                    QuasarAppUtils::Info);
        return false;
    }

    _config.depchLimit = 0;

    if (QuasarAppUtils::Params::isEndable("recursiveDepth")) {
        bool ok;
        _config.depchLimit = QuasarAppUtils::Params::getArg("recursiveDepth").toInt(&ok);
        if (!ok) {
            _config.depchLimit = 0;
            QuasarAppUtils::Params::log("Failed to set the recursive depth. The argument of the recursiveDepth option is invalid!"
                                        " Using the default value 0",
                                        QuasarAppUtils::Warning);
        }
    }

    initIgnoreEnvList();
    initEnvirement();
    initIgnoreList();
    if (!initDistroStruct()) {
        return false;
    }

    if (!initExtraPath()) {
        return false;
    }
    initExtraNames();
    initPlugins();

    if (!initQmake()) {

        if (QuasarAppUtils::PlatformUtils::isSnap()) {
            QuasarAppUtils::Params::log("If you are using qmake from the system repository,"
                                        " then you must use the classic version of CQtDeployer instead of the snap version."
                                        " This is due to the fact that the snap version"
                                        " runs in an isolated container and has limited access"
                                        " to system utilities and the environment."
                                        " For get the classic version of cqtdeployer use the cqtdeployer installer"
                                        " https://github.com/QuasarApp/CQtDeployer/releases", QuasarAppUtils::Info);
        }

        return false;
    }

    QuasarAppUtils::Params::log(
        _config.qtDir.toString(),
        QuasarAppUtils::Debug);

    if (!initQmlInput()) {
        return false;
    }

    return true;
}

bool ConfigParser::parseInfoMode() {
    if ((QuasarAppUtils::Params::isEndable("v") ||
         QuasarAppUtils::Params::isEndable("version"))) {
        DeployCore::printVersion();
        return true;
    }

    DeployCore::help();

    return true;
}

bool ConfigParser::parseInitMode() {

    auto initLvl = QuasarAppUtils::Params::getArg("init");
    QString sourceUrl(":/Distro/Distributions/configures/Init.json");

    if (initLvl == "multi") {
        sourceUrl = ":/Distro/Distributions/configures/Init multiPackage configuration.json";
    } else if (initLvl == "single") {
        sourceUrl = ":/Distro/Distributions/configures/Init single configuration.json";
    }

    QFile configFile(DEFAULT_COFIGURATION_FILE);
    QFile source(sourceUrl);

    if (configFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {

        if (source.open(QIODevice::ReadOnly)) {
            configFile.write(source.readAll());
            source.close();
        }

        configFile.close();
    }

    return true;
}

bool ConfigParser::configureTargets() {
    const auto icons = QuasarAppUtils::Params::getArg("icon").
            split(DeployCore::getSeparator(0), splitbehavior);

    const auto runScripts = QuasarAppUtils::Params::getArg("runScript").
            split(DeployCore::getSeparator(0), splitbehavior);

    const auto disableRunScripts = QuasarAppUtils::Params::getArg("disableRunScript").
            split(DeployCore::getSeparator(0), splitbehavior);

    const auto disableShortcuts = QuasarAppUtils::Params::getArg("disableShortCut").
            split(DeployCore::getSeparator(0), splitbehavior);

    const auto extraDepends = QuasarAppUtils::Params::getArg("extraDepends").
                                  split(DeployCore::getSeparator(0), splitbehavior);

    if (icons.size()) {
        parseTargetPrivate(_config, icons, &TargetInfo::setIcon);
    }

    if (runScripts.size()) {
        parseTargetPrivate(_config, runScripts, &TargetInfo::setRunScript);
    }

    if (disableShortcuts.size() && !enableOptionFotTargetPrivate(_config, disableShortcuts, &TargetInfo::disableShortCut)) {
        packagesErrorLog("disableShortCut");
        return false;
    }

    if (disableRunScripts.size() && !enableOptionFotTargetPrivate(_config, disableRunScripts, &TargetInfo::disableRunScript)) {
        packagesErrorLog("disableRunScript");
        return false;
    }

    if (extraDepends.size()) {
        parseTargetPrivate(_config, extraDepends, &TargetInfo::addDepends);
    }

    return true;
}

bool ConfigParser::parseClearMode() {
    setTargetDir("./" + DISTRO_DIR);

    return true;
}

QString ConfigParser::getRPathFromTargets() {
    QString res;

    for (const auto &i: _config.targets()) {
        if (i.isValid()) {
            res += i.getRPath() + DeployCore::getEnvSeparator();
        }
    }

    return res;
}

void ConfigParser::setTargetDir(const QString &target) {

    if (QuasarAppUtils::Params::isEndable("targetDir")) {
        _config.setTargetDir(QFileInfo(QuasarAppUtils::Params::getArg("targetDir")).absoluteFilePath());
    } else if (target.size()) {
        _config.setTargetDir(QFileInfo(target).absoluteFilePath());
    } else {

        _config.setTargetDir(QFileInfo("./" + DISTRO_DIR).absoluteFilePath());
        QuasarAppUtils::Params::log("The targetDir option is not used."
                                    " CQtDeployer will use default target dir :" + _config.getTargetDir(),
                                    QuasarAppUtils::Info);
    }
}

bool ConfigParser::addTarget(const TargetData& target) {

    if (_config.customPlatform() == Platform::UnknownPlatform ||
            _config.customPlatform() & target.targetInfo.getPlatform()) {

        if (!_config.targetsEdit().contains(target.target)) {
            _config.targetsEdit().insert(target.target,  target.targetInfo);
            return true;
        }
    }

    return false;
}

bool ConfigParser::setTargets(const QStringList &value) {

    bool isfillList = false;

    for (const auto &i : value) {
        QFileInfo targetInfo = DeployCore::findItem(i);

        if (i.isEmpty())
            continue;

        if (targetInfo.isFile()) {

            if (addTarget(createTarget(targetInfo.absoluteFilePath())))
                isfillList = true;
        }
        else if (targetInfo.isDir()) {
            if (!setTargetsInDir(targetInfo.absoluteFilePath())) {
                QuasarAppUtils::Params::log(i + " does not contain executable binaries!",
                                            QuasarAppUtils::Debug);
                continue;
            }
            isfillList = true;

        } else {
            QuasarAppUtils::Params::log(i + " is not a path. Try search this file in system enviroment",
                                        QuasarAppUtils::Debug);

            auto file = QFileInfo(DeployCore::findProcess(QProcessEnvironment::systemEnvironment().value("PATH"), i));

            if (file.exists()) {

                if (addTarget(createTarget(file.absoluteFilePath())))
                    isfillList = true;

            } else {
                QuasarAppUtils::Params::log(targetInfo.absoluteFilePath() + " does not exist!",
                                            QuasarAppUtils::Debug);
            }
        }

        // Work with exits target

    }

    return isfillList;
}

bool ConfigParser::setTargetsRecursive(const QString &dir) {
    if (!setTargetsInDir(dir, true)) {
        QuasarAppUtils::Params::log("setTargetsInDir failed!",
                                     QuasarAppUtils::Debug);
        return false;
    }

    return true;
}

bool ConfigParser::setTargetsInDir(const QString &dir, bool recursive) {
    QDir d(dir);
    if (dir.isEmpty() || !d.exists()) {
        QuasarAppUtils::Params::log(dir + " dir not exits!",
                                    QuasarAppUtils::Debug);
        return false;
    }
    QuasarAppUtils::Params::log("setTargetsInDir check path: " + dir,
                                QuasarAppUtils::Debug);
    QFileInfoList list;

    if (recursive) {
        list = d.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden);
    } else {
        list = d.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden);
    }

    bool result = false;
    for (const auto &file : std::as_const(list)) {

        if (file.isDir()) {
            result |= setTargetsInDir(file.absoluteFilePath(), recursive);
            continue;
        }

        auto name = file.fileName();
        auto sufix = file.completeSuffix();

        if (sufix.isEmpty() ||  name.contains(".dll", Qt::CaseInsensitive) ||
                name.contains(".so", Qt::CaseInsensitive) || name.contains(".exe", Qt::CaseInsensitive)) {

            if (addTarget(createTarget(QDir::fromNativeSeparators(file.absoluteFilePath()))))
                result = true;

        }

    }

    return result;
}

TargetData ConfigParser::createTarget(const QString &target) {
    TargetInfo libinfo;
    auto key = target;
    if (_scaner->fillLibInfo(libinfo, key)) {
        return {libinfo.fullPath(), libinfo};
    }
    return {key, {}};
}

QHash<QString, TargetInfo>
ConfigParser::moveTarget(TargetInfo target, const QString& newLocation) {
    target.setPath(QFileInfo(newLocation).absolutePath());

    return {{newLocation, target}};

}

void ConfigParser::initIgnoreList()
{
    if (QuasarAppUtils::Params::isEndable("ignore")) {
        auto list = QuasarAppUtils::Params::getArg("ignore").
                split(DeployCore::getSeparator(0));

        for (const auto &i : list) {
            _config.ignoreList.addRule(IgnoreData(i));
        }

    }

    IgnoreData ruleUnix, ruleWin;
    Envirement envUnix, envWin;

    envUnix.addEnv(Envirement::recursiveInvairement("/lib", 3));
    envUnix.addEnv(Envirement::recursiveInvairement("/usr/lib", 3));

    if (QuasarAppUtils::PlatformUtils::isSnap()) {
        envUnix.addEnv(Envirement::recursiveInvairement(
            QuasarAppUtils::PlatformUtils::transportPathToSnapRoot("/lib"), 3));
        envUnix.addEnv(Envirement::recursiveInvairement(
            QuasarAppUtils::PlatformUtils::transportPathToSnapRoot("/usr/lib"), 3));
    }

    ruleUnix.prority = SystemLib;
    ruleUnix.platform = Unix;
    ruleUnix.enfirement = envUnix;


    auto addRuleUnix = [&ruleUnix](const QString & lib) {
        ruleUnix.label = lib;
        return ruleUnix;
    };

    _config.ignoreList.addRule(addRuleUnix("libc"));
    _config.ignoreList.addRule(addRuleUnix("ld-"));
    _config.ignoreList.addRule(addRuleUnix("libm"));
    _config.ignoreList.addRule(addRuleUnix("libz"));
    _config.ignoreList.addRule(addRuleUnix("librt"));
    _config.ignoreList.addRule(addRuleUnix("libnsl"));
    _config.ignoreList.addRule(addRuleUnix("libdl"));
    _config.ignoreList.addRule(addRuleUnix("libutil"));
    _config.ignoreList.addRule(addRuleUnix("libresolv"));
    _config.ignoreList.addRule(addRuleUnix("libBrokenLocale"));
    _config.ignoreList.addRule(addRuleUnix("libBrokenLocale"));
    _config.ignoreList.addRule(addRuleUnix("libSegFault"));
    _config.ignoreList.addRule(addRuleUnix("libanl"));
    _config.ignoreList.addRule(addRuleUnix("libcrypt.so"));
    _config.ignoreList.addRule(addRuleUnix("/gconv/"));
    _config.ignoreList.addRule(addRuleUnix("libnss"));

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    auto path = env.value("PATH");
    auto winPath = findWindowsPath(path);

    envWin.addEnv(Envirement::recursiveInvairement(winPath + "/System32", 2));
    envWin.addEnv(Envirement::recursiveInvairement(winPath + "/SysWOW64", 2));

    ruleWin.prority = SystemLib;
    ruleWin.platform = Win;
    ruleWin.enfirement = envWin;

    auto addRuleWin = [&ruleWin](const QString & lib) {
        ruleWin.label = lib;
        return ruleWin;
    };

    // win and core libs :  see https://en.wikipedia.org/wiki/Microsoft_Windows_library_files
    _config.ignoreList.addRule(addRuleWin("Hal.DLL"));
    _config.ignoreList.addRule(addRuleWin("NTDLL.DLL"));
    _config.ignoreList.addRule(addRuleWin("KERNEL32.DLL"));
    _config.ignoreList.addRule(addRuleWin("GDI32.DLL"));
    _config.ignoreList.addRule(addRuleWin("USER32.DLL"));
    _config.ignoreList.addRule(addRuleWin("COMCTL32.DLL"));
    _config.ignoreList.addRule(addRuleWin("COMDLG32.DLL"));
    _config.ignoreList.addRule(addRuleWin("WS2_32.DLL"));
    _config.ignoreList.addRule(addRuleWin("ADVAPI32.DLL"));
    _config.ignoreList.addRule(addRuleWin("NETAPI32.DLL"));
    _config.ignoreList.addRule(addRuleWin("OLE32.DLL"));
    _config.ignoreList.addRule(addRuleWin("SHSCRAP.DLL"));
    _config.ignoreList.addRule(addRuleWin("WINMM.DLL"));
    _config.ignoreList.addRule(addRuleWin("IMM32.DLL"));
    _config.ignoreList.addRule(addRuleWin("KernelBase.DLL"));
    _config.ignoreList.addRule(addRuleWin("dwmapi.DLL"));


}

void ConfigParser::initIgnoreEnvList() {
    QStringList ignoreEnvList;

    if (QuasarAppUtils::Params::isEndable("ignoreEnv")) {
        auto ignoreList = QuasarAppUtils::Params::getArg("ignoreEnv").
                split(DeployCore::getSeparator(0));


        for (const auto &i : ignoreList) {
            auto path = QFileInfo(i).absoluteFilePath();

            if (path.right(1) == "/" || path.right(1) == "\\") {
                path.remove(path.size() - 1, 1);
            }

            ignoreEnvList.append(path);
        }
    }

    // forbid pathes of the snap container
    if (QuasarAppUtils::PlatformUtils::isSnap()) {
        ignoreEnvList.push_back("/lib");
        ignoreEnvList.push_back("/usr/lib");
    }

    ignoreEnvList.push_back(_config.appDir);
    ignoreEnvList.push_back(_config.getTargetDir());

    if (QuasarAppUtils::Params::isEndable("noRecursiveiIgnoreEnv")) {
        _config.envirement.setIgnoreEnvList(ignoreEnvList);
    } else {
        _config.envirement.setIgnoreEnvListRecursive(ignoreEnvList, _config.depchLimit);
    }


}

QString ConfigParser::getPathFrmoQmakeLine(const QString &in) const {
    auto list = in.split(':');
    if (list.size() > 1) {
        list.removeAt(0);
        return QuasarAppUtils::PlatformUtils::transportPathToSnapRoot(
                    QFileInfo(list.join(':')).absoluteFilePath().remove('\r'));
    }

    return "";
}

bool ConfigParser::initQmakePrivate(const QString &qmake) {
    QFileInfo info(qmake);

    QuasarAppUtils::Params::log("initialize qmake for.  " + info.absoluteFilePath(),
                                QuasarAppUtils::Debug);

    QString basePath = info.absolutePath();

    // Invoke qmake executable only when qmake paths exclude snapRootFS path.
    // Because files in snapRootFS is not executable ...
    if (!qmake.contains(QuasarAppUtils::PlatformUtils::snapRootFS()) && setQmake(qmake)) {
        return true;
    }


    // try deploy qt using only read FS permisions

    // check debian qt structure
    if (DeployCore::isDebianQt(info.absoluteFilePath())) {
        // initialize qt that was installed from apt package manager on any debian based os
        QString neededPlatform = DeployCore::getPlatformLibPrefix(_config.getPlatformOfAll());
        int qtVersion = DeployCore::qtVersionToString(_config.isNeededQt());

        QString debianQtRoot = QString("/usr/lib/%0/qt%1").
                               arg(neededPlatform).arg(qtVersion);

        if (QuasarAppUtils::PlatformUtils::isSnap()) {
            debianQtRoot = QuasarAppUtils::PlatformUtils::snapRootFS() + debianQtRoot;
        }

        if (!setQtDir(debianQtRoot)) {
            QuasarAppUtils::Params::log("Failed to initialize Qt directories",
                                        QuasarAppUtils::Error);
            return false;
        }
        
        return true;
    }
    // initialize qt that was installed from qt installer or aqtinstall package manager
    QDir dir(basePath);

    if (!dir.cdUp()) {
        QuasarAppUtils::Params::log("Failed to initialize qt directories by qmake.",
                                    QuasarAppUtils::Error);
        return false;
    }

    // For snap package of cqtdeplyer it is normal behavior
    if (!QuasarAppUtils::PlatformUtils::isSnap()) {
        QuasarAppUtils::Params::log("Failed to execute the qmake process!"
                                    " Trying to initialize Qt directories from path: " + dir.absolutePath(),
                                    QuasarAppUtils::Warning);
    }

    if (!setQtDir(dir.absolutePath())){
        QuasarAppUtils::Params::log("Failed to initialize Qt directories",
                                    QuasarAppUtils::Error);
        return false;
    }

    return true;
}

bool ConfigParser::initQmake() {


    if (!_config.isNeededQt()) {
        QuasarAppUtils::Params::log("Deploy only C/C++ libraries because"
                                    " all target applications do not depend on the Qt libraries",
                                     QuasarAppUtils::Info);
        return true;
    }

    auto qmake = QuasarAppUtils::PlatformUtils::transportPathToSnapRoot(
        QuasarAppUtils::Params::getArg("qmake"));

    QFileInfo info(qmake);

    if (!info.isFile()) {

        QString qmakeFromRPath = DeployCore::findProcess(getRPathFromTargets(), "qmake");

        if (qmakeFromRPath.isEmpty()) {

            if (!QuasarAppUtils::Params::isEndable("noCheckPATH")) {
                auto env = QProcessEnvironment::systemEnvironment();
                auto proc = DeployCore::findProcess(env.value("PATH"), "qmake");
                if (proc.isEmpty()) {
                    QuasarAppUtils::Params::log("The deployment target requires Qt libraries,"
                                                " but initialize of Qt directories is failed."
                                                " Please use the qmake option to set a path to the qmake executable.",
                                                QuasarAppUtils::Error);
                    return false;
                }


                QuasarAppUtils::Params::log(QString("The qmake was found in the PATH variable. qmake : %0"
                                                    " If you want to disable search qmake executable in PATH variable,"
                                                    " use the noCheckPATH option").
                                            arg(proc),
                                            QuasarAppUtils::Info);


                return initQmakePrivate(proc);
            }

            QuasarAppUtils::Params::log("Your distribution required Qt libraries "
                                        "but qmake executable cannot be found in the 'qmake' option or RPATH. "
                                        "You are using the option noCheckPATH, "
                                        "please remove this option from your deploy command to search qmake in PATH",
                                         QuasarAppUtils::Error);

            return false;

        }


        QuasarAppUtils::Params::log(QString("The qmake was found in the RPATH variable. qmake : %0"
                                            " If you want to disable search qmake executable in RPATH variable,"
                                            " use the noCheckRPATH option").
                                    arg(qmakeFromRPath),
                                    QuasarAppUtils::Info);

        return initQmakePrivate(qmakeFromRPath);

    }

    return initQmakePrivate(qmake);
}

bool ConfigParser::setQmake(const QString &value) {

    auto qmakeInfo = QFileInfo(value);

    QuasarAppUtils::Params::log("sets qmake for.  " + qmakeInfo.absoluteFilePath(),
                                QuasarAppUtils::Debug);

    QProcess proc;
    proc.setProgram(qmakeInfo.absoluteFilePath());
    proc.setProcessEnvironment(QProcessEnvironment::systemEnvironment());
    proc.setArguments({"-query"});

    proc.start();

    if (!(proc.waitForStarted(1000) && proc.waitForFinished(1000))) {
        QuasarAppUtils::Params::log("Run qmake fail! " + proc.errorString(), QuasarAppUtils::Warning);
        return false;
    }

    QString qmakeData = proc.readAll();
    auto list = qmakeData.split('\n');

    for (const auto &value : list) {
        if (value.contains("QT_INSTALL_LIBS")) {
            _config.qtDir.setLibs(getPathFrmoQmakeLine(value));
        } else if (value.contains("QT_INSTALL_LIBEXECS")) {
            _config.qtDir.setLibexecs(getPathFrmoQmakeLine(value));
        } else if (value.contains("QT_INSTALL_BINS")) {
            _config.qtDir.setBins(getPathFrmoQmakeLine(value));
        } else if (value.contains("QT_INSTALL_PLUGINS")) {
            _config.qtDir.setPlugins(getPathFrmoQmakeLine(value));
        } else if (value.contains("QT_INSTALL_QML")) {
            _config.qtDir.setQmls(getPathFrmoQmakeLine(value));
        } else if (value.contains("QT_INSTALL_TRANSLATIONS")) {
            _config.qtDir.setTranslations(getPathFrmoQmakeLine(value));
        } else if (value.contains("QT_INSTALL_DATA")) {
            _config.qtDir.setResources(getPathFrmoQmakeLine(value) + "/resources");
        }
    }

    if (_config.qtDir.getLibs().isEmpty()) {
        QuasarAppUtils::Params::log("Wrong output from the qmake process. " + qmakeInfo.absoluteFilePath(),
                                    QuasarAppUtils::Warning);
        QuasarAppUtils::Params::log("Raw output:" + qmakeData,
                                    QuasarAppUtils::Debug);
        QuasarAppUtils::Params::log("Parsed Qt configuration: \n" + _config.qtDir.toString(),
                                    QuasarAppUtils::Debug);

        return false;
    }

    _config.envirement.addEnv(_config.qtDir.getLibs());
    _config.envirement.addEnv(_config.qtDir.getBins());

    return true;
}

bool ConfigParser::setQtDir(const QString &value) {

    QFileInfo info(value);

    QuasarAppUtils::Params::log("initialize qt dirs for. " + info.absoluteFilePath(),
                                QuasarAppUtils::Debug);

    if (DeployCore::isDebianQt(value)) {
        if (QFile::exists(info.absoluteFilePath() + ("/bin"))) {
            _config.qtDir.setBins(info.absoluteFilePath() + ("/bin"));
        }
    } else {
        if (!QFile::exists(info.absoluteFilePath() + ("/bin"))) {
            QuasarAppUtils::Params::log("get qt bin failed!", QuasarAppUtils::Debug);
            return false;
        }
        _config.qtDir.setBins(info.absoluteFilePath() + ("/bin"));
    }

    if (DeployCore::isDebianQt(value)) {
        _config.qtDir.setLibs(info.absolutePath());
    } else {
        if (!QFile::exists(info.absoluteFilePath() + ("/lib"))) {
            QuasarAppUtils::Params::log("get qt lib failed!", QuasarAppUtils::Debug);
            return false;
        }
        _config.qtDir.setLibs(info.absoluteFilePath() + ("/lib"));
    }


    if (!QFile::exists(info.absoluteFilePath() + ("/qml"))) {
        QuasarAppUtils::Params::log("get qt qml failed!", QuasarAppUtils::Debug);
    } else {
        _config.qtDir.setQmls(info.absoluteFilePath() + ("/qml"));
    }

    if (!QFile::exists(info.absoluteFilePath() + ("/plugins"))) {
        QuasarAppUtils::Params::log("get qt plugins failed!", QuasarAppUtils::Debug);
    } else {
        _config.qtDir.setPlugins(info.absoluteFilePath() + ("/plugins"));
    }

    if (_config.qtDir.getQtPlatform() & Unix) {
        if (!QFile::exists(info.absoluteFilePath() + ("/libexec"))) {
            QuasarAppUtils::Params::log("get qt libexec failed!", QuasarAppUtils::Debug);
        } else {
            _config.qtDir.setLibexecs(info.absoluteFilePath() + ("/libexec"));
        }
    } else if (_config.qtDir.getQtPlatform() & Win) {
        _config.qtDir.setLibexecs(info.absoluteFilePath() + ("/bin"));
    }

    if (DeployCore::isDebianQt(value)) {
        _config.qtDir.setTranslations(QString("/usr/share/qt%0/translations").
                                      arg(DeployCore::qtVersionToString(_config.isNeededQt())));
    } else {
        if (!QFile::exists(info.absoluteFilePath() + ("/translations"))) {
            QuasarAppUtils::Params::log("get qt translations failed!", QuasarAppUtils::Debug);
        } else {
            _config.qtDir.setTranslations(info.absoluteFilePath() + ("/translations"));
        }
    }

    if (!QFile::exists(info.absoluteFilePath() + ("/resources"))) {
        QuasarAppUtils::Params::log("get qt resources failed!", QuasarAppUtils::Debug);
    } else {
        _config.qtDir.setResources(info.absoluteFilePath() + ("/resources"));
    }

    _config.envirement.addEnv(_config.qtDir.getLibs());
    _config.envirement.addEnv(_config.qtDir.getBins());

    return true;
}

bool ConfigParser::initExtraPath() {
    auto listLibDir = QuasarAppUtils::Params::getArg("libDir").
            split(DeployCore::getSeparator(0));

    QDir dir;

    for (const auto &i : listLibDir) {
        QFileInfo info(QuasarAppUtils::PlatformUtils::transportPathToSnapRoot(i));
        if (info.isDir()) {
            if (_config.targets().contains(info.absoluteFilePath())) {
                QuasarAppUtils::Params::log("Skip the extra library path because it is target!",
                                            QuasarAppUtils::Debug);
                continue;
            }

            if (_config.envirement.isIgnore(info.absoluteFilePath())) {
                QuasarAppUtils::Params::log(QString("Failed to set libDir path!"
                                                    " The %0 path will be ignored because"
                                                    " this path is child path of the targetDir path"
                                                    " or manually added into ignore environment.").
                                            arg(info.absoluteFilePath()),
                                            QuasarAppUtils::Error);
                return false;
            }

            dir.setPath(info.absoluteFilePath());
            auto extraDirs = getSetDirsRecursive(QDir::fromNativeSeparators(info.absoluteFilePath()), _config.depchLimit);
            _config.extraPaths.addExtraPaths(extraDirs);

            _config.envirement.addEnv(Envirement::recursiveInvairement(dir, 0, _config.depchLimit));
        } else if (i.size() > 1) {

            _config.extraPaths.addExtraPathsMasks({i});

            QuasarAppUtils::Params::log(i + " is added as a path mask",
                                        QuasarAppUtils::Debug);
        } else {
            QuasarAppUtils::Params::log(i + " not added in path mask because"
                                            " the path mask must be large 2 characters",
                                        QuasarAppUtils::Debug);
        }
    }

    return true;
}

void ConfigParser::addExtraNamesMasks(const QStringList& listNamesMasks) {
    for (const auto &i : listNamesMasks) {
        if (i.size() > 1) {
            _config.allowedPaths.addtExtraNamesMasks({i});

            QuasarAppUtils::Params::log(i + " is added as a filename mask",
                                        QuasarAppUtils::Debug);
        } else {
            QuasarAppUtils::Params::log(i + " not added in file mask because"
                                            " the file mask must be large 2 characters",
                                        QuasarAppUtils::Debug);
        }
    }
}

void ConfigParser::initExtraNames() {

    auto listNamesMasks = QuasarAppUtils::Params::getArg("extraLibs").
            split(DeployCore::getSeparator(0));

    addExtraNamesMasks(listNamesMasks);

/*
 * Task https://github.com/QuasarApp/CQtDeployer/issues/422
 * We need to add to extra names libraries without which qt will not work,
 *
*/
    if (_config.isNeededQt()) {
        auto libs = DeployCore::Qt3rdpartyLibs( _config.getPlatformOfAll());
        addExtraNamesMasks(libs);
    }
}

bool ConfigParser::initPlugins() {

    auto listExtraPlugin = QuasarAppUtils::Params::getArg("extraPlugin").
            split(DeployCore::getSeparator(0), splitbehavior);

    auto listEnablePlugins = QuasarAppUtils::Params::getArg("enablePlugins").
            split(DeployCore::getSeparator(0), splitbehavior);

    auto listDisablePlugins = QuasarAppUtils::Params::getArg("disablePlugins").
            split(DeployCore::getSeparator(0), splitbehavior);


    if (listExtraPlugin.size() && !parsePackagesPrivate(_config.packagesEdit(),
                                                        listExtraPlugin,
                                                        &DistroModule::addExtraPlugins)) {
        packagesErrorLog("extra plugins");
        return false;
    }

    if (listEnablePlugins.size() && !parsePackagesPrivate(_config.packagesEdit(),
                                                          listEnablePlugins,
                                                          &DistroModule::addEnabledPlugins)) {
        packagesErrorLog("enable plugins");
        return false;
    }

    if (listDisablePlugins.size() && !parsePackagesPrivate(_config.packagesEdit(),
                                                           listDisablePlugins,
                                                           &DistroModule::addDisabledPlugins)) {
        packagesErrorLog("disable plugins");
        return false;
    }

    return true;
}

QString ConfigParser::findWindowsPath(const QString& path) const {
    auto list = path.split(';');
    QString win_magic = "windows";

    for (const auto &i: list ) {
        int index = i.indexOf(win_magic, 0, Qt::CaseInsensitive);
        if (index > 0 && i.size() == index + win_magic.size()) {
            return QDir::fromNativeSeparators(i);
        }
    }

    return "C:/" + win_magic;
}

QList<iDistribution *> ConfigParser::getDistribution() {
    QList<iDistribution *> distros;
    if (QuasarAppUtils::Params::isEndable("deb")) {
#ifdef Q_OS_LINUX
        distros.push_back(new Deb(_fileManager));
#else
        QuasarAppUtils::Params::log("The deb option availabel only on Linux host palatforms,"
                                    " please remove this options from your deploy command",
                                    QuasarAppUtils::Warning);

#endif
    }

    if (QuasarAppUtils::Params::isEndable("zip")) {
        distros.push_back(new ZipArhive(_fileManager));
    }

    if (QuasarAppUtils::Params::isEndable("qif")) {
        distros.push_back(new QIF(_fileManager));
    }

    if (distros.isEmpty()) {
        distros.push_back(new DefaultDistro(_fileManager));
    }

    return distros;
}

void ConfigParser::initEnvirement() {
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    auto path = env.value("PATH");

    if (!QuasarAppUtils::PlatformUtils::isSnap()) {

        _config.envirement.addEnv(env.value("LD_LIBRARY_PATH"));
        _config.envirement.addEnv(path);
    }

    QStringList dirs;
#ifdef Q_OS_LINUX

    dirs.append(getDirsRecursive(QuasarAppUtils::PlatformUtils::transportPathToSnapRoot("/lib"), 5));
    dirs.append(getDirsRecursive(QuasarAppUtils::PlatformUtils::transportPathToSnapRoot("/usr/lib"), 5));

#else
    auto winPath = findWindowsPath(path);
    dirs.append(getDirsRecursive(winPath + "/System32", 2));
    dirs.append(getDirsRecursive(winPath + "/SysWOW64", 2));

#endif

    _config.envirement.addEnv(dirs);

    if (_config.envirement.size() < 2) {
        QuasarAppUtils::Params::log("System environment is empty",
                                    QuasarAppUtils::Warning);
    }
}

bool ConfigParser::checkSnapPermisions() {

    if (!QuasarAppUtils::PlatformUtils::isSnap())
        return true;


    bool system = QuasarAppUtils::Params::isEndable("deploySystem") ||
            QuasarAppUtils::Params::isEndable("extraLibs");

    if (system && !QuasarAppUtils::PlatformUtils::checkSystemBakupSnapInterface()) {

        QuasarAppUtils::Params::log("You use a deploySystem or extraLibs options,"
                                    " but not added permision system-backup for cqtdeployer."
                                    " Please add permissions system-backup before using cqtdeployer."
                                    " Add system-backup permision from console: ",
                                    QuasarAppUtils::Error);

        QuasarAppUtils::Params::log(
                    "'snap connect cqtdeployer:system-backup :system-backup'",
                    QuasarAppUtils::Info);

        QuasarAppUtils::Params::log(
                    "GUI: Open the gnome system setting >> Applications >> CQtDeployer. "
                    "in menu rights and permisions enable system-backup.",
                    QuasarAppUtils::Info);

        return false;
    }

    return true;
}

QStringList ConfigParser::getDirsRecursive(const QString &path, int maxDepch, int depch) {
    return getSetDirsRecursive(path, maxDepch, depch).values();
}

QSet<QString> ConfigParser::getSetDirsRecursive(const QString &path, int maxDepch, int depch) {
    QDir dir(path);

    QSet<QString> res = {dir.path()};

    if (maxDepch >= 0 && maxDepch <= depch) {
        return res;
    }

    auto list = dir.entryInfoList(QDir::Dirs| QDir::NoDotAndDotDot | QDir::Hidden);

    for (const auto &subDir: list) {
        res.insert(subDir.absoluteFilePath());
        res.unite(getSetDirsRecursive(subDir.absoluteFilePath(), maxDepch, depch + 1));
    }

    return res;
}

bool ConfigParser::smartMoveTargets() {

    QMultiHash<QString, TargetInfo> temp;
    bool result = true;

    QuasarAppUtils::Params::log(QString("Available Targets: "),
                                QuasarAppUtils::Debug);


    for (auto i = _config.targets().cbegin(); i != _config.targets().cend(); ++i) {

        if (!i.value().isValid()) {

            QuasarAppUtils::Params::log(i.value().toString(),
                                        QuasarAppUtils::Error);

            internalError();
            return false;
        }

        QFileInfo target(i.key());

        QString targetPath = _config.getTargetDir() + "/" + i.value().getPackage();

        targetPath += _config.getDistro(i.key()).getBinOutDir();

        if (!_fileManager->cp(target.absoluteFilePath(), targetPath)) {
            result = false;
        }

        auto newTargetKey = targetPath + "/" + target.fileName();

        const auto newTarget = moveTarget(i.value(), newTargetKey);
        temp.unite(newTarget);
        QuasarAppUtils::Params::log(QString("Target: " + newTarget.begin().key()),
                                    QuasarAppUtils::Debug);

        auto pkgKey = i.value().getPackage();
        if (!_config.packagesEdit().contains(pkgKey)) {

            QuasarAppUtils::Params::log(QString("The target %0 belongs to package %1"
                                                " but this package is not initialized!").
                                        arg(i.key(), pkgKey));

            internalError();
            return false;
        }

        valueLink(_config.packagesEdit(), pkgKey,
                  DistroModule{pkgKey}).addTarget(newTargetKey);


    }

    _config.targetsEdit() = QHash<QString, TargetInfo>{temp.begin(), temp.end()};

    return result && configureTargets();
}

ConfigParser::ConfigParser(FileManager *filemanager, PluginsParser *pluginsParser, DependenciesScanner* scaner, Packing *pac):
    _fileManager(filemanager),
    _pluginsParser(pluginsParser),
    _scaner(scaner),
    _packing(pac) {

    assert(_fileManager);
    assert(_pluginsParser);
    assert(_scaner);
    assert(_packing);

    _config.appDir = QuasarAppUtils::Params::getCurrentExecutableDir();
    if (_config.appDir.right(4) == "/bin") {
        _config.appDir = _config.appDir.left(_config.appDir.size() - 4);
    }

    QuasarAppUtils::Params::log("appDir = " + _config.appDir);
}
