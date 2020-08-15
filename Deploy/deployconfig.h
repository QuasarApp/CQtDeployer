#ifndef DEPLOYCONFIG_H
#define DEPLOYCONFIG_H
#include "deploy_global.h"

#include "distromodule.h"
#include "extra.h"
#include "ignorerule.h"
#include "qtdir.h"
#include "targetinfo.h"

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

    /**
     * @brief setTargetDir
     * @param target
     */
    void setTargetDir(const QString& target);

    /**
     * @brief getDistro
     * @param target
     * @return
     */
    DistroModule getDistro(const QString& target) const;

    /**
     * @brief getDistroFromPackage
     * @param package
     * @return distro struct from package
     */
    DistroModule getDistroFromPackage(const QString& package) const;

    const QHash<QString, TargetInfo>& targets() const;
    const QHash<QString, DistroModule>& packages() const;

    QMultiHash<QString, TargetInfo> &targetsEdit();
    QHash<QString, DistroModule>& packagesEdit();

    Platform getPlatform(const QString& package) const;

    QString getDefaultPackage() const;
    void setDefaultPackage(const QString &value);

private:

    /**
     * @brief targets
     * key - path
     * value - create wrapper
     */
    QMultiHash<QString, TargetInfo> _targets;

    /**
     * @brief packages
     * key - package
     * value struvture of package
     * default package is empty value
     */
    QHash<QString, DistroModule> _packages;

    /**
     * @brief targetDir -  targe directory (this folder conteins all files of distrebution kit)
     */
    QString targetDir = "";
    QString defaultPackage = "";


};


#endif // DEPLOYCONFIG_H
