#ifndef CQT_H
#define CQT_H
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QStringList>
#include <QMap>
#include <QDir>


struct DeployConfig {
    QString qmake = "";
    QString targetDir = "";
    QString qmlDir = "";
    QString translationDir = "";
    QString externQmlScaner = "";
    int depchLimit = 0;
    bool deployQml = false;
    QStringList ignoreList;
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
};

/**
 * @brief The CQT class - it is wrapper for cqt file. qcn it is configuration file of deploying project.
 */
class CQT
{
private:

    DeployConfig _config;

    bool createFromDeploy() const;
    bool loadFromFile();
    bool parseQtDeployMode();
    bool parseQtInfoMode();
    bool parseQtClearMode();

    void setTargetDir(const QString &target);
    bool setTargets(const QStringList &value);
    bool setTargetsRecursive(const QString &dir);
    bool setBinDir(const QString &dir, bool recursive);


    void initIgnoreList();
    void initIgnoreEnvList();

    void setQmlScaner(const QString &value);
    void setQmake(const QString &value);
    void setQtDir(const QString &value);

    void setExtraPath(const QStringList &value);
    void setExtraPlugins(const QStringList &value);

    QString recursiveInvairement(int depch, QDir &dir);

    void initEnvirement();

    QStringList getDirsRecursive(const QString &path);
public:
    CQT();
    bool parseParams();

    DeployConfig config() const;
    void setConfig(const DeployConfig &config);
};

#endif // CQT_H
