#ifndef IDISTRIBUTION_H
#define IDISTRIBUTION_H

#include "distrostruct.h"
#include "envirement.h"
#include "templateinfo.h"
#include <QProcess>

#include <QFileInfo>
class FileManager;
class DistroModule;

class DEPLOYSHARED_EXPORT iDistribution
{
public:
    iDistribution(FileManager * fileManager);
    virtual ~iDistribution();

    virtual QString getConfig() const = 0;
    virtual bool deployTemplate() = 0;
    virtual bool removeTemplate() const = 0;
    virtual Envirement toolKitEnv() const = 0;
    virtual QProcessEnvironment processEnvirement() const = 0;

    virtual QString runCmd() = 0;
    virtual QStringList runArg() const = 0;

    QString getClassName() const;

protected:
    QString getLocation() const;
    void setLocation(const QString &location);
    bool unpackFile(const QFileInfo& resource,
                    const QString& target,
                    const TemplateInfo& info) const;

    bool unpackDir(const QString& resource,
                   const QString& target,
                   const TemplateInfo& info) const;

    bool moveData(const QString& from, const QString& to, const QString &ignore) const;
    bool copyFile(const QString& from, const QString& to) const;

    QString findProcess(const QString& env, const QString& proc) const;

    QMap<int, QPair<QString, const DistroModule *> > sortPrefixes(const QHash<QString, DistroModule> &input);
private:
    QString _location = "Temp Template";
    FileManager * _fileManager = nullptr;

};

#endif // IDISTRIBUTION_H
