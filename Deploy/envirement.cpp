//#
//# Copyright (C) 2018-2019 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#include "envirement.h"
#include "quasarapp.h"

#include <QDir>
#include <QFileInfo>

QStringList Envirement::deployEnvironment() const
{
    return _deployEnvironment.toList();
}

QStringList Envirement::ignoreEnvList() const
{
    return _ignoreEnvList.toList();
}

void Envirement::setIgnoreEnvList(const QStringList &ignoreEnvList)
{
    _ignoreEnvList = ignoreEnvList.toSet();
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

#ifdef Q_OS_WIN
        if (path.contains(i, Qt::CaseInsensitive)) {
            return;
        }
#else
        if (path.contains(i)) {
            return;
        }
#endif
    }


#ifdef Q_OS_WIN
    if (!appDir.isEmpty() && path.contains(appDir, Qt::CaseInsensitive)) {
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

    if (!targetDir.isEmpty() && path.contains(targetDir, Qt::CaseInsensitive)) {
        QuasarAppUtils::Params::verboseLog ("Skip paths becouse it is target : " + path);
        return;
    }
    _deployEnvironment.insert(QDir::fromNativeSeparators(path).toUpper());

#else
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

    _deployEnvironment.insert(QDir::fromNativeSeparators(path));
#endif

}

bool Envirement::inThisEnvirement(const QString &file) const {
    QFileInfo info (file);

#ifdef Q_OS_WIN

    if (info.isFile()) {
        return _deployEnvironment.contains(info.absolutePath().toUpper());
    }

    return _deployEnvironment.contains(file.toUpper());
#else
    if (info.isFile()) {
        return _deployEnvironment.contains(info.absolutePath());
    }

    return _deployEnvironment.contains(file);
#endif
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

Envirement::Envirement()
{

}
