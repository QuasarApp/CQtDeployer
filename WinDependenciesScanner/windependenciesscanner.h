#ifndef WINDEPENDENCIESSCANNER_H
#define WINDEPENDENCIESSCANNER_H

#include "windependenciesscanner_global.h"

#include <QMultiMap>
#include <QStringList>


class WINDEPENDENCIESSCANNERSHARED_EXPORT WinDependenciesScanner
{
private:
    QStringList _env;
    QMultiMap<QString, QString> _EnvLibs;
public:
    explicit WinDependenciesScanner();

    void setEnvironment(const QStringList &env);

    QStringList scan(const QString& path);

    ~WinDependenciesScanner();
};

#endif // WINDEPENDENCIESSCANNER_H
