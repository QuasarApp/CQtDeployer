//#
//# Copyright (C) 2018-2020 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#include "envirement.h"
#include "pathutils.h"
#include "quasarapp.h"

#include <QDir>
#include <QFileInfo>

QSet<QString> Envirement::upper(const QSet<QString>& set) const {
    QSet<QString> res;
    for (const auto &i : set) {
        res.insert(PathUtils::fixPath(i));
    }
    return res;
}

QStringList Envirement::deployEnvironment() const {
    return _deployEnvironment.toList();
}

QStringList Envirement::ignoreEnvList() const {
    return _ignoreEnvList.toList();
}

void Envirement::setIgnoreEnvList(const QStringList &ignoreEnvList) {
    _ignoreEnvList = upper(ignoreEnvList.toSet());
}

void Envirement::addEnvRec(const QString &dir, int depch, const QString &appDir, const QString &targetDir) {
    addEnv(Envirement::recursiveInvairement(dir, depch), appDir, targetDir);
}

void Envirement::addEnv(const QString &dir, const QString &appDir, const QString& targetDir) {

    char separator = ':';

#ifdef Q_OS_WIN
    separator = ';';
#endif

    if (dir.contains(separator)) {
        auto list = dir.split(separator);
        for (auto i : list) {
            addEnv(i, appDir, targetDir);
        }
        return;
    }

    auto path = QFileInfo(dir).absoluteFilePath();

    for (QString i :_ignoreEnvList) {
        if (path.contains(i, ONLY_WIN_CASE_INSENSIATIVE)) {
            return;
        }
    }

    if (!appDir.isEmpty() && path.contains(appDir)) {
        QuasarAppUtils::Params::verboseLog("is cqtdeployer dir!: " + path + " app dir : " + appDir);
        return;
    }

    if (!QFileInfo(path).isDir()) {
        QuasarAppUtils::Params::verboseLog("is not dir!! :" + path);
        return;
    }

    if (_deployEnvironment.contains(path)) {
        QuasarAppUtils::Params::verboseLog ("Environment alredy added: " + path);
        return;
    }

    if (!targetDir.isEmpty() && path.contains(targetDir)) {
        QuasarAppUtils::Params::verboseLog ("Skip paths becouse it is target : " + path);
        return;
    }

    _deployEnvironment.insert(PathUtils::fixPath(QDir::fromNativeSeparators(path)));

}

bool Envirement::inThisEnvirement(const QString &file) const {
    QFileInfo info (file);

    if (info.isFile()) {
        return _deployEnvironment.contains(PathUtils::fixPath(info.absolutePath()));
    }

    return _deployEnvironment.contains(PathUtils::fixPath(file));

}

int Envirement::size() const {
    return _deployEnvironment.size();
}

QString Envirement::concatEnv() const {

    if (_deployEnvironment.isEmpty()) {
        return "";
    }

    QString result = *_deployEnvironment.begin();
    for (auto i: _deployEnvironment) {
#ifdef  Q_OS_UNIX
        result += (":" + i);
#else
        result += (";" + i);
#endif
    }

    return result;
}

QString Envirement::recursiveInvairement(QDir &dir, int depch, int depchLimit) {

    char separator = ':';

#ifdef Q_OS_WIN
    separator = ';';
#endif

    if (!dir.exists() || (depchLimit >= 0 && depch >= depchLimit)) {
        return dir.absolutePath();
    }

    QFileInfoList list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    QString res = "";

    for (QFileInfo &i : list) {
        if (!dir.cd(i.fileName())) {
            continue;
        }
        QString temp = Envirement::recursiveInvairement(dir, depch + 1, depchLimit);
        res += (res.size())? separator + temp: temp;

        dir.cdUp();
    }

    res += (res.size())? separator + dir.absolutePath(): dir.absolutePath();

    return res;
}

QString Envirement::recursiveInvairement(const QString &dir, int depch) {
    QDir _dir(dir);

    return recursiveInvairement(_dir, 0, depch);
}

Envirement::Envirement()
{

}
