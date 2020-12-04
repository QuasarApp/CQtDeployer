#ifndef IDISTRIBUTION_H
#define IDISTRIBUTION_H

#include "distrostruct.h"
#include "envirement.h"
#include "templateinfo.h"
#include <QProcess>
#include <QHash>
#include <QFileInfo>

class FileManager;
class DistroModule;
class PackageControl;
class DeployConfig;

struct SystemCommandData {
    QString command;
    QStringList arguments;
};

class DEPLOYSHARED_EXPORT iDistribution
{
public:
    iDistribution(FileManager * fileManager);
    virtual ~iDistribution();

    virtual bool deployTemplate(PackageControl& pkgCtrl) = 0;
    virtual bool removeTemplate() const = 0;
    virtual Envirement toolKitEnv() const = 0;
    virtual QProcessEnvironment processEnvirement() const = 0;

    virtual QList<SystemCommandData> runCmd() = 0;
    virtual QStringList outPutFiles() const = 0;

    virtual bool cb() const;

    QString getClassName() const;

protected:
    QString getLocation() const;
    void setLocation(const QString &location);
    bool unpackFile(const QFileInfo& resource,
                    const QString& target,
                    const TemplateInfo& info,
                    const QStringList &sufixes) const;

    bool unpackDir(const QString& resource,
                   const QString& target,
                   const TemplateInfo& info,
                   const QStringList &sufixes,
                   const QHash<QString, QString> &folderNewNames = {}) const;

    bool moveData(const QString& from, const QString& to, const QString &ignore) const;
    bool copyFile(const QString& from, const QString& to, bool isFileTarget) const;
    bool copyDir(const QString &resource, const QString &target) const;

    void registerOutFiles() const;

    /**
     * @brief collectInfoWithDeployIcons This is wraper of the collectInfo and deployIcon methods.
     * @param pkg This is package object.
     * @param info This is return value (created template information)
     * @return true if information collected successful.
     */
    bool collectInfoWithDeployIcons(const DistroModule &pkg,
                                    TemplateInfo& info);

    /**
     * @brief collectInfo This method collect information about package.
     * @param pkg This is package object.
     * @param info This is return value (created template information)
     * @return true if information collected successful.
     */
    bool collectInfo(const DistroModule &pkg,
                     TemplateInfo& info);

    /**
     * @brief getName This method return name of package from pacakge key.
     * @param pkgKey This is pacakge.
     * @return package name.
     */
    QString getName(const DistroModule &pkgKey) const;

    /**
     * @brief deployIcon This method copy default or custom icon to the package.
     * @param info This is information about package.
     * @note This method change the icon field of the info object.
     * If packages support the icons then you need to invocke this method after the collectInfo method.
     * @param pkg This is package info
     * @return true if this method finished successful.
     */
    bool deployIcon(TemplateInfo &info, const DistroModule &pkg);

    /**
     * @brief dataLocation This method should be retrun location of application or package files.
     * @param module This is module with information about package.
     *  @return return location of data files.
     */
    virtual QString dataLocation(const DistroModule &module) const = 0;

    /**
     * @brief location This method return main patho to files of package.
     * @param module This is module with information about package.
     * @return location of package.
     */
    virtual QString location(const DistroModule &module) const = 0;

    /**
     * @brief releativeLocation This method must be return releative path after install.
     * Default implementation return prefix value. but for some packages types may need to change a prefix value.
     * @param module This is module with information about package.
     * @return releative prefix value.
     */
    virtual QString releativeLocation(const DistroModule &module) const;


private:

    QString _location = "Temp Template";
    FileManager * _fileManager = nullptr;

};

#endif // IDISTRIBUTION_H
