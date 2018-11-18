#include "windependenciesscanner.h"

#include <QList>
#include <QDir>
#include "../qtTools/src/windeployqt/elfreader.h"

WinDependenciesScanner::WinDependenciesScanner() {}

void WinDependenciesScanner::setEnvironment(const QString &env) {
    _env = env.split(":");

    QDir dir;
    for (auto i : _env) {
        dir.setPath(i);
        if (!dir.exists()) {
            continue;
        }

        auto list = dir.entryList(QStringList() << "*.dll",
                                  QDir::Files| QDir::NoDotAndDotDot);



    }

}

QStringList WinDependenciesScanner::scan(const QString &path) {
    QStringList result;

    ElfReader scaner(path);
    auto dep = scaner.dependencies();

    return result;
}
