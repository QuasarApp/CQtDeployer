#ifndef DEPLOY_H
#define DEPLOY_H
#include <QString>
#include <QStringList>

class Deploy
{
private:
    bool deployQml = false;
    QString qmlScaner = "";
    QString qmake = "";
    QString qtDir = "";
    QString target = "";
    QString targetDir = "";

    QStringList QtLibs;
    QStringList noQTLibs;
    QStringList qmlLibs;

    bool isQtLib(const QString& lib) const;

    void copyFiles(const QStringList &files, const QString &target);
    bool copyFile(const QString& file, const QString &target);
    void extract(const QString& file);

    void strip(const QString &dir);

public:
    Deploy();
    bool getDeployQml() const;
    void setDeployQml(bool value);

    QString getQmlScaner() const;
    void setQmlScaner(const QString &value);
    QString getQmake() const;
    void setQmake(const QString &value);
    QString getTarget() const;
    bool setTarget(const QString &value);

    void deploy();
    QString getQtDir() const;
    void setQtDir(const QString &value);
};

#endif // DEPLOY_H
