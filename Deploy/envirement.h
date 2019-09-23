#ifndef ENVIREMENT_H
#define ENVIREMENT_H

#include <QSet>
#include <QStringList>
#include "deploy_global.h"



class DEPLOYSHARED_EXPORT Envirement
{
private:
    QSet<QString> _ignoreEnvList;
    QSet<QString> _deployEnvironment;

public:
    Envirement();
    QStringList deployEnvironment() const;
    QStringList ignoreEnvList() const;
    void setIgnoreEnvList(const QStringList &ignoreEnvList);

    void addEnv(const QString &dir, const QString &appDir = "", const QString &targetDir = "");
    // return true if file exits in this envirement
    bool inThisEnvirement(const QString &file) const;

    int size() const;
    QString concatEnv() const;
};

#endif // ENVIREMENT_H
