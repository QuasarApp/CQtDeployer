#ifndef DEPLOYCONFIG_H
#define DEPLOYCONFIG_H
#include "deploy_global.h"

#include "distromodule.h"
#include "ignorerule.h"
#include "targetinfo.h"

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

class DEPLOYSHARED_EXPORT DeployConfig {

public:
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
     * @brief prefixes
     * key - prefix
     * value struvture of prefix
     * default prefix is empty value
     */
    QHash<QString, DistroModule> prefixes;

    /**
     * @brief envirement - envirement for find libraries
     */
    Envirement envirement;

    /**
     * @brief reset config file to default
     */
    void reset();
    QHash<QString, TargetInfo *> getTargetsListByFilter(const QString& filter);

    /**
     * @brief targetPath
     * @param target
     * @return path to deployement root dir of target.
     * If target value well be empty then return main root of all targets.
     */
    QString getTargetDir(const QString& target = "") const;

private:

    /**
     * @brief targetDir -  targe directory (this folder conteins all files of distrebution kit)
     */
    QString targetDir = "";

};


#endif // DEPLOYCONFIG_H
