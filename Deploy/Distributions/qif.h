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
    bool removeTemplate() const override;
    QProcessEnvironment processEnvirement() const override;
    QStringList outPutFiles() const override;

protected:
    QString dataLocation(const QString &packageName) const override;
    QString location(const QString &packageName) const override;

private:

    QString getStyle(const QString &input) const;
    QString installerFile() const;

    /**
     * @brief deployPackage - private method for deploy package of qt installer framework
     * @param it - this is const iterator of current DistroModule.
     * @param cfg - this is config pointer
     * @param sufixes - this is sufixses of files for copy into package
     * @param pakcagesTemplates - this is list of pakcages and them tempalte patheses
     * @param defaultPackageTempalte this is path to default package template
     * @param pkg this is PackageControl object for move a packge data.
     * @return return true if package deployed successful
     */
    bool deployPackage(const QHash<QString, DistroModule>::const_iterator &it,
                       const DeployConfig *cfg,
                       const QStringList sufixes,
                       const QHash<QString, QString> &pakcagesTemplates,
                       const QString &defaultPackageTempalte,
                       PackageControl &pkg);

    QString binarycreator;
    TemplateInfo generalInfo;


};

#endif // QIF_H
