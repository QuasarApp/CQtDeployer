#include "windependenciesscanner.h"

#include <QList>
#include <QDir>
#include <QDebug>

WinDependenciesScanner::WinDependenciesScanner() {}

void WinDependenciesScanner::setEnvironment(const QStringList &env) {
    QDir dir;
    for (auto i : env) {
        dir.setPath(i);
        if (!dir.exists()) {
            continue;
        }

        auto list = dir.entryInfoList(QStringList() << "*.dll",
                                  QDir::Files| QDir::NoDotAndDotDot);

        for (auto i : list) {
            _EnvLibs.insert(i.fileName(), i.absoluteFilePath());
        }

    }

}

Platform WinDependenciesScanner::platformFromMkSpec(const QString &xSpec)
{
    if (xSpec == QLatin1String("linux-g++"))
        return Unix;
    if (xSpec.startsWith(QLatin1String("win32-")))
        return xSpec.contains(QLatin1String("g++")) ? WindowsMinGW : Windows;
    if (xSpec.startsWith(QLatin1String("winrt-x")))
        return WinRtIntel;
    if (xSpec.startsWith(QLatin1String("winrt-arm")))
        return WinRtArm;
    if (xSpec.startsWith(QLatin1String("wince"))) {
        if (xSpec.contains(QLatin1String("-x86-")))
            return WinCEIntel;
        if (xSpec.contains(QLatin1String("-arm")))
            return WinCEArm;
    }
    return UnknownPlatform;
}

QStringList WinDependenciesScanner::scan(const QString &path, Platform platfr) {
    QStringList result;

    QString errorMessage;

    QStringList dep;
    readExecutable(path, platfr, &errorMessage, &dep);

    if (!errorMessage.isEmpty()) {
        qCritical() << errorMessage;
        return result;
    }

    for (auto i : dep) {
        QString lib(i);
        if (_EnvLibs.count(lib)) {
            result.push_back(_EnvLibs.value(lib));
        }
    }

    return result;
}

WinDependenciesScanner::~WinDependenciesScanner() {

}
