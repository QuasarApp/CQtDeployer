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

    QSet<QString> enabledPlugins() const;
    void setEnabledPlugins(const QSet<QString> &enabled);
    void addEnabledPlugins(const QString &enabled);

    QSet<QString> disabledPlugins() const;
    void setDisabledPlugins(const QSet<QString> &disabled);
    void addDisabledPlugins(const QString &disabled);

    QSet<QString> extraPlugins() const;
    void setExtraPlugins(const QSet<QString> &extraPlugins);
    void addExtraPlugins(const QString &extraPlugin);

private:
    QString _name;
    QString _description;
    QString _version;
    QString _releaseData;
    QString _icon;
    QString _publisher;

    QSet<QString> _targets;
    QSet<QString> _qmlInput;

    // plugins
    QSet<QString> _enabled;
    QSet<QString> _disabled;
    QSet<QString> _extraPlugins;
};

#endif // DISTROMODULE_H
