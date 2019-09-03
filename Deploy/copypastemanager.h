#ifndef COPYPASTEMANAGER_H
#define COPYPASTEMANAGER_H
#include <QFileInfo>
#include <QSet>
#include <QStringList>
#include <deploy_global.h>


class DeployedFiles;

class DEPLOYSHARED_EXPORT CopyPasteManager
{
private:
    bool fileActionPrivate(const QString &file, const QString &target,
                           QStringList *mask, bool isMove);

    bool initDir(const QString &path);
    QSet<QString> _deployedFiles;


public:
    CopyPasteManager();

    void copyFiles(const QStringList &files, const QString &targetDir);
    bool copyFile(const QString &file, const QString &target,
                  QStringList *mask = nullptr);

    bool removeFile(const QString &file);
    bool removeFile(const QFileInfo &file);

    /**
     * @brief smartCopyFile
     * @param file
     * @param target
     * @param mask
     * @return if file in target dir try move file else copy
     */
    bool smartCopyFile(const QString &file, const QString &target, const QString &targetDir,
                       QStringList *mask = nullptr);

    bool moveFile(const QString &file, const QString &target,
                  QStringList *mask = nullptr);

    bool copyFolder(const QString &from, const QString &to,
                    const QStringList &filter = QStringList(),
                    QStringList *listOfCopiedItems = nullptr,
                    QStringList *mask = nullptr);

    void clear(bool);

    QStringList deployedFiles() const;
    void setDeployedFiles(const QSet<QString> &deployedFiles);

    QStringList getDeployedFilesStringList() const;
    void setDeployedFiles(const QStringList &value);

    bool strip(const QString &dir) const;
    bool addToDeployed(const QString& path);

};

#endif // COPYPASTEMANAGER_H
