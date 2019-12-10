//#
//# Copyright (C) 2018-2020 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef CQT_H
#define CQT_H
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

struct DEPLOYSHARED_EXPORT QtDir {
    QString libs;
    QString bins;
    QString libexecs;
    QString plugins;
    QString qmls;
    QString translations;
    QString resources;

    Platform qtPlatform;

    bool isQt(const QString &path) const;
};

struct DEPLOYSHARED_EXPORT Extra {
    QSet<QString> extraPaths;
    QSet<QString> extraPathsMasks;
    QSet<QString> extraNamesMasks;

    bool contains(const QString &path) const;

};

struct DEPLOYSHARED_EXPORT DeployConfig {
    /**
     * @brief targetDir -  targe directory (this folder conteins all files of distrebution kit)
     */
    QString targetDir = "";

    /**
     * @brief depchLimit - recursive search limit
     */
    int depchLimit = 0;

    /**
     * @brief deployQml - enable or disable deploing of qml files.
     */
    bool deployQml = false;

    /**
     * @brief ignoreList - list with ignore files
     */
    IgnoreRule ignoreList;

    /**
     * @brief extraPlugins - list with pathes of extra plugins or plugins names
     */
    QStringList extraPlugins;

    /**
     * @brief appDir - it is cqtdeployer library location for ignre cqtdeployr libraries
     */
    QString appDir;

    /**
     * @brief qtDir - conteins all qt pathes
     */
    QtDir qtDir;

    /**
     * @brief extraPaths - it is list with filters for extra pathes, files or libraries
     */
    Extra extraPaths;
    /**
     * @brief targets
     * key - path
     * value - create wrapper
     */
    QHash<QString, TargetInfo> targets;

    /**
     * @brief envirement - envirement for find libraries
     */
    Envirement envirement;

    /**
     * @brief reset config file to default
     */
    void reset();
    QHash<QString, TargetInfo *> getTargetsListByFilter(const QString& filter);
};

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
public:
    ConfigParser(FileManager *filemanager, DependenciesScanner *scaner);
    bool parseParams();
    bool smartMoveTargets();

    const DeployConfig* config() const;
    friend class deploytest;

};

#endif // CQT_H
