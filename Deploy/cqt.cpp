#include "cqt.h"
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include "deploycore.h"
#include "quasarapp.h"

bool CQT::parseParams() {
    switch (DeployCore::getMode()) {
    case RunMode::Info: {
        qInfo() << "selected info mode" ;

        if (!parseQtInfoMode()) {
            qCritical() << "info mode fail!";
            return false;
        }

        return true;
    }
    case RunMode::Clear: {
        qInfo() << "selected clear mode" ;

        if (!parseQtClearMode()) {
            qCritical() << "clear mode fail!";
            return false;
        }

        return true;
    }

    case RunMode::Deploy: {
        qInfo() << "selected deploy mode" ;

        if (!parseQtDeployMode()) {
            qCritical() << "deploy mode fail!";
            return false;
        }

        return true;
    }

    }

    return false;
}

DeployConfig CQT::config() const {

}

void CQT::setConfig(const DeployConfig &config) {
    _config = config;
}

bool CQT::createFromDeploy() const {

}

bool CQT::loadFromFile() {

}

bool CQT::parseQtDeployMode() {

}

bool CQT::parseQtInfoMode() {

}

bool CQT::parseQtClearMode() {
    deploy->setTargetDir("./");
}

void CQT::setTargetDir(const QString &target) {

    if (QuasarAppUtils::Params::isEndable("targetDir")) {
        targetDir = QFileInfo(QuasarAppUtils::Params::getStrArg("targetDir")).absoluteFilePath();
    } else if (target.size()) {
        targetDir = QFileInfo(target).absoluteFilePath();
    } else {
        if (targets.size())
            targetDir = QFileInfo(targets.begin().key()).absolutePath() + "/Distro";

        targetDir = QFileInfo("./Distro").absoluteFilePath();
        qInfo () << "flag targetDir not  used." << "use default target dir :" << targetDir;
    }

    _fileManager.loadDeployemendFiles(targetDir);
}

bool CQT::setTargets(const QStringList &value) {

    bool isfillList = false;

    for (auto &i : value) {
        QFileInfo targetInfo(i);

        if (i.isEmpty())
            continue;

        if (targetInfo.isFile()) {

            auto sufix = targetInfo.completeSuffix();

            targets.insert(QDir::fromNativeSeparators(i), sufix.isEmpty());
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

bool CQT::setTargetsRecursive(const QString &dir) {
    if (!setBinDir(dir, true)) {
        qWarning() << "setBinDir failed!";
        return false;
    }

    setTargetDir();

    return true;
}

bool CQT::setBinDir(const QString &dir, bool recursive) {
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

        auto sufix = file.completeSuffix();

        if (!((!recursive && sufix.isEmpty()) ||  sufix.contains("dll", Qt::CaseInsensitive) ||
              sufix.contains("so", Qt::CaseInsensitive) || sufix.contains("exe", Qt::CaseInsensitive))) {
            continue;
        }

        result = true;
        targets.insert(QDir::fromNativeSeparators(file.absoluteFilePath()), sufix.isEmpty());
    }

    return result;
}

void CQT::initIgnoreList()
{
    if (QuasarAppUtils::Params::isEndable("ignore")) {
        auto list = QuasarAppUtils::Params::getStrArg("ignore").split(',');
        ignoreList.append(list);
    }

    if (QuasarAppUtils::Params::isEndable("noLibc")) {
        ignoreList.append("libc.so");

    }
}

void CQT::initIgnoreEnvList() {
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
}

void CQT::setQmlScaner(const QString &value) {
    externQmlScaner = QDir::fromNativeSeparators(value);
    QuasarAppUtils::Params::verboseLog("qmlScaner = " + externQmlScaner,
                                       QuasarAppUtils::VerboseLvl::Info);
    deployQml = QFileInfo(externQmlScaner).isFile();
}

void CQT::setQmake(const QString &value) {
    qmake = QDir::fromNativeSeparators(value);

    QFileInfo info(qmake);
    QDir dir = info.absoluteDir();

    if (!dir.cdUp() || !dir.cd("qml")) {
        QuasarAppUtils::Params::verboseLog("get qml fail!");
        return;
    }

    qmlDir = dir.absolutePath();
    QuasarAppUtils::Params::verboseLog("qmlDir = " + qmlDir);

    dir = (info.absoluteDir());
    if (!dir.cdUp() || !dir.cd("translations")) {
        QuasarAppUtils::Params::verboseLog("get translations fail!");
        return;
    }

    translationDir = dir.absolutePath();
    QuasarAppUtils::Params::verboseLog("translations = " + translationDir);
}

void CQT::setQtDir(const QString &value) {
    DeployCore::qtDir = QDir::fromNativeSeparators(value);
    addEnv(DeployCore::qtDir);

    addEnv(DeployCore::qtDir + "/lib");
    addEnv(DeployCore::qtDir + "/bin");

}

void CQT::setExtraPath(const QStringList &value) {
    QDir dir;

    for (auto i : value) {
        QFileInfo info(i);
        if (info.isDir()) {
            if (targets.contains(info.absoluteFilePath())) {
                QuasarAppUtils::Params::verboseLog("skip the extra lib path becouse it is target!");
                continue;
            }

            dir.setPath(info.absoluteFilePath());
            DeployCore::extraPaths.push_back(
                        QDir::fromNativeSeparators(info.absoluteFilePath()));
            addEnv(recursiveInvairement(0, dir));
        } else {
            QuasarAppUtils::Params::verboseLog(i + " does not exist! and skiped");
        }
    }
}

void CQT::setExtraPlugins(const QStringList &value) {
    for (auto i : value) {
        QFileInfo info(i);
        if (info.exists()) {
            extraPlugins.append(info.absoluteFilePath());
        } else {
            QuasarAppUtils::Params::verboseLog(i + " does not exist! and skiped");
        }
    }
}

QString CQT::recursiveInvairement(int depch, QDir &dir) {

    char separator = ':';

#ifdef Q_OS_WIN
    separator = ';';
#endif

    if (!dir.exists() || depch >= depchLimit) {
        return dir.absolutePath();
    }

    QFileInfoList list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    QString res = "";

    for (QFileInfo &i : list) {
        dir.cd(i.fileName());
        QString temp = recursiveInvairement(depch + 1, dir);
        res += (res.size())? separator + temp: temp;

        dir.cdUp();
    }

    res += (res.size())? separator + dir.absolutePath(): dir.absolutePath();

    return res;
}

void CQT::initEnvirement() {
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    addEnv(env.value("LD_LIBRARY_PATH"));
    addEnv(env.value("PATH"));

    if (QuasarAppUtils::Params::isEndable("deploySystem")) {
        QStringList dirs;
        if (!QuasarAppUtils::Params::isEndable("noLibc"))
            dirs.append(getDirsRecursive("/lib"));
        dirs.append(getDirsRecursive("/usr/lib"));

        for (auto &&i : dirs) {
            addEnv(i);
        }
    }

    if (deployEnvironment.size() < 2) {
        qWarning() << "system environment is empty";
    }
}

QStringList CQT::getDirsRecursive(const QString &path) {
    QDir dir(path);

    QStringList res;

    auto list = dir.entryInfoList(QDir::Dirs| QDir::NoDotAndDotDot);

    for (auto &&subDir: list) {
        res.push_back(subDir.absoluteFilePath());
        res.append(getDirsRecursive(subDir.absoluteFilePath()));
    }

    return res;
}



CQT::CQT() {
    
}
