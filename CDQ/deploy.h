#ifndef DEPLOY_H
#define DEPLOY_H
#include <QDir>
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
    QStringList neededPlugins;
    QStringList ignoreList;

    bool isQtLib(const QString& lib) const;

    void copyFiles(const QStringList &files, const QString &target);
    bool copyFile(const QString& file, const QString &target);
    void extract(const QString& file);

    void extractPlugins(const QString &lib);
    bool copyPlugin(const QString &plugin);
    void copyPlugins(const QStringList& list);
    bool copyFolder(QDir &from, QDir &to, const QString &filter = "");


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
