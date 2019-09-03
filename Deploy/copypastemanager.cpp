#include "copypastemanager.h"
#include <QDir>
#include <quasarapp.h>
#include "deploycore.h"
#include "deployedfiles.h"
#include <fstream>

CopyPasteManager::CopyPasteManager(DeployedFiles *deplyed) {
    assert(deplyed);
    _deployedFiles = deplyed;
}

bool CopyPasteManager::initDir(const QString &path) {

    if (!QFileInfo::exists(path)) {
        _deployedFiles->addToDeployed(path);
        if (!QDir().mkpath(path)) {
            return false;
        }
    }

    return true;
}

bool CopyPasteManager::fileActionPrivate(const QString &file, const QString &target,
                               QStringList *masks, bool isMove) {

    auto info = QFileInfo(file);

    bool copy = !masks;
    if (masks) {
        for (auto mask : *masks) {
            if (info.absoluteFilePath().contains(mask)) {
                copy = true;
                break;
            }
        }
    }

    if (!copy) {
        QuasarAppUtils::Params::verboseLog(((isMove)? "skip move :": "skip copy :" + file));
        return false;
    }

    auto name = info.fileName();
    info.setFile(target + QDir::separator() + name);

    if (!initDir(info.absolutePath())) {
        return false;
    }

    if (QFileInfo(file).absoluteFilePath() ==
            QFileInfo(target + QDir::separator() + name).absoluteFilePath()) {
        return true;
    }

    if (QuasarAppUtils::Params::isEndable("always-overwrite") &&
            info.exists() && !removeFile( target + QDir::separator() + name)) {
        return false;
    }

    qInfo() << ((isMove)? "move :": "copy :") << file;

    QFile sourceFile(file);

    if (!((isMove)?
          sourceFile.rename(target + QDir::separator() + name):
          sourceFile.copy(target + QDir::separator() + name))) {

        QuasarAppUtils::Params::verboseLog("Qt Operation fail " + file + " >> " + target + QDir::separator() + name +
                                           " Qt error: " + sourceFile.errorString(),
                                           QuasarAppUtils::Warning);

        std::ifstream  src(file.toStdString(),
                           std::ios::binary);
        std::ofstream  dst((target + QDir::separator() + name).toStdString(),
                           std::ios::binary);

        dst << src.rdbuf();

        if (!QFileInfo::exists(target + QDir::separator() + name)) {
            QuasarAppUtils::Params::verboseLog("std Operation fail file not copied. "
                                               "Сheck if you have access to the target dir",
                                               QuasarAppUtils::Error);
            return false;

        }
    }

    _deployedFiles->addToDeployed(target + QDir::separator() + name);
    return true;
}

bool CopyPasteManager::removeFile(const QString &file) {
    return removeFile(QFileInfo (file));
}

bool CopyPasteManager::smartCopyFile(const QString &file, const QString &target,
                                     const QString& targetDir, QStringList *mask) {
    if (file.contains(targetDir)) {
        if (!moveFile(file, target, mask)) {
            QuasarAppUtils::Params::verboseLog(" file not moved! try copy");

            if (!copyFile(file, target, mask)) {
                qCritical() << "not copy target to bin dir " << file;
                return false;
            };
        };
    } else {
        if (!copyFile(file, target, mask)) {
            qCritical() << "not copy target to bin dir " << file;
            return false;
        };
    }

    return true;
}

bool CopyPasteManager::moveFile(const QString &file, const QString &target, QStringList *masks) {
    return fileActionPrivate(file, target, masks, true);
}

bool CopyPasteManager::copyFolder(const QString &from, const QString &to, const QStringList &filter,
                        QStringList *listOfCopiedItems, QStringList *mask) {

    QDir fromDir(from);

    auto list = fromDir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);

    for (auto &&item : list) {
        if (QFileInfo(item).isDir()) {

            copyFolder(item.absoluteFilePath(), to + "/" + item.fileName(), filter, listOfCopiedItems, mask);
        } else {

            QString skipFilter = "";
            for (auto && i: filter) {
                if (item.fileName().contains(i)) {
                    skipFilter = i;
                    break;
                }
            }

            if (!skipFilter.isEmpty()) {
                QuasarAppUtils::Params::verboseLog(
                            item.absoluteFilePath() + " ignored by filter " + skipFilter,
                            QuasarAppUtils::VerboseLvl::Info);
                continue;
            }

            if (!copyFile(item.absoluteFilePath(), to , mask)) {
                QuasarAppUtils::Params::verboseLog(
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

void CopyPasteManager::clear(bool force, const QString& targetDir) {
    qInfo() << "clear start!";

    if (force) {
        qInfo() << "clear force! " << targetDir;

        if (QDir(targetDir).removeRecursively()) {
            return;
        }

        QuasarAppUtils::Params::verboseLog("Remove target Dir fail, try remove old deployemend files",
                                           QuasarAppUtils::Warning);
    }

    auto deployedFiles = _deployedFiles->getDeployedFiles();
    QMap<int, QFileInfo> sortedOldData;
    for (auto& i :deployedFiles) {
        sortedOldData.insertMulti(i.size(), QFileInfo(i));
    }

    for (auto it = sortedOldData.end(); it != sortedOldData.begin(); --it) {

        auto index = it - 1;

        if (index.value().isFile()) {
            if (removeFile(index.value())) {
                qInfo() << "Remove " << index.value().absoluteFilePath() << " becouse it is deployed file";
            }

        } else {
            QDir qdir(index.value().absoluteFilePath());
            if (!qdir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries).count()) {
                qdir.removeRecursively();
                qInfo() << "Remove " << index.value().absoluteFilePath() << " becouse it is empty";
            }
        }
    }

    deployedFiles.clear();
}

void CopyPasteManager::copyFiles(const QStringList &files, const QString& targetDir) {
    for (auto file : files) {
        QFileInfo target(file);
        auto targetPath = targetDir + QDir::separator() + "lib";
        if (target.completeSuffix().contains("dll", Qt::CaseInsensitive) ||
                target.completeSuffix().contains("exe", Qt::CaseInsensitive)) {

            targetPath = targetDir;
        }

        if (!smartCopyFile(file, targetPath, targetDir)) {
            QuasarAppUtils::Params::verboseLog(file + " not copied");
        }
    }
}

bool CopyPasteManager::copyFile(const QString &file, const QString &target,
                      QStringList *masks) {

    return fileActionPrivate(file, target, masks, false);
}

bool CopyPasteManager::removeFile(const QFileInfo &file) {

    if (!QFile::remove(file.absoluteFilePath())) {
        QuasarAppUtils::Params::verboseLog("Qt Operation fail (remove file) " + file.absoluteFilePath(),
                                           QuasarAppUtils::Warning);

        if (remove(file.absoluteFilePath().toLatin1())) {
            QuasarAppUtils::Params::verboseLog("std Operation fail file not removed." + file.absoluteFilePath(),
                                               QuasarAppUtils::Error);
            return false;
        }
    }

    return true;
}
