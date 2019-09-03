/*
 * Copyright (C) 2018-2019 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef DEPLOY_H
#define DEPLOY_H
#include <QDir>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <dependenciesscanner.h>
#include "deploy_global.h"
#include "qml.h"

class DEPLOYSHARED_EXPORT Deploy {
  private:
    bool deployQml = false;
    int depchLimit = 0;

    QSettings settings;
    QString externQmlScaner = "";

    QString qmake = "";
    /**
     * @brief targets
     * key - path
     * value - create wrapper
     */
    QMap<QString, bool> targets;
    QString targetDir = "";
    QString qmlDir = "";
    QString translationDir = "";

    QStringList deployEnvironment;

    QStringList neadedLibs;
    QStringList systemLibs;
    QStringList ignoreList;
    QStringList ignoreEnvList;
    QStringList extraPlugins;

    QString appDir;

    DependenciesScanner scaner;

    int find(const QString& str, const QStringList& list) const;

    void extract(const QString &file);
    QString recursiveInvairement(int depch, QDir &dir);
    bool copyPlugin(const QString &plugin);
    void copyPlugins(const QStringList &list);
    bool copyTranslations(QStringList list);

    bool createQConf();

    bool extractQml();


    QStringList extractImportsFromDir(const QString &dirpath);
    QFileInfoList findFilesInsideDir(const QString &name, const QString &dirpath);
    bool extractQmlAll();
    bool extractQmlFromSource(const QString &sourceDir);
    QString filterQmlPath(const QString &path);
    void extractLib(const QString & file);

    void addEnv(const QString& dir);
    QString concatEnv() const;
    bool smartMoveTargets();
    bool isLib(const QFileInfo &file);
    bool setBinDir(const QString& dir, bool recursive = false);

    bool deployMSVC();


public:
    Deploy();
    void initEnvirement();
    void initIgnoreEnvList();
    void initIgnoreList();


    QStringList getDirsRecursive(const QString& path);

    bool getDeployQml() const;
    void setDeployQml(bool value);

    QString getQmlScaner() const;
    void setQmlScaner(const QString &value);
    QString getQmake() const;
    void setQmake(const QString &value);
    bool setTargets(const QStringList &value);

    bool setTargetsRecursive(const QString& dir);

    bool createRunScript(const QString &target);

    void deploy();
    QString getQtDir() const;
    void setQtDir(const QString &value);

    void clear(bool);
    void setExtraPath(const QStringList &value);
    void setExtraPlugins(const QStringList &value);
    void setDepchLimit(int value);
    void setTargetDir(const QString &target = "");


    friend class deploytest;
};

#endif // DEPLOY_H
