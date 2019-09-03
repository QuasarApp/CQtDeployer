#ifndef DEPLOYEDFILES_H
#define DEPLOYEDFILES_H
#include <QSet>
#include <QStringList>
#include <deploy_global.h>

class DEPLOYSHARED_EXPORT DeployedFiles
{
private:
    QSet<QString> deployedFiles;

public:
    DeployedFiles(const QStringList& from);
    bool addToDeployed(const QString& path);
    bool strip(const QString &dir);

    QSet<QString> getDeployedFiles() const;
    void setDeployedFiles(const QSet<QString> &value);

    QStringList getDeployedFilesStringList() const;
    void setDeployedFiles(const QStringList &value);
};

#endif // DEPLOYEDFILES_H
