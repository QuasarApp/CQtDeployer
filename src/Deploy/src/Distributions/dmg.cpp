/*
 * Copyright (C) 2026 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "dmg.h"

#include <deployconfig.h>
#include <distromodule.h>
#include <packagecontrol.h>
#include <pathutils.h>
#include <quasarapp.h>
#include <filemanager.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>

#ifdef Q_OS_UNIX
#include <unistd.h>
#endif

namespace {
    // The DMG stage is created in the "defaultDMGTemplate" folder of the
    // target directory and removed by the removeTemplate method.
    constexpr auto kDefaultTemplateLocation = "defaultDMGTemplate";

    // The Info.plist template of the application bundle (issue #166).
    constexpr auto kPlistTemplate =
            QLatin1String(":/Templates/DMG/Distributions/Templates/dmg");

    // The custom placeholders of the Info.plist template.
    constexpr auto kBundleIdPlaceHolder =
            QLatin1String("$CQT_BUNDLE_ID");
    constexpr auto kBundleExecutablePlaceHolder =
            QLatin1String("$CQT_BUNDLE_EXECUTABLE");
}

Dmg::Dmg(FileManager *fileManager) :
    iDistribution(fileManager)
{
    setLocation(kDefaultTemplateLocation);
}

bool Dmg::isAvailable() const {
#ifdef Q_OS_MACOS
    auto hdiutil = QStandardPaths::findExecutable("hdiutil");
    return !hdiutil.isEmpty();
#else
    return false;
#endif
}

bool Dmg::deployTemplate(PackageControl &pkg) {
    const DeployConfig *cfg = DeployCore::_config;

    // A separate DMG archive is created for every available package,
    // the same way the deb distribution creates a separate .deb file
    // for every available package (issue #166).
    auto list = pkg.availablePackages();
    for (const auto &key: list) {
        auto module = cfg->getDistroFromPackage(key);

        TemplateInfo info;
        if (!collectInfo(module, info)) {
            return false;
        }

        auto stageDir = location(module);
        auto bundleDir = stageDir + "/" + bundleName(module) + ".app";
        auto dataLoc = dataLocation(module);

        // The bundle works on a copy of the deployed package, the
        // original package in the target directory is not changed
        // (the deployer does not rewrite the binaries, issue #166).
        if (!pkg.copyPackage(key, dataLoc)) {
            return false;
        }

        const auto executableName = Dmg::executableName(module);
        const auto hasExecutable = !module.targets().isEmpty();
        auto executable = dataLoc + "/" + executableName;

        // The CFBundleExecutable key must be the name of the executable
        // file directly in the MacOS subfolder of the bundle (issue #166).
        // The default macOS deploy layout keeps the executable in the
        // package root, so on the custom binOut layout the executable is
        // moved to the MacOS root of the bundle (on the bundle copy only).
        if (hasExecutable) {
            auto inPackage = dataLoc + module.getBinOutDir() + executableName;
            if (inPackage != executable &&
                !moveToBundleRoot(inPackage, executable)) {
                return false;
            }
        }

        if (!moveQConf(dataLoc, module)) {
            return false;
        }

        info.Custom[kBundleIdPlaceHolder] =
                bundleIdentifier(bundleName(module));
        info.Custom[kBundleExecutablePlaceHolder] =
                hasExecutable ? executableName : QString();

        if (!unpackFile(QFileInfo(kPlistTemplate + "/Contents/Info.plist"),
                        bundleDir + "/Contents", info, {"plist"})) {
            return false;
        }

        if (!createApplicationsLink(stageDir)) {
            return false;
        }

        // The DMG output name is the per-package "dmgOut" value or the
        // default "<name>.dmg" file name (the same way as the "debOut"
        // value of the deb distribution, issue #166).
        _stages.push_back({
            stageDir,
            executable,
            bundleName(module),
            cfg->getTargetDir() + "/" + info.dmgOut,
            rpaths(module)
        });
    }

    return true;
}

bool Dmg::deployRawTemplate(PackageControl &pkg) {
    const DeployConfig *cfg = DeployCore::_config;

    // The raw template is extracted into the bundle layout of every
    // available package (the Info.plist file of the application bundle,
    // the same way the raw template of the deb distribution is extracted
    // into the layout of every package, issue #166).
    auto list = pkg.availablePackages();
    for (const auto &key: list) {
        auto module = cfg->getDistroFromPackage(key);

        const auto bundleDir = location(module) + "/" + bundleName(module) + ".app";
        if (!unpackDir(kPlistTemplate, bundleDir, {}, {})) {
            return false;
        }
    }

    return true;
}

bool Dmg::removeTemplate() const {
    const DeployConfig *cfg = DeployCore::_config;

    registerOutFiles();
    return QDir(cfg->getTargetDir() + "/" + getLocation()).removeRecursively();
}

Envirement Dmg::toolKitEnv() const {
    Envirement result;
    result.addEnv(QProcessEnvironment::systemEnvironment().value("PATH"));

    return result;
}

QProcessEnvironment Dmg::processEnvirement() const {
    return QProcessEnvironment::systemEnvironment();
}

QList<SystemCommandData> Dmg::runCmd() {
    QList<SystemCommandData> res;

    for (const auto &stage: std::as_const(_stages)) {

        // The bundle executable keeps the build time install names
        // (@rpath/...) and the Qt libraries are deployed in the MacOS
        // folder of the bundle, so the @executable_path rpath makes the
        // @rpath references resolvable (issue #166). The rpath is added
        // to the bundle copy only, the original deployed binary is not
        // rewritten.
        if (QFileInfo(stage.executable).exists()) {
            for (const auto &rpath: stage.rpaths) {
                res.push_back({"install_name_tool",
                               {"-add_rpath", rpath, stage.executable}});
            }
        }

        res.push_back({"hdiutil", {"create",
                                   "-volname", stage.volumeName,
                                   "-srcfolder", stage.stageDir,
                                   "-ov",
                                   "-format", "UDZO",
                                   stage.output}});
    }

    return res;
}

QStringList Dmg::outPutFiles() const {
    QStringList res;
    for (const auto &stage: std::as_const(_stages)) {
        res.append(stage.output);
    }

    return res;
}

QString Dmg::dataLocation(const DistroModule &module) const {
    const DeployConfig *cfg = DeployCore::_config;

    return cfg->getTargetDir() + "/" + getLocation() + "/"
            + module.key() + "/" + bundleName(module) + ".app/Contents/MacOS";
}

QString Dmg::location(const DistroModule &module) const {
    const DeployConfig *cfg = DeployCore::_config;

    return cfg->getTargetDir() + "/" + getLocation() + "/" + module.key();
}

QString Dmg::bundleName(const DistroModule &module) const {
    auto targets = module.targets();
    if (targets.size()) {
        return QFileInfo(*targets.begin()).baseName();
    }

    return getName(module);
}

QString Dmg::executableName(const DistroModule &module) const {
    auto targets = module.targets();
    if (targets.size()) {
        return QFileInfo(*targets.begin()).fileName();
    }

    return getName(module);
}

QString Dmg::bundleIdentifier(const QString &bundleName) const {
    QString id = "org.quasarapp.";
    for (const auto &ch: bundleName.toLower()) {
        if (ch.isLetterOrNumber()) {
            id += ch;
        }
    }

    return id;
}

QStringList Dmg::rpaths(const DistroModule &module) const {
    QStringList res = {"@executable_path"};

    // On the custom libOut layout the Qt libraries are deployed in a
    // subfolder of the MacOS folder of the bundle, so the @rpath
    // references of the executable are resolved with the additional
    // rpath (issue #166).
    auto libOutDir = module.getLibOutDir();
    if (libOutDir != "/") {
        if (libOutDir.endsWith('/')) {
            libOutDir.chop(1);
        }
        res << "@executable_path" + libOutDir;
    }

    return res;
}

bool Dmg::moveToBundleRoot(const QString &from, const QString &to) const {
    if (!QFileInfo::exists(from)) {
        qCritical() << "The executable file is not found in the package: " + from;
        return false;
    }

    if (!QFile::rename(from, to)) {
        QuasarAppUtils::Params::log("Failed to move the executable file "
                                    "to the application bundle: " + from,
                                    QuasarAppUtils::Error);
        return false;
    }

    return true;
}

bool Dmg::moveQConf(const QString &dataLoc, const DistroModule &module) const {
    auto src = dataLoc + module.getBinOutDir() + "qt.conf";
    auto dst = dataLoc + "/qt.conf";

    if (src == dst) {
        // The default flat deploy layout: the qt.conf file is already
        // next to the executable in the MacOS root of the bundle.
        return true;
    }

    if (!QFileInfo::exists(src)) {
        return true;
    }

    QFile file(src);
    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "Failed to open the qt.conf file: " + src;
        return false;
    }

    // The qt.conf file is moved to the MacOS root of the bundle (next
    // to the bundle executable), so the Prefix key must point to the
    // package root (the MacOS folder) instead of the bin folder
    // (issue #166).
    auto lines = QString::fromUtf8(file.readAll()).split('\n');
    file.close();

    for (auto &line: lines) {
        if (line.startsWith("Prefix=")) {
            line = "Prefix= ./";
        }
    }

    QFile out(dst);
    if (!out.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qCritical() << "Failed to create the qt.conf file in the bundle: " + dst;
        return false;
    }

    out.write(lines.join('\n').toUtf8());
    out.close();

    return QFile::remove(src);
}

bool Dmg::createApplicationsLink(const QString &stageDir) const {
#ifdef Q_OS_UNIX
    auto link = stageDir + "/Applications";

    QFile::remove(link);
    if (::symlink("/Applications", link.toLocal8Bit().constData()) != 0) {
        qCritical() << "Failed to create the Applications link in " + link;
        return false;
    }

    return true;
#else
    qCritical() << "The symbolic links are not supported on this platform.";
    return false;
#endif
}

