//#
//# Copyright (C) 2018-2021 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef ENVIREMENT_H
#define ENVIREMENT_H

#include <QSet>
#include <QStringList>
#include "deploy_global.h"

class QDir;

class DEPLOYSHARED_EXPORT Envirement
{
private:
    Envirement *_ignoreEnvList = nullptr;
    QSet<QString> _dataEnvironment;

    QSet<QString> upper(const QSet<QString> &set) const;
public:
    Envirement();
    ~Envirement();
    QStringList environmentList() const;
    QStringList ignoreEnvList() const;
    void setIgnoreEnvList(const QStringList &ignoreEnvList);
    void setIgnoreEnvListRecursive(const QStringList &ignoreEnvList, int depch = -1);

    void addEnvRec(const QString &dir, int depch);

    void addEnv(const QString &dir);
    void addEnv(const QStringList &listDirs);

    void clear();

    // return true if file exits in this envirement
    bool inThisEnvirement(const QString &file) const;

    int size() const;
    QString concatEnv() const;

    static QStringList recursiveInvairement(QDir &dir, int depch, int depchLimit = -1);
    static QStringList recursiveInvairement(const QString &dir, int depch = -1);
};

#endif // ENVIREMENT_H
