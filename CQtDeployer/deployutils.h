#ifndef DEPLOYUTILS_H
#define DEPLOYUTILS_H

#include <QStringList>
#include <QDebug>

class Deploy;

class DeployUtils
{
public:
    DeployUtils() = delete;

    static QString qtDir;
    static QStringList extraPaths;

    static bool isQtLib(const QString &lib);
    static bool isExtraLib(const QString &lib);
    static int getLibPriority(const QString &lib);
    static void verboseLog(const QString &str);
    static void help();
    static bool parseQt(Deploy *deploy);
};

#endif // DEPLOYUTILS_H
