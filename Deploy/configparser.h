//#
//# Copyright (C) 2018-2020 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef CQT_H
#define CQT_H
#include "deployconfig.h"
#include "distrostruct.h"
#include "envirement.h"
#include "ignorerule.h"
#include "targetinfo.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QStringList>
#include <QMap>
#include <QDir>
#include "deploy_global.h"

class FileManager;
class DependenciesScanner;
class Packing;
class iDistribution;
class PluginsParser;

class DEPLOYSHARED_EXPORT ConfigParser
{
public:
    ConfigParser(FileManager *filemanager, PluginsParser* pluginsParser, DependenciesScanner *scaner, Packing* pac);
    bool parseParams();
    bool smartMoveTargets();

    const DeployConfig* config() const;
    friend class deploytest;

private:

    DeployConfig _config;
    FileManager *_fileManager;
    PluginsParser *_pluginsParser;
    DependenciesScanner *_scaner;
    Packing * _packing;

    QHash<QString, QString> _Targetpackages;

    bool createFromDeploy(const QString& file) const;
    bool loadFromFile(const QString& file);
    bool initDistroStruct();
    bool initPackages();
    bool parseDeployMode();
    bool parseInfoMode();
    bool parseInitMode();

    bool parseClearMode();

    QSet<QString> getQtPathesFromTargets();

    void setTargetDir(const QString &target = "");
    bool setTargets(const QStringList &value);
    bool setTargetsRecursive(const QString &dir);
    bool setBinDir(const QString &dir, bool recursive = false);

    void initIgnoreList();
    void initIgnoreEnvList();

    QString getPathFrmoQmakeLine(const QString& in) const;
    bool initQmakePrivate(const QString& qmake);
    bool initQmake();
    bool initQmlInput();

    bool setQmake(const QString &value);
    bool setQtDir(const QString &value);

    void initExtraPath();
    void initExtraNames();

    bool initPlugins();

    void initEnvirement();

    bool checkSnapPermisions();

    QStringList getDirsRecursive(const QString &path, int maxDepch = -1, int depch = 0);
    QSet<QString> getSetDirsRecursive(const QString &path, int maxDepch = -1, int depch = 0);

    QString getRelativeLink(const QString& from, const QString& to);
    void writeKey(const QString &key, QJsonObject &, const QString &confFileDir) const;
    void readKey(const QString &key, const QJsonObject &obj, const QString &confFileDir) const;
    void readString(const QString &key, const QString &val, const QString &confFileDir) const;

    QHash<QString, TargetInfo> createTarget(const QString &target);
    QHash<QString, TargetInfo> moveTarget(TargetInfo target, const QString &newLocation);


    QString readKeyArray(int separatorLvl, const QJsonArray &array, const QString &confFileDir) const;
    QJsonValue writeKeyArray(int separatorLvl, const QString &parameter, const QString &confFileDir) const;
    QString findWindowsPath(const QString &path) const;

    QList<iDistribution *> getDistribution();

    bool isNeededQt() const;
};

#endif // CQT_H
