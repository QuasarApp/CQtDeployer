#ifndef DEPLOYUTILS_H
#define DEPLOYUTILS_H

#include <QStringList>



class DeployUtils
{
public:
    DeployUtils();

    static QString qtDir;
    static QStringList extraPaths;

    static bool isQtLib(const QString &lib);
    static bool isExtraLib(const QString &lib);
    static int getLibPriority(const QString &lib);

};

#endif // DEPLOYUTILS_H
