#ifndef QTDIR_H
#define QTDIR_H

#include <QDir>
#include "deploy_global.h"
#include "deploycore.h"

class DEPLOYSHARED_EXPORT QtDir {
    QString libs;
    QString bins;
    QString libexecs;
    QString plugins;
    QString qmls;
    QString translations;
    QString resources;

    Platform qtPlatform;


public:
    QString getLibs() const;
    void setLibs(const QString &value);
    QString getBins() const;
    void setBins(const QString &value);
    QString getLibexecs() const;
    void setLibexecs(const QString &value);
    QString getPlugins() const;
    void setPlugins(const QString &value);
    QString getQmls() const;
    void setQmls(const QString &value);
    QString getTranslations() const;
    void setTranslations(const QString &value);
    QString getResources() const;
    void setResources(const QString &value);
    Platform getQtPlatform() const;
    void setQtPlatform(const Platform &value);

};

#endif // QTDIR_H
