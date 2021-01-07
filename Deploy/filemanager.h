//#
//# Copyright (C) 2018-2021 QuasarApp.
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

    QSet<QString> _deployedFiles;

    /**
     * @brief changeDistanation - this function create new distanation path.
     *  (merge base path and pathFrom with depth)
     * @param absalutePath
     * @param basePath
     * @param depch
     * @return the new path
     */
    QString changeDistanation(const QString &absalutePath, QString basePath, int depch);
public:
    FileManager();
    bool initDir(const QString &path);

    bool copyFile(const QString &file, const QString &target,
                  QStringList *mask = nullptr, bool targetIsFile = false);

    /**
     * @brief copyFiles - copy files (source) to (to)
     * @param source - copy source files
     * @param to - distanation folder
     * @param saveStructSize - size of name of source file to save after copy. example:
     *  copy /Foo/A.txt /Bar/ (size 0) - result file copyed to /Bar/A.txt
     *  copy /Foo/A.txt /Bar/ (size 1) - to /Bar/Foo/A.txt
     * @param filter - list of forbiden names for ignore files
     * @param listOfCopiedItems - list of copyed files
     * @param mask - mask of copyed files
     * @return true if files copyed
     */
    bool copyFiles(const QStringList &source, const QString &to, int saveStructSize = 0,
                    const QStringList &filter = QStringList(),
                    QStringList *listOfCopiedItems = nullptr,
                    QStringList *mask = nullptr);

    bool copyFolder(const QString &from, const QString &to,
                    const QStringList &filter = QStringList(),
                    QStringList *listOfCopiedItems = nullptr,
                    QStringList *mask = nullptr, bool force = false);

    bool cp(const QString &from, const QString &to,
            const QStringList &filter = QStringList(),
            QStringList *listOfCopiedItems = nullptr,
            QStringList *mask = nullptr, bool force = false);

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



    bool moveFolder(const QString &from, const QString &to, const QString &ignore = "");

    void clear(const QString& targetDir, bool force);


    QStringList getDeployedFilesStringList() const;
    QSet<QString> getDeployedFiles() const;

    bool strip(const QString &dir) const;
    bool addToDeployed(const QString& path);
    void removeFromDeployed(const QString& path);

    void saveDeploymendFiles(const QString &targetDir);
    void loadDeployemendFiles(const QString &targetDir);
};

#endif // COPYPASTEMANAGER_H
