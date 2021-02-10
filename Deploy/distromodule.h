/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef DISTROMODULE_H
#define DISTROMODULE_H

#include "distrostruct.h"

#include <QSet>

class DEPLOYSHARED_EXPORT DistroModule: public DistroStruct
{
public:
    explicit DistroModule(const QString& key );
    DistroModule(const DistroModule& other ) = default;

    const QSet<QString> &targets() const;
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

    QString homePage() const;
    void setHomePage(const QString &homePage);

    QString prefix() const;
    void setPrefix(const QString &location);

    QString key() const;

    bool isDefaultModule() const;

    bool isValid();

    QSet<QString> extraData() const;
    void setExtraData(const QSet<QString> &extraFiles);
    void addExtraData(const QString &extraFile);

protected:
    void setKey(const QString &key);

private:
    int _id = 0;
    QString _name;
    QString _key;
    QString _prefix;
    QString _description;
    QString _version;
    QString _releaseData;
    QString _icon;
    QString _publisher;
    QString _homePage;

    QSet<QString> _targets;
    QSet<QString> _qmlInput;

    // plugins
    QSet<QString> _enabled;
    QSet<QString> _disabled;
    QSet<QString> _extraPlugins;
    QSet<QString> _extraData;

};

#endif // DISTROMODULE_H
