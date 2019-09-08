#ifndef ENVIREMENT_H
#define ENVIREMENT_H

#include <QStringList>



class Envirement
{
private:
    QStringList _ignoreEnvList;
    QStringList _deployEnvironment;

public:
    Envirement();
    QStringList deployEnvironment() const;
    void setDeployEnvironment(const QStringList &deployEnvironment);
    QStringList ignoreEnvList() const;
    void setIgnoreEnvList(const QStringList &ignoreEnvList);

    void addEnv(const QString &dir, const QString &appDir, const QString &targetDir);

    int size() const;
    QString concatEnv() const;
};

#endif // ENVIREMENT_H
