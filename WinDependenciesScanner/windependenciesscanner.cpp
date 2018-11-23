#include "windependenciesscanner.h"

#include <QList>
#include <QDir>
#include "../qtTools/src/windeployqt/elfreader.h"

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

QStringList WinDependenciesScanner::scan(const QString &path) {
    QStringList result;

    ElfReader scaner(path);
    auto dep = scaner.dependencies();

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
