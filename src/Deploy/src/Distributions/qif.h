#ifndef QIF_H
#define QIF_H

#include "idistribution.h"

class PackageControl;
class DeployConfig;

/**
 * @brief The QIF class provides interface betvin deployment targets and Qt Installer Framework
 */
class DEPLOYSHARED_EXPORT QIF: public iDistribution
{
public:
    QIF(FileManager *fileManager);
    Envirement toolKitEnv() const override;
    QList<SystemCommandData> runCmd() override;
    bool deployTemplate(PackageControl &pkg) override;
    bool deployRawTemplate(PackageControl &pkgCtrl) override;
    bool removeTemplate() const override;
    QProcessEnvironment processEnvirement() const override;
    QStringList outPutFiles() const override;
    bool isAvailable() const override;

protected:
    QString dataLocation(const DistroModule &module) const override;
    QString location(const DistroModule &module) const override;

private:

    QString getStyle(const QString &input) const;
    QString installerFile() const;

    /**
     * @brief deployPackage - private method for deploy package of qt installer framework
     * @param dist - this is  DistroModule.
     * @param sufixes - this is sufixses of files for copy into package
     * @param pakcagesTemplates - this is list of pakcages and them tempalte patheses
     * @param defaultPackageTempalte this is path to default package template
     * @param pkg this is PackageControl object for move a packge data.
     * @return return true if package deployed successful
     */
    bool deployPackage(const DistroModule &dist,
                       const QStringList sufixes,
                       const QHash<QString, QString> &pakcagesTemplates,
                       const QString &defaultPackageTempalte,
                       PackageControl &pkg);

    /**
     * @brief initDefaultConfiguratuin This method initialise the default configuration of installer.
     * @return true if functions finished successful.
     */
    bool initDefaultConfiguratuin();

    /**
     * @brief addLicense This method inserts the license file of the
     * -license option into the Qt Installer Framework installer (issue #456).
     * The license file is copied into the meta directory of every package of
     * the installer template and the Licenses element is added to the
     * package.xml of the package, so the installer shows the license
     * agreement before the installation starts.
     * @param pkg This is the PackageControl object with the packages of the deploy.
     * @return true if the license was inserted successful (or the option is not set).
     */
    bool addLicense(PackageControl &pkg) const;

    TemplateInfo generalInfo;
};

#endif // QIF_H
