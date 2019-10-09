//#
//# Copyright (C) 2018-2019 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#include "configparser.h"
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include "deploycore.h"
#include "filemanager.h"
#include "pathutils.h"
#include "quasarapp.h"

bool ConfigParser::parseParams() {

    auto path = QuasarAppUtils::Params::getStrArg("confFile");
    bool createFile = !path.isEmpty();
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

    if (createFile) {
        createFromDeploy(path);
    }

    return true;
}

const DeployConfig *ConfigParser::config() const {
    return &_config;
}

void ConfigParser::writeKey(const QString& key, QJsonObject& obj, const QString& confFileDir) const {
    if (QuasarAppUtils::Params::isEndable(key)) {
        auto list = QuasarAppUtils::Params::getStrArg(key).split(',');

        if (list.size() > 1) {
            QJsonArray array;

            for (auto &i: list) {
                QJsonValue val;
                val = i;

                if (PathUtils::isPath(i)) {
                    val = PathUtils::getRelativeLink(
                                QFileInfo(confFileDir).absoluteFilePath(),
                                QFileInfo(i).absoluteFilePath());
                }
                array.push_back(val);
            }

            obj[key] = array;
        } else {
            if (list.size() && list.first().isEmpty()) {
                obj[key] = QJsonValue(true);
            } else {
                auto val = list.first();

                if (PathUtils::isPath(val)) {
                    val = PathUtils::getRelativeLink(
                                QFileInfo(confFileDir).absoluteFilePath(),
                                QFileInfo(val).absoluteFilePath());

                }

                obj[key] = val;
            }
        }
    }
}

void ConfigParser::readKey(const QString& key, const QJsonObject& obj,
                           const QString& confFileDir) const {

    if (!QuasarAppUtils::Params::isEndable(key)) {

         if (obj[key].isArray()) {
             auto array = obj[key].toArray();
             QStringList list;

             for (QJsonValue i : array) {
                 QString val = i.toString();

                 if (!val.isEmpty()) {
                     if (PathUtils::isPath(val)) {
                         list.push_back(
                                     QFileInfo(confFileDir + '/' + val).absoluteFilePath());

                     } else {
                         list.push_back(val);
                     }
                 }
             }

             QuasarAppUtils::Params::setArg(key, list.join(','));

         } else if (!obj[key].isUndefined()) {
             QString val = obj[key].toString();
             if (!val.isEmpty()) {

                 if (PathUtils::isPath(val)) {
                     val = QFileInfo(confFileDir + '/' + val).absoluteFilePath();
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

    for (auto &key :DeployCore::helpKeys()) {
        writeKey(key, obj, QFileInfo(confFile).absolutePath());
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

bool ConfigParser::parseQtDeployMode() {

    if (QuasarAppUtils::Params::isEndable("deploySystem-with-libc")) {
        QuasarAppUtils::Params::setEnable("deploySystem", true );
    }

    auto bin = QuasarAppUtils::Params::getStrArg("bin").split(',');

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

    _config.distroStruct.init();

    _config.depchLimit = 0;

    if (QuasarAppUtils::Params::isEndable("recursiveDepth")) {
        bool ok;
        _config.depchLimit = QuasarAppUtils::Params::getArg("recursiveDepth").toInt(&ok);
        if (!ok) {
            _config.depchLimit = 0;
            qWarning() << "recursiveDepth is invalid! use default value 0";
        }
    }

    auto listLibDir = QuasarAppUtils::Params::getStrArg("libDir").split(",");
    auto listExtraPlugin =
            QuasarAppUtils::Params::getStrArg("extraPlugin").split(",");
    setExtraPath(listLibDir);
    setExtraPlugins(listExtraPlugin);

    auto qmake = QuasarAppUtils::Params::getStrArg("qmake");
    QString basePath = "";

    QFileInfo info(qmake);

    if (!info.isFile() || (info.baseName() != "qmake")) {
        qInfo() << "deploy only C libs because qmake is not found";
        return true;
    }

    basePath = info.absolutePath();
    setQmake(qmake);

    auto qmlDir = QuasarAppUtils::Params::getStrArg("qmlDir");
    QDir dir(basePath);

    if (QFileInfo::exists(qmlDir) ||
            QuasarAppUtils::Params::isEndable("allQmlDependes")) {
        _config.deployQml = true;

    } else {
        QuasarAppUtils::Params::verboseLog("qml dir not exits!",
                                           QuasarAppUtils::VerboseLvl::Warning);
    }

    if (!dir.cdUp()) {
        return false;
    }

    setQtDir(dir.absolutePath());

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

void ConfigParser::setTargetDir(const QString &target) {

    if (QuasarAppUtils::Params::isEndable("targetDir")) {
        _config.targetDir = QFileInfo(QuasarAppUtils::Params::getStrArg("targetDir")).absoluteFilePath();
    } else if (target.size()) {
        _config.targetDir = QFileInfo(target).absoluteFilePath();
    } else {
        if (_config.targets.size())
            _config.targetDir = QFileInfo(
                        *_config.targets.begin()).absolutePath() + "/" + DISTRO_DIR;

        _config.targetDir = QFileInfo("./" + DISTRO_DIR).absoluteFilePath();
        qInfo () << "flag targetDir not  used." << "use default target dir :" << _config.targetDir;
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

            _config.targets.insert(QDir::fromNativeSeparators(i));
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
            _config.targets.insert(QDir::fromNativeSeparators(file.absoluteFilePath()));

        }

       }

    return result;
}

void ConfigParser::initIgnoreList()
{
    if (QuasarAppUtils::Params::isEndable("ignore")) {
        auto list = QuasarAppUtils::Params::getStrArg("ignore").split(',');

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
        ruleUnix.platform = Unix32 | Unix64;
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

//    envWin.addEnv(recursiveInvairement("C:/Windows", 3), "", "");
//    ruleWin.prority = ExtraLib;
//    ruleWin.platform = Win32 | Win64;
//    ruleWin.enfirement = envWin;

//    auto addRuleWin = [&ruleWin](const QString & lib) {
//        ruleWin.label = lib;
//        return ruleWin;
//    };

//    _config.ignoreList.addRule(addRuleWin("kernelBase"));
//    _config.ignoreList.addRule(addRuleWin("gdi32"));
//    _config.ignoreList.addRule(addRuleWin("kernel32"));
//    _config.ignoreList.addRule(addRuleWin("msvcrt"));
//    _config.ignoreList.addRule(addRuleWin("user32"));
}

void ConfigParser::initIgnoreEnvList() {
    QStringList ignoreEnvList;

    // remove windows from envirement,
    ignoreEnvList.push_back(":/Windows");

    if (QuasarAppUtils::Params::isEndable("ignoreEnv")) {
        auto ignoreList = QuasarAppUtils::Params::getStrArg("ignoreEnv").split(',');


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

void ConfigParser::setQmake(const QString &value) {
    _config.qmake = QDir::fromNativeSeparators(value);

    QFileInfo info(_config.qmake);
    QDir dir = info.absoluteDir();

    if (!dir.cdUp() || !dir.cd("qml")) {
        QuasarAppUtils::Params::verboseLog("get qml fail!");
        return;
    }

    _config.qmlDir = dir.absolutePath();
    QuasarAppUtils::Params::verboseLog("qmlDir = " + _config.qmlDir);

    dir = (info.absoluteDir());
    if (!dir.cdUp() || !dir.cd("translations")) {
        QuasarAppUtils::Params::verboseLog("get translations fail!");
        return;
    }

    _config.translationDir = dir.absolutePath();
    QuasarAppUtils::Params::verboseLog("translations = " + _config.translationDir);
}

void ConfigParser::setQtDir(const QString &value) {
    _config.qtDir = QDir::fromNativeSeparators(value);
    _config.envirement.addEnv(_config.qtDir, _config.appDir, _config.targetDir);
    _config.envirement.addEnv(_config.qtDir + "/lib", _config.appDir, _config.targetDir);
    _config.envirement.addEnv(_config.qtDir + "/bin", _config.appDir, _config.targetDir);

}

void ConfigParser::setExtraPath(const QStringList &value) {
    QDir dir;

    for (auto i : value) {
        QFileInfo info(i);
        if (info.isDir()) {
            if (_config.targets.contains(info.absoluteFilePath())) {
                QuasarAppUtils::Params::verboseLog("skip the extra lib path becouse it is target!");
                continue;
            }

            dir.setPath(info.absoluteFilePath());
            auto extraDirs = getDirsRecursive(QDir::fromNativeSeparators(info.absoluteFilePath()), _config.depchLimit);
            _config.extraPaths.append(extraDirs);

            _config.envirement.addEnv(recursiveInvairement(dir), _config.appDir, _config.targetDir);
        } else {
            QuasarAppUtils::Params::verboseLog(i + " does not exist! and skiped");
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

void ConfigParser::initEnvirement() {
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    _config.envirement.addEnv(env.value("LD_LIBRARY_PATH"), _config.appDir, _config.targetDir);
    _config.envirement.addEnv(env.value("PATH"), _config.appDir, _config.targetDir);


    if (QuasarAppUtils::Params::isEndable("deploySystem")) {
        QStringList dirs;

        dirs.append(getDirsRecursive("/lib", 10));
        dirs.append(getDirsRecursive("/usr/lib", 10));

        for (auto &&i : dirs) {
            _config.envirement.addEnv(i, _config.appDir, _config.targetDir);
        }
    }

    if (_config.envirement.size() < 2) {
        qWarning() << "system environment is empty";
    }
}

QStringList ConfigParser::getDirsRecursive(const QString &path, int maxDepch, int depch) {
    QDir dir(path);

    QStringList res = {path};

    if (maxDepch >= 0 && maxDepch <= depch) {
        return res;
    }

    auto list = dir.entryInfoList(QDir::Dirs| QDir::NoDotAndDotDot);

    for (auto &&subDir: list) {
        res.push_back(subDir.absoluteFilePath());
        res.append(getDirsRecursive(subDir.absoluteFilePath(), maxDepch, depch + 1));
    }

    return res;
}

bool ConfigParser::smartMoveTargets() {

    QSet<QString> temp;
    bool result = true;
    for (auto i = _config.targets.cbegin(); i != _config.targets.cend(); ++i) {

        QFileInfo target(*i);

        QString targetPath = _config.targetDir;

        if (DeployCore::isLib(target)) {
            targetPath += _config.distroStruct.getLibOutDir();
        } else {
            targetPath += _config.distroStruct.getBinOutDir();
        }

        if (!_fileManager->smartCopyFile(target.absoluteFilePath(), targetPath)) {
            result = false;
        }


        temp.insert(targetPath + "/" + target.fileName());

    }

    _config.targets = temp;

    return result;
}

ConfigParser::ConfigParser(FileManager *filemanager):
    _fileManager(filemanager) {

    assert(_fileManager);

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

void DeployConfig::reset() {
    *this = DeployConfig{};
}
