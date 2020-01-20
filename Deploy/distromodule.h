#ifndef DISTROMODULE_H
#define DISTROMODULE_H

#include "distrostruct.h"

#include <QSet>

class DEPLOYSHARED_EXPORT DistroModule: public DistroStruct
{
public:
    DistroModule();
    QSet<QString> targets() const;
    void setTargets(const QSet<QString> &targets);
    void addTarget(const QString& target);

    QSet<QString> qmlInput() const;
    void setQmlInput(const QSet<QString> &qmlInput);
    void addQmlInput(const QString& target);


    QString name() const;
    void setName(const QString &name);

    QString description() const;
    void setDescription(const QString &description);

    QString version() const;
    void setVersion(const QString &version);

    QString releaseData() const;
    void setReleaseData(const QString &releaseData);

    QString icon() const;
    void setIcon(const QString &icon);

    QString publisher() const;
    void setPublisher(const QString &publisher);

private:
    QString _name;
    QString _description;
    QString _version;
    QString _releaseData;
    QString _icon;
    QString _publisher;

    QSet<QString> _targets;
    QSet<QString> _qmlInput;
};

#endif // DISTROMODULE_H
