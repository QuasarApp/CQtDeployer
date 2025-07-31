/*
 * Copyright (C) 2018-2024 QuasarApp.
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

    QString publisher() const;
    void setPublisher(const QString &publisher);

    const QSet<QString> &enabledPlugins() const;
    void setEnabledPlugins(const QSet<QString> &enabled);
    void addEnabledPlugins(const QString &enabled);

    const QSet<QString> &disabledPlugins() const;
    void setDisabledPlugins(const QSet<QString> &disabled);
    void addDisabledPlugins(const QString &disabled);

    const QSet<QString>& extraPlugins() const;
    void setExtraPlugins(const QSet<QString> &extraPlugins);
    void addExtraPlugins(const QString &extraPlugin);

    QString homePage() const;
    void setHomePage(const QString &homePage);

    QString prefix() const;
    void setPrefix(const QString &location);

    QString key() const;

    bool isDefaultModule() const;

    bool isValid();

    const QSet<QString>& extraData() const;
    void setExtraData(const QSet<QString> &extraFiles);
    void addExtraData(const QString &extraFile);

    const QSet<QString>& translation() const;
    void setTranslation(const QSet<QString> &tr);
    void addTranslation(const QString &tr);

    QString installDirDEB() const;
    void setInstallDirDEB(const QString &newInstallDir);

    /**
     * @brief debOut This method return output filePath to debian package
     * @return output filePath to debian package
     */
    const QString &debOut() const;

    /**
     * @brief setDebOut This method sets new value of debian outpup path.
     * @param newDebOut This is new value of the debian output
     */
    void setDebOut(const QString &newDebOut);

    /**
     * @brief debOut This method return output filePath to zip package
     * @return output filePath to zip package
     */
    const QString &zipOut() const;

    /**
     * @brief setDebOut This method sets new value of zip outpup path.
     * @param newDebOut This is new value of the zip output
     */
    void setZipOut(const QString &newZipOut);

protected:
    void setKey(const QString &key);

private:
    QString _name;
    QString _key;
    QString _prefix;
    QString _description;
    QString _version;
    QString _releaseData;
    QString _publisher;
    QString _homePage;

    QSet<QString> _targets;
    QSet<QString> _qmlInput;

    // plugins
    QSet<QString> _enabled;
    QSet<QString> _disabled;
    QSet<QString> _extraPlugins;

    // extra data
    QSet<QString> _extraData;

    // extra translations
    QSet<QString> _tr;

    QString _installDirDEB;
    QString _debOut;
    QString _zipOut;

};

#endif // DISTROMODULE_H
