#ifndef WINDEPENDENCIESSCANNER_H
#define WINDEPENDENCIESSCANNER_H

#include <QMultiMap>
#include <QStringList>
#include "../qtTools/src/windeployqt/utils.h"


class WinDependenciesScanner {
private:
    QStringList _env;
    QMap<QString, QString> _EnvLibs;
    Platform platformFromMkSpec(const QString &xSpec);
    QMap<QString, QString> qMakeAll(QString *errorMessage,
                                    const QString &binary = "qmake");
public:
    explicit WinDependenciesScanner();

    void setEnvironment(const QStringList &env);

    QStringList scan(const QString& path,
                     Platform platfr = UnknownPlatform,
                     const QString &qmake = "qmake");

    ~WinDependenciesScanner();
};

#endif // WINDEPENDENCIESSCANNER_H
