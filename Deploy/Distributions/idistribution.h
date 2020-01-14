#ifndef IDISTRIBUTION_H
#define IDISTRIBUTION_H

#include "distrostruct.h"
#include "envirement.h"
#include "templateinfo.h"

#include <QFileInfo>

class DEPLOYSHARED_EXPORT iDistribution
{
public:
    iDistribution();
    virtual ~iDistribution();

    virtual QString getConfig() const = 0;
    virtual bool deployTemplate() const = 0;
    virtual Envirement toolKitLocation() const = 0;
    virtual QString runCmd() const = 0;
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

private:
    QString _location = "packageTemplate";

};

#endif // IDISTRIBUTION_H
