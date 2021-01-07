//#
//# Copyright (C) 2018-2021 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#include "deploycore.h"
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

QStringList Envirement::environmentList() const {
    return _dataEnvironment.values();
}

QStringList Envirement::ignoreEnvList() const {
    return _ignoreEnvList->environmentList();
}

void Envirement::setIgnoreEnvList(const QStringList &ignoreEnvList) {

    if (!_ignoreEnvList)
        _ignoreEnvList = new Envirement();

    _ignoreEnvList->addEnv(ignoreEnvList);
}

void Envirement::setIgnoreEnvListRecursive(const QStringList &ignoreEnvList, int depch) {
    if (!_ignoreEnvList)
        _ignoreEnvList = new Envirement();

    for (const auto &i: ignoreEnvList) {
        _ignoreEnvList->addEnvRec(i, depch);
    }
}

void Envirement::addEnvRec(const QString &dir, int depch) {
    addEnv(Envirement::recursiveInvairement(dir, depch));
}

void Envirement::addEnv(const QString &dir) {

    char separator = DeployCore::getEnvSeparator();
    if (dir.contains(separator)) {
        addEnv(dir.split(separator));
    } else {
        addEnv(QStringList{dir});
    }
}

void Envirement::addEnv(const QStringList &listDirs) {

    for (const auto& i : listDirs) {
        auto path = PathUtils::fixPath(QFileInfo(i).absoluteFilePath());

        if (_ignoreEnvList && _ignoreEnvList->inThisEnvirement(i)) {
            continue;
        }

        if (!QFileInfo(path).isDir()) {
            QuasarAppUtils::Params::log("is not dir!! :" + path);
            continue;
        }

        if (_dataEnvironment.contains(path)) {
            QuasarAppUtils::Params::log ("Environment alredy added: " + path);
            continue;
        }

        _dataEnvironment.insert(path);
    }
}

void Envirement::clear() {
    if (_ignoreEnvList)
        delete _ignoreEnvList;

    _dataEnvironment.clear();

}

bool Envirement::inThisEnvirement(const QString &file) const {
    QFileInfo info (file);

    if (info.isFile()) {
        return _dataEnvironment.contains(PathUtils::fixPath(info.absolutePath()));
    }

    return _dataEnvironment.contains(PathUtils::fixPath(info.absoluteFilePath()));

}

int Envirement::size() const {
    return _dataEnvironment.size();
}

QString Envirement::concatEnv() const {

    if (_dataEnvironment.isEmpty()) {
        return "";
    }

    QString result = *_dataEnvironment.begin();
    for (const auto &i: _dataEnvironment) {
        result += (DeployCore::getEnvSeparator() + i);
    }

    return result;
}

QStringList Envirement::recursiveInvairement(QDir &dir, int depch, int depchLimit) {

    if (!dir.exists() || (depchLimit >= 0 && depch >= depchLimit)) {
        return {dir.absolutePath()};
    }

    QFileInfoList list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    QStringList res = {};

    for (QFileInfo &i : list) {
        if (!dir.cd(i.fileName())) {
            continue;
        }
        res += Envirement::recursiveInvairement(dir, depch + 1, depchLimit);

        dir.cdUp();
    }

    res += dir.absolutePath();

    return res;
}

QStringList Envirement::recursiveInvairement(const QString &dir, int depch) {
    QDir _dir(dir);

    return recursiveInvairement(_dir, 0, depch);
}

Envirement::Envirement() {

}

Envirement::~Envirement() {
    if (_ignoreEnvList) {
        delete _ignoreEnvList;
        _ignoreEnvList = nullptr;
    }
}
