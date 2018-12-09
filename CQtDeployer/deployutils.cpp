#include "deployutils.h"

#include <QFileInfo>

QString DeployUtils::qtDir = "";
QStringList DeployUtils::extraPaths = QStringList();

DeployUtils::DeployUtils()
{

}

int DeployUtils::getLibPriority(const QString &lib) {

    if (!QFileInfo(lib).isFile()) {
        return 0;
    }

    if (isQtLib(lib)) {
        return 3;
    }

    if (isExtraLib(lib)) {
        return 2;
    }

    return 1;
}

bool DeployUtils::isQtLib(const QString &lib) {
    QFileInfo info(lib);
    return !qtDir.isEmpty() && info.absolutePath().contains(qtDir);

}

bool DeployUtils::isExtraLib(const QString &lib) {
    QFileInfo info(lib);

    for (auto i : extraPaths) {
        if (info.absolutePath().contains(i)) {
            return true;
        }
    }

    return false;
}
