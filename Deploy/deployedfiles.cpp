#include "deployedfiles.h"

#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <quasarapp.h>
#include <QDebug>

QSet<QString> DeployedFiles::getDeployedFiles() const {
    return deployedFiles;
}

void DeployedFiles::setDeployedFiles(const QSet<QString> &value) {
    deployedFiles = value;
}

QStringList DeployedFiles::getDeployedFilesStringList() const {
    return deployedFiles.toList();
}

void DeployedFiles::setDeployedFiles(const QStringList &value) {
    deployedFiles.clear();
    deployedFiles.fromList(value);
}

DeployedFiles::DeployedFiles(const QStringList& from) {
    setDeployedFiles(from);
}

bool DeployedFiles::addToDeployed(const QString& path) {
    auto info = QFileInfo(path);
    if (info.isFile() || !info.exists()) {
        deployedFiles += info.absoluteFilePath();

        auto completeSufix = info.completeSuffix();
        if (info.isFile() && (completeSufix.isEmpty() || completeSufix.toLower() == "run"
                || completeSufix.toLower() == "sh")) {

            if (!QFile::setPermissions(path, static_cast<QFile::Permission>(0x7777))) {
                QuasarAppUtils::Params::verboseLog("permishens set fail", QuasarAppUtils::Warning);
            }
        }
    }
    return true;
}

bool DeployedFiles::strip(const QString &dir) {

#ifdef Q_OS_WIN
    Q_UNUSED(dir);
    return true;
#else
    QFileInfo info(dir);

    if (!info.exists()) {
        QuasarAppUtils::Params::verboseLog("dir not exits!");
        return false;
    }

    if (info.isDir()) {
        QDir d(dir);
        auto list = d.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

        bool res = false;
        for (auto &&i : list) {
            res = strip(i.absoluteFilePath()) || res;
        }

        return res;
    } else {

        auto sufix = info.completeSuffix();
        if (!sufix.contains("so") && !sufix.contains("dll")) {
            return false;
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


void DeployedFiles::clear(bool force, const QString& targetDir) {

    qInfo() << "clear start!";

    if (force) {
        qInfo() << "clear force! " << targetDir;

        if (QDir(targetDir).removeRecursively()) {
            return;
        }

        QuasarAppUtils::Params::verboseLog("Remove target Dir fail, try remove old deployemend files",
                                           QuasarAppUtils::Warning);
    }

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
