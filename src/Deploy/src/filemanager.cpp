/*
 * Copyright (C) 2018-2026 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */



#include "filemanager.h"
#include <QDebug>
#include <QDir>
#include <quasarapp.h>
#include "defines.h"
#include "deployconfig.h"
#include "deploycore.h"
#include <QProcess>
#include <QStack>
#include <fstream>
#include "pathutils.h"
#include <QRegularExpression>
#include <cmath>
#include <LIEF/PE.hpp>

#ifdef Q_OS_WIN
#include "windows.h"
#endif

FileManager::FileManager() {
}

bool FileManager::initDir(const QString &path) {

    QString workPath = path;

    if (!QFileInfo::exists(workPath)) {

        QStack<QString> toInitDirs;
        while (!QFile::exists(workPath)) {
            QString dirName = PathUtils::popItem(workPath);
            toInitDirs.push(dirName);

        };

        if (toInitDirs.isEmpty())
            return false;

        QDir dir(workPath);

        while (toInitDirs.size()) {
            QString dirName = toInitDirs.pop();

            // The directory might have been created by another process in
            // parallel (for example by another cqtdeployer instance that
            // deploys into a sibling folder with the same missing parent
            // dir). QDir::mkdir fails in this case, so do not treat a
            // failed mkdir as an error when the directory exists now.
            // The cd() call below rejects a path that is not a directory.
            if (!dir.mkdir(dirName) && !dir.exists(dirName)) {
                return false;
            }

            if (!dir.cd(dirName)) {
                return false;
            }

            if (!addToDeployed(dir.absolutePath())) {
                return false;
            }
        }
    }

    return true;
}


QSet<QString> FileManager::getDeployedFiles() const {
    return _deployedFiles;
}

QStringList FileManager::getDeployedFilesStringList() const {
    return _deployedFiles.values();
}

void FileManager::loadDeployemendFiles(const QString &targetDir) {
    auto settings = QuasarAppUtils::Settings::instance();

    if (targetDir.isEmpty())
        return;

    QStringList deployedFiles = settings->getValue(targetDir, "").toStringList();

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    _deployedFiles.unite(deployedFiles.toSet());
#else
    _deployedFiles.unite(QSet<QString>(deployedFiles.begin(), deployedFiles.end()));
#endif
}


bool FileManager::addToDeployed(const QString& path) {
    auto info = QFileInfo(path);
    if (info.exists()) {
        _deployedFiles += info.absoluteFilePath();
        if (!QFile::setPermissions(path, static_cast<QFile::Permission>(0x7775))) {
            qWarning() << "Failed to set permissions";
        }

#ifdef Q_OS_WIN

        if (info.isFile()) {
            auto stdString = QDir::toNativeSeparators(info.absoluteFilePath()).toStdString();

            DWORD attribute = GetFileAttributesA(stdString.c_str());
            if (!SetFileAttributesA(stdString.c_str(), attribute & static_cast<DWORD>(~FILE_ATTRIBUTE_HIDDEN))) {
                qWarning() << "Failed to set permissions";
            }
        }
#endif
    }

    return true;
}

void FileManager::removeFromDeployed(const QString &path) {

    _deployedFiles -= path;
}

void FileManager::saveDeploymendFiles(const QString& targetDir) {
    if (targetDir.isEmpty())
        return;

    auto settings = QuasarAppUtils::Settings::instance();
    settings->setValue(targetDir, getDeployedFilesStringList());
}

bool FileManager::strip(const QString &dir) const {

    QFileInfo info(dir);

    // Never follow symbolic links. The deploy directory may contain links to
    // the system Qt installation (deploySystem=false mode), and `strip`
    // rewrites its target in place (the same inode is reused). Stripping
    // through such a link would destroy the host Qt libraries while this
    // very process is mapped against them, corrupting the running binary and
    // causing a segfault in QProcess (issue #166).
    if (info.isSymLink()) {
        return true;
    }

    if (!info.exists()) {
        qDebug() << QString("Directory %0 does not exist!").arg(dir);
        return false;
    }

    if (info.isDir()) {
        QDir d(dir);
        auto list = d.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden);

        bool res = false;
        for (const auto &i : list) {
            res = strip(i.absoluteFilePath()) || res;
        }

        return res;
    } else {

#ifdef Q_OS_WIN
        // There is no `strip` tool on Windows, so the PE binaries are
        // stripped in-process with LIEF (the debug information is removed
        // and the binary is rebuilt).
        const auto sufix = info.completeSuffix();
        if (!sufix.contains("dll") && !sufix.contains("exe")) {
            return true;
        }

        auto pe = LIEF::PE::Parser::parse(info.absoluteFilePath().toStdString());
        if (pe == nullptr) {
            qCritical() << "Unable to parse PE file for strip: " + dir;
            return false;
        }

        if (!pe->has_debug()) {
            return true;
        }

        pe->clear_debug();

        LIEF::PE::Builder builder(*pe, LIEF::PE::Builder::config_t());
        if (!LIEF::is_ok(builder.build())) {
            qCritical() << "Unable to rebuild stripped PE file: " + dir;
            return false;
        }

        builder.write(info.absoluteFilePath().toStdString());
        return true;
#else
        auto sufix = info.completeSuffix();
        // macOS dylibs and framework binaries (no file suffix) are
        // stripped as well. On a macOS host the system strip tool
        // supports the Mach-O format (issue #166).
        if (!sufix.contains("so") && !sufix.contains("dll")
                && !sufix.contains("dylib", Qt::CaseInsensitive)
                && !DeployCore::isMacOSFrameworkBinary(info)) {
            return true;
        }

        QProcess P;
        P.setProgram("strip");
        P.setArguments(QStringList() << info.absoluteFilePath());
        P.start();

        if (!P.waitForStarted())
            return false;
        if (!P.waitForFinished())
            return false;

        return P.exitCode() == 0;
#endif
    }
}


bool FileManager::fileActionPrivate(const QString &file, const QString &target,
                                    QStringList *masks, bool isMove, bool targetIsFile) {
    
    auto info = QFileInfo(file);
    
    bool copy = !masks;
    if (masks) {
        for (const auto &mask : std::as_const(*masks)) {
            if (info.absoluteFilePath().contains(mask, DeployCore::getCaseSensitivity())) {
                copy = true;
                break;
            }
        }
    }

    QString operation = (isMove)? "move :": "copy";

    if (!copy) {
        qDebug() << "Skip " + operation + " (by mask) :" + file;
        return true;
    }

    auto name = info.fileName();
    QString tergetFile = target + QDir::separator() + name;
    if (targetIsFile) {
        tergetFile = target;
    }

    info.setFile(tergetFile);

    if (!initDir(info.absolutePath())) {
        return false;
    }

    if (QFileInfo(file).absoluteFilePath() ==
            QFileInfo(tergetFile).absoluteFilePath()) {
        return true;
    }

    if (!QuasarAppUtils::Params::isEndable("noOverwrite") &&
            info.exists() && !removeFile( tergetFile)) {
        return false;
    }

    if (isMove) {
        qDebug() << operation + " :" + file;
    } else {
        qInfo() << operation + " :" + file;
    }

    // Symbolic links are handled separately from a regular file copy:
    // - a link to a directory must be recreated as a link, because a
    //   directory content cannot be copied as a file;
    // - on macOS the links of the framework bundles (Versions/Current -> A)
    //   must be kept, otherwise dyld cannot resolve the bundle (issue #166);
    // - on Linux the deployed bundle must be self-contained (issue #825):
    //   a symlinked library (for example the CUDA
    //   libcudart.so.12 -> libcudart.so.12.3.10 pair) is copied as a
    //   regular file with the content of the resolved target, because a
    //   created link would store a target that points to the source
    //   machine and dangles as soon as the app is run on another host.
    if (!isMove && !QFileInfo::exists(tergetFile)) {
        auto sourceInfo = QFileInfo(file);
        if (sourceInfo.isSymLink()) {
            // symLinkTarget() returns the absolute target (Qt 6).
            auto linkTarget = sourceInfo.symLinkTarget();

            // When the target is inside the source directory it is stored
            // relative to the created link, so the deployed bundle stays
            // self-contained (issue #825).
            auto storedTarget = linkTarget;
            const auto linkDir = sourceInfo.absolutePath() + QDir::separator();
            if (!linkTarget.isEmpty() && linkTarget.startsWith(linkDir)) {
                storedTarget = linkTarget.mid(linkDir.size());
            }

#ifdef Q_OS_MACOS
            // macOS framework bundles contain symlinks (Versions/Current ->
            // A). The link itself must be kept, otherwise dyld cannot
            // resolve the bundle (issue #166).
            if (QFile::link(storedTarget, tergetFile)) {
                qInfo() << operation + " (link) :" + file;
                addToDeployed(tergetFile);
                return true;
            }

            qWarning() << "Failed to create the symlink " + tergetFile +
                    " -> " + storedTarget +
                    ", fallback to a regular file copy";
#else
            const auto targetInfo = QFileInfo(linkTarget);
            if (!targetInfo.isFile()) {
                // A link to a directory or a broken link is recreated as a
                // link, the regular copy below cannot follow it.
                if (QFile::link(storedTarget, tergetFile)) {
                    qInfo() << operation + " (link) :" + file;
                    addToDeployed(tergetFile);
                    return true;
                }

                qWarning() << "Failed to create the symlink " + tergetFile +
                        " -> " + storedTarget +
                        ", fallback to a regular file copy";
            } else {
                // The regular copy below follows the symlink and copies the
                // content of the resolved target, so the deployed library is
                // a self-contained regular file (issue #825).
                qDebug() << "Skip the symlink " + file + " -> " + linkTarget +
                        ", copy the target content instead";
            }
#endif
        }
    }

    QFile sourceFile(file);
    auto sourceFileAbsalutePath = QFileInfo(file).absoluteFilePath();

    bool tarExits = QFileInfo::exists(tergetFile);
    if (tarExits && !QuasarAppUtils::Params::isEndable("noOverwrite")) {
        qInfo() << tergetFile + " already exists!";
        return true;
    }

    if (!((isMove)?
          sourceFile.rename(tergetFile):
          sourceFile.copy(tergetFile))) {

        qCritical() << "Failed to " + operation + " from " + file + " to " + tergetFile;

        qCritical() << sourceFile.errorString();
        return false;
    }

    if (!isMove) {
        // QFile::copy() creates the target file with the default
        // permissions (on Unix the executable bit is lost). Restore the
        // source permissions so that deployed executables, e.g. the
        // bundled QIF/binarycreator, stay executable (issue #872).
        QFile targetFile(tergetFile);
        if (!targetFile.setPermissions(QFileInfo(file).permissions())) {
            qWarning() << "Failed to set permissions for " + tergetFile;
        }
    }

    if (isMove) {
        removeFromDeployed(sourceFileAbsalutePath);
    }

    addToDeployed(tergetFile);
    return true;
}

bool FileManager::removeFile(const QString &file) {
    return removeFile(QFileInfo (file));
}

bool FileManager::smartCopyFile(const QString &file,
                                const QString &target,
                                QStringList *mask,
                                bool ifFileTarget) {
    auto config = DeployCore::_config;

    // macOS: a library that lives inside a .framework bundle must be
    // deployed as the whole bundle, because dyld requires the
    // Name.framework/Versions/<V>/Name layout (issue #166).
    auto frameworkRoot = DeployCore::macOSFrameworkRoot(file);
    if (!frameworkRoot.isEmpty()) {
        auto targetFw = target + QDir::separator() + QFileInfo(frameworkRoot).fileName();

        if (file.contains(config->getTargetDir(), DeployCore::getCaseSensitivity())) {
            if (moveFolder(frameworkRoot, targetFw)) {
                return true;
            }

            qDebug() << "Failed to move the framework. Trying to copy it";
        }

        if (!copyFolder(frameworkRoot, targetFw, {}, nullptr, mask, true)) {
            qCritical() << "Failed to copy the framework " + frameworkRoot;
            return false;
        }

        return true;
    }

    if (file.contains(config->getTargetDir(), DeployCore::getCaseSensitivity())) {
        if (!moveFile(file, target, mask)) {
            qDebug() << "Failed to move the file. Trying to copy it";

            if (!copyFile(file, target, mask, ifFileTarget)) {
                qCritical() << "Failed to copy the target file " + file;
                return false;
            }
        }
    } else {
        if (!copyFile(file, target, mask, ifFileTarget)) {
            qCritical() << "Failed to copy the target file " + file;
            return false;
        }
    }

    return true;
}

bool FileManager::moveFile(const QString &file, const QString &target, QStringList *masks, bool targetIsFile) {
    return fileActionPrivate(file, target, masks, true, targetIsFile);
}

bool FileManager::copyFolder(const QString &from,
                             const QString &to,
                             const QStringList &filter,
                             QStringList *listOfCopiedItems, QStringList *mask, bool force) {

    QDir fromDir(from);

    auto list = fromDir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries | QDir::Hidden);

    for (const auto &item : list) {
        if (item.isDir()) {

            if (!copyFolder(item.absoluteFilePath(), to + "/" + item.fileName(), filter, listOfCopiedItems, mask, force)) {
                return false;
            }

        } else {

            if (!force) {
                QString skipFilter = "";
                for (const auto &i: filter) {
                    if (item.fileName().contains(i, DeployCore::getCaseSensitivity())) {
                        skipFilter = i;
                        break;
                    }
                }

                if (!skipFilter.isEmpty()) {
                    qDebug() << 
                                item.absoluteFilePath() + " ignored by filter " + skipFilter;
                    continue;
                }
                auto config = DeployCore::_config;

                LibInfo info;
                info.setName(item.fileName());
                info.setPath(item.absolutePath());
                info.setPlatform(GeneralFile);

                if (config)
                    if (auto rule = config->ignoreList.isIgnore(info)) {
                        qDebug() << 
                                    item.absoluteFilePath() + " ignored by rule " + rule->label;
                        continue;
                    }
            }

            if (!copyFile(item.absoluteFilePath(), to , mask)) {
                qWarning() << 
                            "not copied file " + to + "/" + item.fileName();
                continue;
            }

            if (listOfCopiedItems) {
                *listOfCopiedItems << to + "/" + item.fileName();
            }
        }
    }

    return true;
}

bool FileManager::cp(const QString &from,
                     const QString &to,
                     const QStringList &filter,
                     QStringList *listOfCopiedItems,
                     QStringList *mask,
                     bool force) {

    QFileInfo info(from);
    if (!info.exists())
        return false;

    if (info.isDir()) {
        return copyFolder(from, to + "/" + PathUtils::getName(info.absoluteFilePath()),
                          filter, listOfCopiedItems, mask, force);
    }

    return copyFile(from, to, mask);
}

bool FileManager::moveFolder(const QString &from, const QString &to, const QString& ignore) {
    QFileInfo info(from);

    if (to.contains(from)) {
        return false;
    }

    if (!info.exists())
        return false;

    if (!initDir(to)) {
        return false;
    }

    if (info.isFile()) {

        if (ignore.size() && info.absoluteFilePath().contains(ignore)) {
            return true;
        }

        if (!moveFile(info.absoluteFilePath(), to)) {
            return false;
        }
        return true;
    }

    QDir dir(from);
    auto list = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden);
    for (const auto &i :list) {
        auto targetDir = to;
        if (i.isDir()) {
            targetDir += "/" + i.fileName();
        }

        if (!moveFolder(i.absoluteFilePath(), targetDir, ignore)) {
            return false;
        }
    }

    return true;
}

void FileManager::clear(const QString& targetDir, bool force) {
    qInfo() << "Clear start!";
    if (force) {
        qInfo() << "Force clear " + targetDir;
        if (QDir(targetDir).removeRecursively()) {
            return;
        }

        qWarning() << "Remove target Dir fail, try remove old deployemend files";
    }

    QMultiMap<int, QFileInfo> sortedOldData;
    for (const auto& i : std::as_const(_deployedFiles)) {
        sortedOldData.insert(i.size(), QFileInfo(i));
    }

    for (auto it = sortedOldData.end(); it != sortedOldData.begin(); --it) {

        auto index = std::prev(it);

        if (!index.value().exists()) {
            continue;
        }

        if (index.value().isFile()) {
            if (removeFile(index.value())) {
                qInfo() << "Remove " + index.value().absoluteFilePath() + " because it is deployed file";
            }

        } else {
            QDir qdir(index.value().absoluteFilePath());
            if (!qdir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries).count()) {
                qdir.removeRecursively();
                qInfo() << "Remove " + index.value().absoluteFilePath() + " because it is empty";
            }
        }
    }

    _deployedFiles.clear();
}

bool FileManager::copyFile(const QString &file, const QString &target,
                           QStringList *masks, bool targetIsFile) {

    return fileActionPrivate(file, target, masks, false, targetIsFile);
}

QString FileManager::changeDistanation(const QString& absalutePath,
                                       QString basePath,
                                       int depch) {
    QRegularExpression _matcher;
    _matcher.setPattern("[\\/]");
    auto prefixes = absalutePath.split(_matcher, splitbehavior);
    depch = std::min(static_cast<long long>(depch), static_cast<long long>(prefixes.size()));
    while (depch) {
        auto index = prefixes.size() - depch;
        if (index >= 0) {
            basePath += "/" + prefixes[index];
        }

        depch--;
    }

    return basePath;
}

bool FileManager::copyFiles(const QStringList &source,
                            const QString &to, int saveStructSize,
                            const QStringList &filter,
                            QStringList *listOfCopiedItems,
                            QStringList *mask) {

    for (const auto &item : source) {

        QFileInfo info(item);

        QString skipFilter = "";
        for (const auto &i: filter) {
            if (info.fileName().contains(i, DeployCore::getCaseSensitivity())) {
                skipFilter = i;
                break;
            }
        }

        if (!skipFilter.isEmpty()) {
            qDebug() << 
                        info.absoluteFilePath() + " ignored by filter " + skipFilter;
            continue;
        }
        auto config = DeployCore::_config;

        LibInfo libInfo;
        libInfo.setName(info.fileName());
        libInfo.setPath(info.absolutePath());
        libInfo.setPlatform(GeneralFile);

        if (config)
            if (auto rule = config->ignoreList.isIgnore(libInfo)) {
                qDebug() << 
                            info.absoluteFilePath() + " ignored by rule " + rule->label;
                continue;
            }

        auto distanation = changeDistanation(info.absolutePath(), to, saveStructSize);

        if (!copyFile(info.absoluteFilePath(), distanation , mask)) {
            qWarning() << 
                        "Failed to copy file " + distanation + "/" + info.fileName();
            continue;
        }

        if (listOfCopiedItems) {
            *listOfCopiedItems << distanation + "/" + info.fileName();
        }
    }


    return true;
}

bool FileManager::removeFile(const QFileInfo &file) {

    if (!file.exists()) {
        return true;
    }

    QFile f(file.absoluteFilePath());
    if (!f.remove()) {
        qCritical() << "Qt Operation fail (remove file) " + file.absoluteFilePath();

        qCritical() << "Message: " + f.errorString();

        return false;
    }

    return true;
}
