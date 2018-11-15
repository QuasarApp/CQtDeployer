#include "windependenciesscanner.h"

#include <QList>
#include "../qtTools/src/windeployqt/elfreader.h"

WinDependenciesScanner::WinDependenciesScanner() {}

QList<QByteArray> WinDependenciesScanner::scan(const QString &path) {
    ElfReader scaner(path);
    return scaner.dependencies();
}
