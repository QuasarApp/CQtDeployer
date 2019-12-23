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

#define DISTRO_DIR QString("DistributionKit")

class  FileManager;
class  DependenciesScanner;

class DEPLOYSHARED_EXPORT ConfigParser
{
private:

    DeployConfig _config;
    FileManager *_fileManager;
    DependenciesScanner *_scaner;

    QHash<QString, QString> _Targetprefixes;

    bool createFromDeploy(const QString& file) const;
    bool loadFromFile(const QString& file);
    bool initDustroStruct();
    bool parseQtDeployMode();
    bool parseQtInfoMode();
    bool parseQtClearMode();

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

    void setExtraPath(const QStringList &value);
    void setExtraNames(const QStringList &value);

    void setExtraPlugins(const QStringList &value);

    QString recursiveInvairement(QDir &dir, int depch = 0, int depchLimit = -1);
    QString recursiveInvairement(const QString &dir, int depch);

    void initEnvirement();

    QStringList getDirsRecursive(const QString &path, int maxDepch = -1, int depch = 0);
    QSet<QString> getSetDirsRecursive(const QString &path, int maxDepch = -1, int depch = 0);

    QString getRelativeLink(const QString& from, const QString& to);
    void writeKey(const QString &key, QJsonObject &, const QString &confFileDir) const;
    void readKey(const QString &key, const QJsonObject &obj, const QString &confFileDir) const;
    QHash<QString, TargetInfo> prepareTarget(const QString &target);

    QString readKeyArray(int separatorLvl, const QJsonArray &array, const QString &confFileDir) const;
    QJsonValue writeKeyArray(int separatorLvl, const QString &parameter, const QString &confFileDir) const;
    QString findWindowsPath(const QString &path) const;
public:
    ConfigParser(FileManager *filemanager, DependenciesScanner *scaner);
    bool parseParams();
    bool smartMoveTargets();

    const DeployConfig* config() const;
    friend class deploytest;

};

#endif // CQT_H
