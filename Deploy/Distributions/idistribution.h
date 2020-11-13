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
     * @brief collectInfo This method collect information about package.
     * @param it This is iterator of package.
     * @param cfg This is pointer to config.
     * @param info This is return value (created template information)
     * @param fDefaultPakcage This is return value
     * (return true if package that collected information is a default package)
     * @return true if information collected successful.
     */
    bool collectInfo(const QHash<QString, DistroModule>::const_iterator &it,
                     const DeployConfig *cfg,
                     TemplateInfo& info,
                     bool &fDefaultPakcage);

    /**
     * @brief dataLocation This method should be retrun location of application or package files.
     * @param packageName This is name of package that request data location.
     *  @return return location of data files.
     */
    virtual QString dataLocation(const QString& packageName) const = 0;

    /**
     * @brief location This method return main patho to files of package.
     * @param packageName This is name of package that request data location.
     * @return location of package.
     */
    virtual QString location(const QString& packageName) const = 0;

private:

    QString _location = "Temp Template";
    FileManager * _fileManager = nullptr;

};

#endif // IDISTRIBUTION_H
