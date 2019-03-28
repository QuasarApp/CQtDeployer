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
#include <windependenciesscanner.h>
#include "deploy_global.h"

class DEPLOYSHARED_EXPORT Deploy {
  private:
    bool deployQml = false;
    bool onlyCLibs = false;
    int depchLimit = 0;
    QStringList deployedFiles;

    QSettings settings;
    QString qmlScaner = "";
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

    QStringList QtLibs;
    QStringList noQTLibs;
    QStringList qmlLibs;
    QStringList neededPlugins;
    QStringList ignoreList;
    QStringList extraPlugins;

    QString appDir;

    WinDependenciesScanner winScaner;

    bool fileActionPrivate(const QString &file, const QString &target,
                           QStringList *mask, bool isMove);

    void copyFiles(const QStringList &files);
    bool copyFile(const QString &file, const QString &target,
                  QStringList *mask = nullptr);
    /**
     * @brief smartCopyFile
     * @param file
     * @param target
     * @param mask
     * @return if file in target dir try move file else copy
     */
    bool smartCopyFile(const QString &file, const QString &target,
                       QStringList *mask = nullptr);

    bool moveFile(const QString &file, const QString &target,
                  QStringList *mask = nullptr);
    void extract(const QString &file, bool isExtractPlugins = true);
    QString recursiveInvairement(int depch, QDir &dir);
    void extractPlugins(const QString &lib);
    bool copyPlugin(const QString &plugin);
    void copyPlugins(const QStringList &list);
    bool copyTranslations(QStringList list);

    bool copyFolder(const QString &from, const QString &to,
                    const QStringList &filter = QStringList(),
                    QStringList *listOfCopiedItems = nullptr,
                    QStringList *mask = nullptr);

    bool extractQml();

    bool strip(const QString &dir);

    QStringList extractImportsFromDir(const QString &dirpath);
    QFileInfoList findFilesInsideDir(const QString &name, const QString &dirpath);
    bool extractQmlAll();
    bool extractQmlFromSource(const QString &sourceDir);
    QString filterQmlPath(const QString &path);
    void extractLinuxLib(const QString & file, bool isExtractPlugins);
    void extractWindowsLib(const QString & file, bool isExtractPlugins);

    void addEnv(const QString& dir);
    QString concatEnv() const;
    bool smartMoveTargets();
    bool isLib(const QFileInfo &file);
    bool setBinDir(const QString& dir, bool recursive = false);

    bool initDir(const QString &path);
    void setTargetDir();
    bool deployMSVC();

public:
    Deploy();
    void initEnvirement();

    bool getDeployQml() const;
    void setDeployQml(bool value);

    QString getQmlScaner() const;
    void setQmlScaner(const QString &value);
    QString getQmake() const;
    void setQmake(const QString &value);
    bool setTargets(const QStringList &value);

    bool setTargetsRecursive(const QString& dir);

    bool createRunScript(const QString &target);
    void createQConf();

    void deploy();
    QString getQtDir() const;
    void setQtDir(const QString &value);

    void clear();
    void setOnlyCLibs(bool value);
    void setExtraPath(const QStringList &value);
    void setExtraPlugins(const QStringList &value);
    void setDepchLimit(int value);

    friend class deploytest;
};

#endif // DEPLOY_H
