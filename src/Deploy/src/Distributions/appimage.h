#ifndef APPIMAGE_H
#define APPIMAGE_H

#include "idistribution.h"

/**
 * @brief The AppImageInOut struct contains the input (the AppDir) and the
 *  output (the created AppImage) value of one AppImage package.
 */
struct AppImageInOut {
    QString appDir;
    QString output;
    bool needArch = false;
};

/**
 * @brief The AppImage class contains methods for create AppImage packages
 *  (the "appimage" option, issue #898).
 *
 * The backend is modeled on the backends of the "zip" and the "deb"
 * options (issue #898): for every package the AppDir of the root layout
 * is created (the AppRun launch script, the <name>.desktop file, the icon
 * and the deployed data), the appimagetool packs the AppDir into the
 * final <name>.AppImage file of the target dir.
 *
 * The appimagetool is bundled within the CQtDeployer package (the
 * AppImage folder next to the executable, see the copyAppImage target of
 * src/CQtDeployer/CMakeLists.txt), much like the Qt Installer Framework
 * tools (issue #867). When the tool is already installed on the system
 * (for example as the "appimage" package of the Debian distributions)
 * the system tool is used, because the system PATH is searched first.
 */
class DEPLOYSHARED_EXPORT AppImage: public iDistribution
{
public:
    AppImage(FileManager *fileManager);

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
    QList<AppImageInOut> inouts;
};

#endif // APPIMAGE_H
