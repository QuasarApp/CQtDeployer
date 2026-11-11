#ifndef DEB_H
#define DEB_H

#include "idistribution.h"

/**
 * @brief The DebInOut struct contains input and output value of the debian packages.
 */
struct DebInOut {
    QString input;
    QString output;
};

/**
 * @brief The deb class contains methods for create a debian package.
 */
class DEPLOYSHARED_EXPORT Deb: public iDistribution
{
public:
    Deb(FileManager *fileManager);

    // iDistribution interface
public:
    bool deployTemplate(PackageControl &pkgCtrl) override;
    bool deployRawTemplate(PackageControl &pkgCtrl) override;

    bool removeTemplate() const override;
    Envirement toolKitEnv() const override;
    QProcessEnvironment processEnvirement() const override;
    QList<SystemCommandData> runCmd() override;
    QStringList outPutFiles() const override;

    // iDistribution interface
protected:
    QString dataLocation(const DistroModule &module) const override;
    QString location(const DistroModule &module) const override;
    QString releativeLocation(const DistroModule &module) const override;

private:
    /**
     * @brief addLicense This method inserts the license file of the
     * -license option into the debian package (issue #456). The license
     * file is copied into the standard location of the license text of the
     * Debian policy: usr/share/doc/<package>/copyright.
     * @param packageLocation This is the root directory of the debian package.
     * @param packageName This is the name of the debian package.
     * @return true if the license was inserted successful (or the option is not set).
     */
    bool addLicense(const QString &packageLocation, const QString &packageName) const;

    QList<DebInOut> inouts;
};

#endif // DEB_H
