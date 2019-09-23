#ifndef CQT_H
#define CQT_H
#include "distrostruct.h"
#include "envirement.h"
#include "ignorerule.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QStringList>
#include <QMap>
#include <QDir>
#include "deploy_global.h"

#define DISTRO_DIR QString("DistributionKit")

class  FileManager;

struct DEPLOYSHARED_EXPORT DeployConfig {
    QString qmake = "";
    QString targetDir = "";
    QString qmlDir = "";
    int depchLimit = 0;
    bool deployQml = false;
    IgnoreRule ignoreList;
    QStringList extraPlugins;
    QString appDir;
    QString qtDir;
    QStringList extraPaths;
    /**
     * @brief targets
     * key - path
     * value - create wrapper
     */
    QMap<QString, bool> targets;
    Envirement envirement;
    DistroStruct distroStruct;
    QString translationDir;

};

class DEPLOYSHARED_EXPORT ConfigParser
{
private:

    DeployConfig _config;
    FileManager *_fileManager;
    bool createFromDeploy(const QString& file) const;
    bool loadFromFile(const QString& file);
    bool parseQtDeployMode();
    bool parseQtInfoMode();
    bool parseQtClearMode();

    void setTargetDir(const QString &target = "");
    bool setTargets(const QStringList &value);
    bool setTargetsRecursive(const QString &dir);
    bool setBinDir(const QString &dir, bool recursive = false);


    void initIgnoreList();
    void initIgnoreEnvList();

    void setQmake(const QString &value);
    void setQtDir(const QString &value);

    void setExtraPath(const QStringList &value);
    void setExtraPlugins(const QStringList &value);

    QString recursiveInvairement(int depch, QDir &dir);
    QString recursiveInvairement(int depch, const QString &dir);

    void initEnvirement();

    QStringList getDirsRecursive(const QString &path, int maxDepch = -1, int depch = 0);


    void writeKey(const QString &key, QJsonObject &) const;
    void readKey(const QString &key, const QJsonObject &obj) const;
public:
    ConfigParser(FileManager *filemanager);
    bool parseParams();
    bool smartMoveTargets();

    const DeployConfig* config() const;
    friend class deploytest;

};

#endif // CQT_H
