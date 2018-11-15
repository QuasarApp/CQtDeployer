#ifndef WINDEPENDENCIESSCANNER_H
#define WINDEPENDENCIESSCANNER_H

#include "windependenciesscanner_global.h"


class WINDEPENDENCIESSCANNERSHARED_EXPORT WinDependenciesScanner
{
public:
    explicit WinDependenciesScanner();

    QList<QByteArray> scan(const QString& path);

    ~WinDependenciesScanner();
};

#endif // WINDEPENDENCIESSCANNER_H
