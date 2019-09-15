#include "configparser.h"
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include "deploycore.h"
#include "filemanager.h"
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

void ConfigParser::writeKey(const QString& key, QJsonObject& obj) const {
    if (QuasarAppUtils::Params::isEndable(key)) {
        auto list = QuasarAppUtils::Params::getStrArg(key).split(',');

        if (list.size() > 1) {
            QJsonArray array;

            for (auto &i: list) {
                QJsonValue val;
                val = i;
                array.push_back(val);
            }

            obj[key] = array;
        } else {
            if (list.size() && list.first().isEmpty()) {
                obj[key] = QJsonValue(true);
            } else {
                obj[key] = list.first();
            }
        }
    }
}

void ConfigParser::readKey(const QString& key, const QJsonObject& obj) const {
    if (!QuasarAppUtils::Params::isEndable(key)) {

         if (obj[key].isArray()) {
             auto array = obj[key].toArray();
             QStringList list;

             for (QJsonValue i : array) {
                 QString val = i.toString();
                 if (!val.isEmpty()) {
                     list.push_back(val);
                 }
             }

             QuasarAppUtils::Params::setArg(key, list.join(','));

         } else if (!obj[key].isUndefined()) {
             QString val = obj[key].toString();
             if (!val.isEmpty()) {
                 QuasarAppUtils::Params::setArg(key, val);
             } else {
                 QuasarAppUtils::Params::setEnable(key, true);
             }
         }

    }
}

bool ConfigParser::createFromDeploy(const QString& confFile) const {
    QJsonObject obj;

    for (auto &key :DeployCore::helpKeys()) {
        writeKey(key, obj);
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

    if (file.open(QIODevice::ReadOnly)) {
        auto doc = QJsonDocument::fromJson(file.readAll());

        if (!doc.isObject()) {
            return false;
        }

        auto obj = doc.object();

        for (auto &key: obj.keys()) {
            readKey(key, obj);
        }

        file.close();

        return true;
    }

    return false;
}

bool ConfigParser::parseQtDeployMode() {

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
    setTargetDir("./");

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
                        _config.targets.begin().key()).absolutePath() + "/Distro";

        _config.targetDir = QFileInfo("./Distro").absoluteFilePath();
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

            _config.targets.insert(QDir::fromNativeSeparators(i), sufix.isEmpty());
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
            _config.targets.insert(QDir::fromNativeSeparators(file.absoluteFilePath()), sufix.isEmpty());

        }

       }

    return result;
}

void ConfigParser::initIgnoreList()
{
    if (QuasarAppUtils::Params::isEndable("ignore")) {
        auto list = QuasarAppUtils::Params::getStrArg("ignore").split(',');
        _config.ignoreList.append(list);
    }

    if (QuasarAppUtils::Params::isEndable("noLibc")) {
        _config.ignoreList.append("libc.so");

    }
}

void ConfigParser::initIgnoreEnvList() {
    if (QuasarAppUtils::Params::isEndable("ignoreEnv")) {
        auto ignoreList = QuasarAppUtils::Params::getStrArg("ignoreEnv").split(',');

        QStringList ignoreEnvList;

        for (auto &i : ignoreList) {
            auto path = QFileInfo(i).absoluteFilePath();

            if (path.right(1) == "/" || path.right(1) == "\\") {
                path.remove(path.size() - 1, 1);
            }

            ignoreEnvList.append(path);
        }

        _config.envirement.setIgnoreEnvList(ignoreEnvList);
    }
}

void ConfigParser::setQmlScaner(const QString &value) {
    _config.externQmlScaner = QDir::fromNativeSeparators(value);
    QuasarAppUtils::Params::verboseLog("qmlScaner = " + _config.externQmlScaner,
                                       QuasarAppUtils::VerboseLvl::Info);
    _config.deployQml = QFileInfo(_config.externQmlScaner).isFile();
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
            _config.extraPaths.push_back(
                        QDir::fromNativeSeparators(info.absoluteFilePath()));
            _config.envirement.addEnv(recursiveInvairement(0, dir), _config.appDir, _config.targetDir);
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

QString ConfigParser::recursiveInvairement(int depch, QDir &dir) {

    char separator = ':';

#ifdef Q_OS_WIN
    separator = ';';
#endif

    if (!dir.exists() || depch >= _config.depchLimit) {
        return dir.absolutePath();
    }

    QFileInfoList list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    QString res = "";

    for (QFileInfo &i : list) {
        if (!dir.cd(i.fileName())) {
            continue;
        }
        QString temp = recursiveInvairement(depch + 1, dir);
        res += (res.size())? separator + temp: temp;

        dir.cdUp();
    }

    res += (res.size())? separator + dir.absolutePath(): dir.absolutePath();

    return res;
}

void ConfigParser::initEnvirement() {
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    _config.envirement.addEnv(env.value("LD_LIBRARY_PATH"), _config.appDir, _config.targetDir);
    _config.envirement.addEnv(env.value("PATH"), _config.appDir, _config.targetDir);


    if (QuasarAppUtils::Params::isEndable("deploySystem")) {
        QStringList dirs;
        if (!QuasarAppUtils::Params::isEndable("noLibc"))
            dirs.append(getDirsRecursive("/lib", 20));
        dirs.append(getDirsRecursive("/usr/lib", 20));

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

    QStringList res;

    if (maxDepch > 0 && maxDepch < depch) {
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

    QMap<QString, bool> temp;
    bool result = true;
    for (auto i = _config.targets.cbegin(); i != _config.targets.cend(); ++i) {

        QFileInfo target(i.key());
        auto targetPath = _config.targetDir + (DeployCore::isLib(target) ? "/lib" : "/bin");

        if (target.completeSuffix().compare("dll", Qt::CaseInsensitive) == 0 ||
                target.completeSuffix().compare("exe", Qt::CaseInsensitive) == 0) {

            targetPath = _config.targetDir;

        }

        if (!_fileManager->smartCopyFile(target.absoluteFilePath(), targetPath, _config.targetDir)) {
            result = false;
        }


        temp.insert(targetPath + "/" + target.fileName(), i.value());

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
