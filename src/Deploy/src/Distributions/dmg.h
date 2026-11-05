#ifndef DMG_H
#define DMG_H

#include "idistribution.h"

/**
 * @brief The DmgInOut struct contains the input (the root folder of the
 *  volume) and the output (the created DMG file) value of one DMG package.
 */
struct DmgInOut {
    QString input;
    QString output;
    QString volumeName;
};

/**
 * @brief The Dmg class contains methods for create DMG packages
 *  (the "dmg" option, issue #899).
 *
 * The backend is modeled on the backends of the "zip" and the "deb"
 * options: for every package the root folder of the DMG volume is
 * created (the deployed data of the package, the icon, the README file
 * with the name/description/version/publisher of the package and the
 * Applications symbolic link of the standard drag-to-install UX), the
 * hdiutil tool packs the root folder into the final <name>.dmg file of
 * the target dir.
 *
 * The hdiutil is a standard tool of the macOS system (the same way the
 * dpkg-deb tool of the deb backend is a standard tool of the Linux
 * system), so no bundled binaries are needed: the system PATH is
 * searched (the toolKitEnv method). The DMG packages can be created
 * only on the macOS host platforms (the isAvailable method), on the
 * other hosts the "dmg" option is skipped with a warning (the same
 * rule as the "deb" and the "appimage" options, issue #899).
 *
 * Note: the "dmg" option was an alias for the "qtifw" option before
 * issue #899 (issue #884), now it creates the real DMG packages with
 * the hdiutil tool.
 */
class DEPLOYSHARED_EXPORT Dmg: public iDistribution
{
public:
    Dmg(FileManager *fileManager);

    // iDistribution interface
public:
    bool deployTemplate(PackageControl &pkg) override;
    bool deployRawTemplate(PackageControl &pkgCtrl) override;
    bool removeTemplate() const override;
    Envirement toolKitEnv() const override;
    QProcessEnvironment processEnvirement() const override;
    QList<SystemCommandData> runCmd() override;
    QStringList outPutFiles() const override;
    bool isAvailable() const override;

    // iDistribution interface
protected:
    QString dataLocation(const DistroModule &module) const override;
    QString location(const DistroModule &module) const override;

private:
    QList<DmgInOut> inouts;
};

#endif // DMG_H
