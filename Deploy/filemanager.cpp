/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */



#include "filemanager.h"
#include <QDir>
#include <quasarapp.h>
#include "configparser.h"
#include "deploycore.h"
#include <QProcess>
#include <fstream>
#include "pathutils.h"

#ifdef Q_OS_WIN
#include "windows.h"
#endif

FileManager::FileManager() {
}

bool FileManager::initDir(const QString &path) {

    if (!QFileInfo::exists(path)) {
        if (!QDir().mkpath(path)) {
            return false;
        }
        addToDeployed(path);
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
    auto settings = QuasarAppUtils::Settings::get();

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
            QuasarAppUtils::Params::log("permishens set fail", QuasarAppUtils::Warning);
        }

#ifdef Q_OS_WIN

        if (info.isFile()) {
            auto stdString = QDir::toNativeSeparators(info.absoluteFilePath()).toStdString();

            DWORD attribute = GetFileAttributesA(stdString.c_str());
            if (!SetFileAttributesA(stdString.c_str(), attribute & static_cast<DWORD>(~FILE_ATTRIBUTE_HIDDEN))) {
                QuasarAppUtils::Params::log("attribute set fail", QuasarAppUtils::Warning);
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
    auto settings = QuasarAppUtils::Settings::get();
    settings->setValue(targetDir, getDeployedFilesStringList());
}

bool FileManager::strip(const QString &dir) const {

#ifdef Q_OS_WIN
    Q_UNUSED(dir)
    return true;
#else
    QFileInfo info(dir);

    if (!info.exists()) {
        QuasarAppUtils::Params::log("dir not exits!");
        return false;
    }

    if (info.isDir()) {
        QDir d(dir);
        auto list = d.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

        bool res = false;
        for (const auto &i : list) {
            res = strip(i.absoluteFilePath()) || res;
        }

        return res;
    } else {

        auto sufix = info.completeSuffix();
        if (!sufix.contains("so") && !sufix.contains("dll")) {
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
    }
#endif
}


bool FileManager::fileActionPrivate(const QString &file, const QString &target,
                                    QStringList *masks, bool isMove, bool targetIsFile) {
    
    auto info = QFileInfo(file);
    
    bool copy = !masks;
    if (masks) {
        for (auto mask : *masks) {
            if (info.absoluteFilePath().contains(mask, ONLY_WIN_CASE_INSENSIATIVE)) {
                copy = true;
                break;
            }
        }
    }

    if (!copy) {
        QuasarAppUtils::Params::log(((isMove)? "skip move :": "skip copy (by mask):" + file ));
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
        QuasarAppUtils::Params::log( "move :" + file,
                                    QuasarAppUtils::Debug);
    } else {
        QuasarAppUtils::Params::log("copy :" + file,
                                    QuasarAppUtils::Info);
    }


    QFile sourceFile(file);
    auto sourceFileAbsalutePath = QFileInfo(file).absoluteFilePath();

    bool tarExits = QFileInfo(tergetFile).exists();
    if (tarExits && !QuasarAppUtils::Params::isEndable("noOverwrite")) {
        QuasarAppUtils::Params::log(tergetFile + " already exists!",
                                    QuasarAppUtils::Info);
        return true;
    }

    if (!((isMove)?
          sourceFile.rename(tergetFile):
          sourceFile.copy(tergetFile))) {

        QuasarAppUtils::Params::log("Operation fail " + file + " >> " + tergetFile,
                                    QuasarAppUtils::Error);

        QuasarAppUtils::Params::log(sourceFile.errorString(),
                                    QuasarAppUtils::Error);
        return false;
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

    if (file.contains(config->getTargetDir(), ONLY_WIN_CASE_INSENSIATIVE)) {
        if (!moveFile(file, target, mask)) {
            QuasarAppUtils::Params::log(" file not moved! try copy");

            if (!copyFile(file, target, mask, ifFileTarget)) {
                QuasarAppUtils::Params::log("not copy target to bin dir " + file,
                                            QuasarAppUtils::Error);
                return false;
            }
        }
    } else {
        if (!copyFile(file, target, mask, ifFileTarget)) {
            QuasarAppUtils::Params::log("not copy target to bin dir " + file,
                                        QuasarAppUtils::Error);
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

    auto list = fromDir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);

    for (const auto &item : list) {
        if (item.isDir()) {

            if (!copyFolder(item.absoluteFilePath(), to + "/" + item.fileName(), filter, listOfCopiedItems, mask, force)) {
                return false;
            }

        } else {

            if (!force) {
                QString skipFilter = "";
                for (const auto &i: filter) {
                    if (item.fileName().contains(i, ONLY_WIN_CASE_INSENSIATIVE)) {
                        skipFilter = i;
                        break;
                    }
                }

                if (!skipFilter.isEmpty()) {
                    QuasarAppUtils::Params::log(
                                item.absoluteFilePath() + " ignored by filter " + skipFilter,
                                QuasarAppUtils::VerboseLvl::Debug);
                    continue;
                }
                auto config = DeployCore::_config;

                LibInfo info;
                info.setName(item.fileName());
                info.setPath(item.absolutePath());
                info.setPlatform(GeneralFile);

                if (config)
                    if (auto rule = config->ignoreList.isIgnore(info)) {
                        QuasarAppUtils::Params::log(
                                    item.absoluteFilePath() + " ignored by rule " + rule->label,
                                    QuasarAppUtils::VerboseLvl::Debug);
                        continue;
                    }
            }

            if (!copyFile(item.absoluteFilePath(), to , mask)) {
                QuasarAppUtils::Params::log(
                            "not copied file " + to + "/" + item.fileName(),
                            QuasarAppUtils::VerboseLvl::Warning);
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
        // This is qt bug, filename return emptu value if dir have the seporator on the end of path.
        // Qt 5.15.2
        auto last = from.right(1);
        if (last == "/" || last == "\\") {
            last = from.mid(0, from.size() -1 );
            info.setFile(last);
        }

        return copyFolder(from, to + "/" + info.fileName(),
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
    auto list = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
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
    QuasarAppUtils::Params::log( "clear start!",
                                 QuasarAppUtils::Info);
    if (force) {
        QuasarAppUtils::Params::log("clear force! " + targetDir,
                                    QuasarAppUtils::Info);
        if (QDir(targetDir).removeRecursively()) {
            return;
        }

        QuasarAppUtils::Params::log("Remove target Dir fail, try remove old deployemend files",
                                    QuasarAppUtils::Warning);
    }

    QMultiMap<int, QFileInfo> sortedOldData;
    for (auto& i : _deployedFiles) {
        sortedOldData.insert(i.size(), QFileInfo(i));
    }

    for (auto it = sortedOldData.end(); it != sortedOldData.begin(); --it) {

        auto index = it - 1;

        if (!index.value().exists()) {
            continue;
        }

        if (index.value().isFile()) {
            if (removeFile(index.value())) {
                QuasarAppUtils::Params::log("Remove " + index.value().absoluteFilePath() + " because it is deployed file",
                                            QuasarAppUtils::Info);
            }

        } else {
            QDir qdir(index.value().absoluteFilePath());
            if (!qdir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries).count()) {
                qdir.removeRecursively();
                QuasarAppUtils::Params::log("Remove " + index.value().absoluteFilePath() + " because it is empty",
                                            QuasarAppUtils::Info);
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

    auto prefixes = absalutePath.split(QRegExp("[\\/]"), splitbehavior);
    depch = std::min(depch, prefixes.size());
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
            if (info.fileName().contains(i, ONLY_WIN_CASE_INSENSIATIVE)) {
                skipFilter = i;
                break;
            }
        }

        if (!skipFilter.isEmpty()) {
            QuasarAppUtils::Params::log(
                        info.absoluteFilePath() + " ignored by filter " + skipFilter,
                        QuasarAppUtils::VerboseLvl::Debug);
            continue;
        }
        auto config = DeployCore::_config;

        LibInfo libInfo;
        libInfo.setName(info.fileName());
        libInfo.setPath(info.absolutePath());
        libInfo.setPlatform(GeneralFile);

        if (config)
            if (auto rule = config->ignoreList.isIgnore(libInfo)) {
                QuasarAppUtils::Params::log(
                            info.absoluteFilePath() + " ignored by rule " + rule->label,
                            QuasarAppUtils::VerboseLvl::Info);
                continue;
            }

        auto distanation = changeDistanation(info.absolutePath(), to, saveStructSize);

        if (!copyFile(info.absoluteFilePath(), distanation , mask)) {
            QuasarAppUtils::Params::log(
                        "not copied file " + distanation + "/" + info.fileName(),
                        QuasarAppUtils::VerboseLvl::Warning);
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
        QuasarAppUtils::Params::log("Qt Operation fail (remove file) " + file.absoluteFilePath(),
                                    QuasarAppUtils::Error);

        QuasarAppUtils::Params::log("Message: " + f.errorString(),
                                    QuasarAppUtils::Error);

        return false;
    }

    return true;
}
