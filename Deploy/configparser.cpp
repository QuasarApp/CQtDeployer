//#
//# Copyright (C) 2018-2020 QuasarApp.
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
#include "pathutils.h"
#include "quasarapp.h"

#include <assert.h>

/**
 * this function init prefixes of project
 * inputParamsList - list of parameters
 *                   patern : value;prefix
 * mainContainer - container for insert data, usually it is prefixes map.
 * seterFunc - this is method of item of mainConteiner for set value from inputParamsList
 * important : prefix in inputParamsList must be second.
 */
template<typename Container, typename Setter>
bool parsePrefixesPrivate(Container& mainContainer,
                          const QStringList &inputParamsList,
                          Setter setter) {

    for (auto& str: inputParamsList) {
        auto pair = str.split(DeployCore::getSeparator(1));
        auto first = pair.value(0, "");
        auto second = pair.value(1, "");
        if (pair.size() == 1)
            (mainContainer[""].*setter)(first);
        else {
            if (!mainContainer.contains(first)) {
                return false;
            }

            (mainContainer[first].*setter)(second);

        }

    }

    return true;
}

bool ConfigParser::parseParams() {

    auto path = QuasarAppUtils::Params::getStrArg("confFile");
    bool createFile = !(path.isEmpty() || QFile::exists(path));
    if (path.isEmpty()) {
        path = QFileInfo("./").absoluteFilePath();
    }
    loadFromFile(path);

    switch (DeployCore::getMode()) {
    case RunMode::Info: {
        qInfo() << "selected info mode" ;

        if (!parseQtInfoMode()) {
            qCritical() << "info mode fail!";
            return false;
        }

        DeployCore::_config = &_config;

        break;
    }
    case RunMode::Clear: {
        qInfo() << "selected clear mode" ;

        if (!parseQtClearMode()) {
            qCritical() << "clear mode fail!";
            return false;
        }

        DeployCore::_config = &_config;

        break;
    }

    case RunMode::Deploy: {
        qInfo() << "selected deploy mode" ;

        if (!parseQtDeployMode()) {
            qCritical() << "deploy mode fail!";
            return false;
        }

        DeployCore::_config = &_config;

        break;
    }

    }

    if (createFile && !createFromDeploy(path)) {
        QuasarAppUtils::Params::verboseLog("Do not create a deploy config file in " + path,
                                           QuasarAppUtils::Error);
    }

    return true;
}

const DeployConfig *ConfigParser::config() const {
    return &_config;
}

QJsonValue ConfigParser::writeKeyArray(int separatorLvl, const QString &parameter,
                                 const QString &confFileDir) const {

    auto list = parameter.split(DeployCore::getSeparator(separatorLvl));

    if (list.size() > 1) {
        QJsonArray array;

        for (auto &i: list) {
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
    if (QuasarAppUtils::Params::isEndable(key)) {
        obj[key] = writeKeyArray(0, QuasarAppUtils::Params::getStrArg(key), confFileDir);
    }
}

QString ConfigParser::readKeyArray(int separatorLvl, const QJsonArray &array,
                                       const QString& confFileDir) const {

    QStringList list;

    for (QJsonValue i : array) {

        if (i.isArray()) {
            list.push_back(readKeyArray(separatorLvl + 1, i.toArray(), confFileDir));
        } else {
            QString val = i.toString();

            if (!val.isEmpty()) {

                if (PathUtils::isPath(val)) {
                    QString path;

                    if (PathUtils::isAbsalutPath(val)) {
                        path = QFileInfo(val).absoluteFilePath();
                    } else {
                        path = QFileInfo(confFileDir + '/' + val).absoluteFilePath();
                    }

                    list.push_back(path);

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

         if (obj[key].isArray()) {
             auto array = obj[key].toArray();
             QuasarAppUtils::Params::setArg(key, readKeyArray(0, array, confFileDir));

         } else if (!obj[key].isUndefined()) {
             QString val = obj[key].toString();
             if (!val.isEmpty()) {

                 if (PathUtils::isPath(val)) {

                     if (PathUtils::isAbsalutPath(val)) {
                         val = QFileInfo(val).absoluteFilePath();
                     } else {
                         val = QFileInfo(confFileDir + '/' + val).absoluteFilePath();
                     }
                 }

                 QuasarAppUtils::Params::setArg(key, val);
             } else {
                 auto value = obj[key].toBool(true);
                 QuasarAppUtils::Params::setEnable(key, value);
             }
         }

    }
}

bool ConfigParser::createFromDeploy(const QString& confFile) const {
    QJsonObject obj;

    auto info = QFileInfo(confFile);

    for (auto &key :DeployCore::helpKeys()) {
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

        for (auto &key: obj.keys()) {
            readKey(key, obj, confFilePath);
        }

        file.close();

        return true;
    }

    return false;
}

bool ConfigParser::initDistroStruct() {

    if (!initPrefixes()) {
        return false;
    }

    auto &mainDistro = _config.prefixes;

#ifdef Q_OS_LINUX

    auto binOut = QuasarAppUtils::Params::getStrArg("binOut", "/bin").
            split(DeployCore::getSeparator(0));
    auto libOut = QuasarAppUtils::Params::getStrArg("libOut", "/lib").
            split(DeployCore::getSeparator(0));

#else
    auto binOut = QuasarAppUtils::Params::getStrArg("binOut", "/").
            split(DeployCore::getSeparator(0));
    auto libOut = QuasarAppUtils::Params::getStrArg("libOut", "/").
            split(DeployCore::getSeparator(0));
#endif

    auto qmlOut = QuasarAppUtils::Params::getStrArg("qmlOut", "/qml").
            split(DeployCore::getSeparator(0));
    auto trOut = QuasarAppUtils::Params::getStrArg("trOut", "/translations").
            split(DeployCore::getSeparator(0));
    auto pluginOut = QuasarAppUtils::Params::getStrArg("pluginOut", "/plugins").
            split(DeployCore::getSeparator(0));
    auto recOut = QuasarAppUtils::Params::getStrArg("recOut", "/resources").
            split(DeployCore::getSeparator(0));

    auto erroLog = [](const QString &flag){
            QuasarAppUtils::Params::verboseLog(QString("Set %0 fail, becouse you try set %0 for not inited prefix."
                                               " Use 'targetPrefix' flag for init the prefixes").arg(flag),
                                               QuasarAppUtils::Error);
        };

// init distro stucts for all targets
    if (!parsePrefixesPrivate(mainDistro, binOut, &DistroModule::setBinOutDir)) {
        erroLog("binOut");
        return false;
    }

    if (!parsePrefixesPrivate(mainDistro, libOut, &DistroModule::setLibOutDir)) {
        erroLog("libOut");
        return false;
    }

    if (!parsePrefixesPrivate(mainDistro, qmlOut, &DistroModule::setQmlOutDir)) {
        erroLog("qmlOut");
        return false;
    }

    if (!parsePrefixesPrivate(mainDistro, trOut, &DistroModule::setTrOutDir)) {
        erroLog("trOut");
        return false;
    }

    if (!parsePrefixesPrivate(mainDistro, pluginOut, &DistroModule::setPluginsOutDir)) {
        erroLog("pluginOut");
        return false;
    }

    if (!parsePrefixesPrivate(mainDistro, recOut, &DistroModule::setResOutDir)) {
        erroLog("recOut");
        return false;
    }

    return true;
}

bool ConfigParser::initPrefixes() {

    if (QuasarAppUtils::Params::isEndable("targetPrefix")) {
        auto tar_prefixes_array = QuasarAppUtils::Params::getStrArg("targetPrefix", "").
                split(DeployCore::getSeparator(0));


        for (auto& str: tar_prefixes_array) {
            auto pair = str.split(DeployCore::getSeparator(1));
            auto prefix = pair.value(0, "");

            auto list = _config.getTargetsListByFilter(pair.value(1, ""));

            for (auto &target : list) {
                target->setSufix(prefix);
            }

            _config.prefixes.insert(prefix, {});

            if (pair.size() != 2) {

                QuasarAppUtils::Params::verboseLog(
                            "Wrong the targetPrefix value. The targetPrefix value must be "
                            "contains the prefix path like first item and target mask like second item."
                            " Prefix and Target must be separated ';' char. "
                            " For example:  -targetPrefix prefix/path/1;target1,prefix/path/2;tar",
                            QuasarAppUtils::Error);

                return false;

            }
        }
    }

    return true;
}

bool ConfigParser::initQmlInput() {

    auto qmlDir = QuasarAppUtils::Params::getStrArg("qmlDir").
            split(DeployCore::getSeparator(0));

    if (QuasarAppUtils::Params::isEndable("allQmlDependes")) {
        _config.deployQml = true;
    }

    for (auto& str: qmlDir) {

        if (QFileInfo::exists(str)) {
            _config.deployQml = true;

            auto targetVal = str.split(DeployCore::getSeparator(1));
            _config.prefixes[targetVal.value(1, "")].addQmlInput(targetVal.value(0, ""));
        } else {
            QuasarAppUtils::Params::verboseLog("qml dir not exits!",
                                               QuasarAppUtils::VerboseLvl::Warning);
        }
    }

    return true;
}

bool ConfigParser::parseQtDeployMode() {

    if (QuasarAppUtils::Params::isEndable("deploySystem-with-libc")) {
        QuasarAppUtils::Params::setEnable("deploySystem", true );
    }

    auto bin = QuasarAppUtils::Params::getStrArg("bin").
            split(DeployCore::getSeparator(0));

    if (!setTargets(bin)) {

        auto binDir = QuasarAppUtils::Params::getStrArg("binDir");
        if (!(setTargetsRecursive(binDir) || setTargets({"./"}))) {
            qCritical() << "setTargetDir fail!";
            return false;
        }
    }

    initIgnoreEnvList();
    initEnvirement();
    initIgnoreList();
    if (!initDistroStruct()) {
        return false;
    }


    _config.depchLimit = 0;

    if (QuasarAppUtils::Params::isEndable("recursiveDepth")) {
        bool ok;
        _config.depchLimit = QuasarAppUtils::Params::getArg("recursiveDepth").toInt(&ok);
        if (!ok) {
            _config.depchLimit = 0;
            qWarning() << "recursiveDepth is invalid! use default value 0";
        }
    }

    auto listLibDir = QuasarAppUtils::Params::getStrArg("libDir").
            split(DeployCore::getSeparator(0));
    auto listNamesMasks = QuasarAppUtils::Params::getStrArg("extraLibs").
            split(DeployCore::getSeparator(0));

    auto listExtraPlugin = QuasarAppUtils::Params::getStrArg("extraPlugin").
            split(DeployCore::getSeparator(0));

    setExtraPath(listLibDir);
    setExtraNames(listNamesMasks);
    setExtraPlugins(listExtraPlugin);

    if (!initQmake()) {
        return false;
    }

    initQmlInput();

    return true;
}

bool ConfigParser::parseQtInfoMode() {
    if ((QuasarAppUtils::Params::isEndable("v") ||
            QuasarAppUtils::Params::isEndable("version"))) {
        DeployCore::printVersion();
        return true;
    }

    DeployCore::help();

    return true;
}

bool ConfigParser::parseQtClearMode() {
    setTargetDir("./" + DISTRO_DIR);

    return true;
}

QSet<QString> ConfigParser::getQtPathesFromTargets() {
    QSet<QString> res;

    for (auto &i: _config.targets) {
        if (i.isValid() && !i.getQtPath().isEmpty()) {
            res.insert(i.getQtPath());
        }
    }

    return res;
}

void ConfigParser::setTargetDir(const QString &target) {

    if (QuasarAppUtils::Params::isEndable("targetDir")) {
        _config.setTargetDir(QFileInfo(QuasarAppUtils::Params::getStrArg("targetDir")).absoluteFilePath());
    } else if (target.size()) {
        _config.setTargetDir(QFileInfo(target).absoluteFilePath());
    } else {

        _config.setTargetDir(QFileInfo("./" + DISTRO_DIR).absoluteFilePath());
        qInfo () << "flag targetDir not  used." << "use default target dir :" << _config.getTargetDir();
    }
}

bool ConfigParser::setTargets(const QStringList &value) {

    bool isfillList = false;

    for (auto &i : value) {
        QFileInfo targetInfo(i);

        if (i.isEmpty())
            continue;

        if (targetInfo.isFile()) {

            auto sufix = targetInfo.completeSuffix();

            _config.targets.unite(createTarget(QDir::fromNativeSeparators(i)));

            isfillList = true;
        }
        else if (targetInfo.isDir()) {
            if (!setBinDir(i)) {
                DeployCore::verboseLog(i + " du not contains executable binaries!");
                continue;
            }
            isfillList = true;

        } else {
            DeployCore::verboseLog(targetInfo.absoluteFilePath() + " not exits!");
        }
    }

    if (!isfillList)
        return false;

    setTargetDir();

    return true;
}

bool ConfigParser::setTargetsRecursive(const QString &dir) {
    if (!setBinDir(dir, true)) {
        qWarning() << "setBinDir failed!";
        return false;
    }

    setTargetDir();

    return true;
}

bool ConfigParser::setBinDir(const QString &dir, bool recursive) {
    QDir d(dir);
    if (dir.isEmpty() || !d.exists()) {
        DeployCore::verboseLog(dir + " dir not exits!");
        return false;
    }
    DeployCore::verboseLog("setBinDir check path: " + dir);
    QFileInfoList list;

    if (recursive) {
        list = d.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    } else {
        list = d.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    }

    bool result = false;
    for (auto &file : list) {

        if (file.isDir()) {
            result |= setBinDir(file.absoluteFilePath(), recursive);
            continue;
        }

        auto name = file.fileName();
        auto sufix = file.completeSuffix();

        if (sufix.isEmpty() ||  name.contains(".dll", Qt::CaseInsensitive) ||
              name.contains(".so", Qt::CaseInsensitive) || name.contains(".exe", Qt::CaseInsensitive)) {

            result = true;

            _config.targets.unite(createTarget(QDir::fromNativeSeparators(file.absoluteFilePath())));

        }

       }

    return result;
}

QHash<QString, TargetInfo> ConfigParser::createTarget(const QString &target) {
    TargetInfo libinfo;
    auto key = target;
    if (_scaner->fillLibInfo(libinfo, key)) {
        return {{libinfo.fullPath(), libinfo}};
    } else {
        return {{key, {}}};
    }
}

QHash<QString, TargetInfo>
ConfigParser::moveTarget(TargetInfo target, const QString& newLocation) {
    target.setPath(QFileInfo(newLocation).absolutePath());

    return {{newLocation, target}};

}

void ConfigParser::initIgnoreList()
{
    if (QuasarAppUtils::Params::isEndable("ignore")) {
        auto list = QuasarAppUtils::Params::getStrArg("ignore").
                split(DeployCore::getSeparator(0));

        for (auto &i : list) {
            _config.ignoreList.addRule(IgnoreData(i));
        }

    }

    IgnoreData ruleUnix, ruleWin;

    Envirement envUnix, envWin;

    if (!QuasarAppUtils::Params::isEndable("deploySystem-with-libc")) {

        envUnix.addEnv(recursiveInvairement("/lib", 3), "", "");
        envUnix.addEnv(recursiveInvairement("/usr/lib", 3), "", "");
        ruleUnix.prority = SystemLib;
        ruleUnix.platform = Unix;
        ruleUnix.enfirement = envUnix;


        auto addRuleUnix = [&ruleUnix](const QString & lib) {
            ruleUnix.label = lib;
            return ruleUnix;
        };

        _config.ignoreList.addRule(addRuleUnix("libc"));
        _config.ignoreList.addRule(addRuleUnix("ld-"));
        _config.ignoreList.addRule(addRuleUnix("libpthread"));
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
        _config.ignoreList.addRule(addRuleUnix("libcrypt"));
        _config.ignoreList.addRule(addRuleUnix("/gconv/"));
        _config.ignoreList.addRule(addRuleUnix("libnss"));
    }

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    auto path = env.value("PATH");
    auto winPath = findWindowsPath(path);

    envWin.addEnv(recursiveInvairement(winPath + "/System32", 2), "", "");
    envWin.addEnv(recursiveInvairement(winPath + "/SysWOW64", 2), "", "");

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

}

void ConfigParser::initIgnoreEnvList() {
    QStringList ignoreEnvList;

    if (QuasarAppUtils::Params::isEndable("ignoreEnv")) {
        auto ignoreList = QuasarAppUtils::Params::getStrArg("ignoreEnv").
                split(DeployCore::getSeparator(0));


        for (auto &i : ignoreList) {
            auto path = QFileInfo(i).absoluteFilePath();

            if (path.right(1) == "/" || path.right(1) == "\\") {
                path.remove(path.size() - 1, 1);
            }

            ignoreEnvList.append(path);
        }
    }

    _config.envirement.setIgnoreEnvList(ignoreEnvList);

}

QString ConfigParser::getPathFrmoQmakeLine(const QString &in) const {
    auto list = in.split(':');
    if (list.size() > 1) {
        list.removeAt(0);
        return QFileInfo(list.join(':')).absoluteFilePath().remove('\r');
    }

    return "";
}

bool ConfigParser::initQmakePrivate(const QString &qmake) {
    QFileInfo info(qmake);

    QString basePath = info.absolutePath();
    if (!setQmake(qmake)) {
        QDir dir(basePath);

        if (!dir.cdUp()) {
            QuasarAppUtils::Params::verboseLog("fail init qmake",
                                               QuasarAppUtils::Error);
            return false;
        }

        QuasarAppUtils::Params::verboseLog("exec qmake fail!, try init qtDir from path:" + dir.absolutePath(),
                                           QuasarAppUtils::Warning);

        if (!setQtDir(dir.absolutePath())){
            QuasarAppUtils::Params::verboseLog("fail ini qmake",
                                               QuasarAppUtils::Error);
            return false;
        }

    }

    return true;
}

bool ConfigParser::initQmake() {

    auto qmake = QuasarAppUtils::Params::getStrArg("qmake");

    QFileInfo info(qmake);

    if (!info.isFile() || (info.baseName() != "qmake")) {

        auto qtList = getQtPathesFromTargets();

        if (qtList.isEmpty()) {
            qInfo() << "deploy only C libs because qmake is not found";
            return true;

        } else if (qtList.size() > 1) {
            QuasarAppUtils::Params::verboseLog("Your deployment targets were compiled by different qmake,"
                                               "qt auto-capture is not possible. Use the -qmake flag to solve this problem.",
                                               QuasarAppUtils::Error);
            return false;
        }

        auto qt = *qtList.begin();

        if (qt.right(3).compare("lib", Qt::CaseInsensitive)) {
            return initQmakePrivate(QFileInfo(qt + "/../bin/qmake").absoluteFilePath());
        }

        return initQmakePrivate(QFileInfo(qt + "/qmake").absoluteFilePath());
    }
    return initQmakePrivate(qmake);
}

bool ConfigParser::setQmake(const QString &value) {

    auto qmakeInfo = QFileInfo(QDir::fromNativeSeparators(value));

    if (!(qmakeInfo.fileName().compare("qmake", Qt::CaseInsensitive) ||
        qmakeInfo.fileName().compare("qmake.exe", Qt::CaseInsensitive))) {
        return false;
    }

    QProcess proc;
    proc.setProgram(qmakeInfo.absoluteFilePath());
    proc.setProcessEnvironment(QProcessEnvironment::systemEnvironment());
    proc.setArguments({"-query"});

    proc.start();
    if (!proc.waitForFinished(1000)) {
        QuasarAppUtils::Params::verboseLog("run qmake fail!");

        return false;
    }

    QString qmakeData = proc.readAll();
    auto list = qmakeData.split('\n');

    for (auto &value : list) {
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
        } else if (value.contains("QMAKE_XSPEC")) {
            auto val = value.split(':').value(1);

            if (val.contains("win32")) {
                _config.qtDir.setQtPlatform(Platform::Win);
            } else {
                _config.qtDir.setQtPlatform(Platform::Unix);
            }
        }
    }
    _config.envirement.addEnv(_config.qtDir.getLibs(), _config.appDir, _config.getTargetDir());
    _config.envirement.addEnv(_config.qtDir.getBins(), _config.appDir, _config.getTargetDir());

    return true;
}

bool ConfigParser::setQtDir(const QString &value) {

    QFileInfo info(value);

    if (!QFile::exists(info.absoluteFilePath() + ("/bin"))) {
        QuasarAppUtils::Params::verboseLog("get qt bin fail!");
        return false;
    }
    _config.qtDir.setBins(info.absoluteFilePath() + ("/bin"));

    if (!QFile::exists(info.absoluteFilePath() + ("/lib"))) {
        QuasarAppUtils::Params::verboseLog("get qt lib fail!");
        return false;
    }
    _config.qtDir.setLibs(info.absoluteFilePath() + ("/lib"));

    if (!QFile::exists(info.absoluteFilePath() + ("/qml"))) {
        QuasarAppUtils::Params::verboseLog("get qt qml fail!");
    } else {
        _config.qtDir.setQmls(info.absoluteFilePath() + ("/qml"));
    }

    if (!QFile::exists(info.absoluteFilePath() + ("/plugins"))) {
        QuasarAppUtils::Params::verboseLog("get qt plugins fail!");
    } else {
        _config.qtDir.setPlugins(info.absoluteFilePath() + ("/plugins"));
    }

#ifdef Q_OS_UNIX
    if (!QFile::exists(info.absoluteFilePath() + ("/libexec"))) {
        QuasarAppUtils::Params::verboseLog("get qt libexec fail!");
    } else {
        _config.qtDir.setLibexecs(info.absoluteFilePath() + ("/libexec"));
    }
#endif
#ifdef Q_OS_WIN
    _config.qtDir.setLibexecs(info.absoluteFilePath() + ("/bin"));
#endif

    if (!QFile::exists(info.absoluteFilePath() + ("/translations"))) {
        QuasarAppUtils::Params::verboseLog("get qt translations fail!");
    } else {
        _config.qtDir.setTranslations(info.absoluteFilePath() + ("/translations"));
    }

    if (!QFile::exists(info.absoluteFilePath() + ("/resources"))) {
        QuasarAppUtils::Params::verboseLog("get qt resources fail!");
    } else {
        _config.qtDir.setResources(info.absoluteFilePath() + ("/resources"));
    }

#ifdef Q_OS_UNIX
    _config.qtDir.setQtPlatform(Platform::Unix);
#endif
#ifdef Q_OS_WIN
    _config.qtDir.setQtPlatform(Platform::Win);
#endif

    _config.envirement.addEnv(_config.qtDir.getLibs(), _config.appDir, _config.getTargetDir());
    _config.envirement.addEnv(_config.qtDir.getBins(), _config.appDir, _config.getTargetDir());

    return true;
}

void ConfigParser::setExtraPath(const QStringList &value) {
    QDir dir;

    for (auto i : value) {
        QFileInfo info(i);
        if (info.isDir()) {
            if (_config.targets.contains(info.absoluteFilePath())) {
                QuasarAppUtils::Params::verboseLog("skip the extra lib path becouse it is target!",
                                                   QuasarAppUtils::Info);
                continue;
            }

            dir.setPath(info.absoluteFilePath());
            auto extraDirs = getSetDirsRecursive(QDir::fromNativeSeparators(info.absoluteFilePath()), _config.depchLimit);
            _config.extraPaths.addExtraPaths(extraDirs);

            _config.envirement.addEnv(recursiveInvairement(dir), _config.appDir, _config.getTargetDir());
        } else if (i.size() > 1) {

            _config.extraPaths.addExtraPathsMasks({i});

            QuasarAppUtils::Params::verboseLog(i + " added like a path mask",
                                               QuasarAppUtils::Info);
        } else {
            QuasarAppUtils::Params::verboseLog(i + " not added in path mask becouse"
                                                   " the path mask must be large 2 characters",
                                               QuasarAppUtils::Warning);
        }
    }
}

void ConfigParser::setExtraNames(const QStringList &value) {
    for (auto i : value) {
        if (i.size() > 1) {
            _config.extraPaths.addtExtraNamesMasks({i});

            QuasarAppUtils::Params::verboseLog(i + " added like a file name mask",
                                               QuasarAppUtils::Info);
        } else {
            QuasarAppUtils::Params::verboseLog(i + " not added in file mask becouse"
                                                   " the file mask must be large 2 characters",
                                               QuasarAppUtils::Warning);
        }

    }
}

void ConfigParser::setExtraPlugins(const QStringList &value) {
    for (auto i : value) {
        if (!i.isEmpty())
            _config.extraPlugins.append(i);
    }
}

QString ConfigParser::recursiveInvairement(QDir &dir, int depch, int depchLimit ) {

    char separator = ':';

#ifdef Q_OS_WIN
    separator = ';';
#endif

    if (depchLimit < 0) {
        depchLimit = _config.depchLimit;
    }

    if (!dir.exists() || depch >= depchLimit) {
        return dir.absolutePath();
    }

    QFileInfoList list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    QString res = "";

    for (QFileInfo &i : list) {
        if (!dir.cd(i.fileName())) {
            continue;
        }
        QString temp = recursiveInvairement(dir, depch + 1);
        res += (res.size())? separator + temp: temp;

        dir.cdUp();
    }

    res += (res.size())? separator + dir.absolutePath(): dir.absolutePath();

    return res;
}

QString ConfigParser::recursiveInvairement(const QString &dir, int depch) {
    QDir _dir(dir);

    return recursiveInvairement(_dir, 0, depch);
}

QString ConfigParser::findWindowsPath(const QString& path) const {
    auto list = path.split(';');
    QString win_magic = "windows";

    for (auto i: list ) {
        int index = i.indexOf(win_magic, 0, Qt::CaseInsensitive);
        if (index > 0 && i.size() == index + win_magic.size()) {
            return QDir::fromNativeSeparators(i);
        }
    }

    return "C:/" + win_magic;
}

void ConfigParser::initEnvirement() {
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    auto path = env.value("PATH");

    _config.envirement.addEnv(env.value("LD_LIBRARY_PATH"), _config.appDir, _config.getTargetDir());
    _config.envirement.addEnv(path, _config.appDir, _config.getTargetDir());


    QStringList dirs;


#ifdef Q_OS_LINUX

    dirs.append(getDirsRecursive("/lib", 5));
    dirs.append(getDirsRecursive("/usr/lib", 5));
#else
    auto winPath = findWindowsPath(path);
    dirs.append(getDirsRecursive(winPath + "/System32", 2));
    dirs.append(getDirsRecursive(winPath + "/SysWOW64", 2));

#endif



    for (auto &&i : dirs) {
        _config.envirement.addEnv(i, _config.appDir, _config.getTargetDir());
    }

    if (_config.envirement.size() < 2) {
        qWarning() << "system environment is empty";
    }
}

QStringList ConfigParser::getDirsRecursive(const QString &path, int maxDepch, int depch) {
    return getSetDirsRecursive(path, maxDepch, depch).toList();
}

QSet<QString> ConfigParser::getSetDirsRecursive(const QString &path, int maxDepch, int depch) {
    QDir dir(path);

    QSet<QString> res = {path};

    if (maxDepch >= 0 && maxDepch <= depch) {
        return res;
    }

    auto list = dir.entryInfoList(QDir::Dirs| QDir::NoDotAndDotDot);

    for (auto &&subDir: list) {
        res.insert(subDir.absoluteFilePath());
        res.unite(getSetDirsRecursive(subDir.absoluteFilePath(), maxDepch, depch + 1));
    }

    return res;
}

bool ConfigParser::smartMoveTargets() {

    decltype (_config.targets) temp;
    bool result = true;
    for (auto i = _config.targets.cbegin(); i != _config.targets.cend(); ++i) {

        QFileInfo target(i.key());

        QString targetPath = _config.getTargetDir();

        if (DeployCore::isLib(target)) {
            targetPath += _config.getDistro(i.key()).getLibOutDir();
        } else {
            targetPath += _config.getDistro(i.key()).getBinOutDir();
        }

        if (!_fileManager->smartCopyFile(target.absoluteFilePath(), targetPath)) {
            result = false;
        }

        auto newTargetKey = targetPath + "/" + target.fileName();
        temp.unite(moveTarget(i.value(), newTargetKey));

        _config.prefixes[i.value().getSufix()].addTarget(newTargetKey);

    }

    _config.targets = temp;

    return result;
}

ConfigParser::ConfigParser(FileManager *filemanager, DependenciesScanner* scaner):
    _fileManager(filemanager),
    _scaner(scaner) {

    assert(_fileManager);
    assert(_scaner);

#ifdef Q_OS_LINUX
    _config.appDir = QuasarAppUtils::Params::getStrArg("appPath");

    if (_config.appDir.right(4) == "/bin") {
        _config.appDir = _config.appDir.left(_config.appDir.size() - 4);
    }
#else
    _config.appDir = QuasarAppUtils::Params::getStrArg("appPath");
#endif

    QuasarAppUtils::Params::verboseLog("appDir = " + _config.appDir);
}
