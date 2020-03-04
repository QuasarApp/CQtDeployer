//#
//# Copyright (C) 2018-2020 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef COPYPASTEMANAGER_H
#define COPYPASTEMANAGER_H
#include <QFileInfo>
#include <QSet>
#include <QStringList>
#include <deploy_global.h>



class DEPLOYSHARED_EXPORT FileManager
{
private:
    bool fileActionPrivate(const QString &file, const QString &target,
                           QStringList *mask, bool isMove, bool targetIsFile);

    bool initDir(const QString &path);
    QSet<QString> _deployedFiles;


public:
    FileManager();

    bool copyFile(const QString &file, const QString &target,
                  QStringList *mask = nullptr, bool targetIsFile = false);

    bool removeFile(const QString &file);
    bool removeFile(const QFileInfo &file);

    /**
     * @brief smartCopyFile
     * @param file
     * @param target
     * @param mask
     * @param ifFileTarget
     * @return if file in target dir try move file else copy
     */
    bool smartCopyFile(const QString &file, const QString &target,
                       QStringList *mask = nullptr, bool ifFileTarget = false);

    bool moveFile(const QString &file, const QString &target,
                  QStringList *mask = nullptr, bool targetIsFile = false);

    bool copyFolder(const QString &from, const QString &to,
                    const QStringList &filter = QStringList(),
                    QStringList *listOfCopiedItems = nullptr,
                    QStringList *mask = nullptr);

    bool moveFolder(const QString &from, const QString &to, const QString &ignore);

    void clear(const QString& targetDir, bool force);


    QStringList getDeployedFilesStringList() const;
    QSet<QString> getDeployedFiles() const;

    bool strip(const QString &dir) const;
    bool addToDeployed(const QString& path);

    void saveDeploymendFiles(const QString &targetDir);
    void loadDeployemendFiles(const QString &targetDir);
};

#endif // COPYPASTEMANAGER_H
