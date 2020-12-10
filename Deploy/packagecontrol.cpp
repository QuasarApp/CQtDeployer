#include "packagecontrol.h"

#include <QDir>
#include <QFileInfo>


PackageControl::PackageControl()
{

}

PackageControl::~PackageControl(){

}

bool PackageControl::isEmpty(const QString &package) const {
    QFileInfo info(package);
    QDir dirInfo(package);
    return !(info.exists() && dirInfo.entryList(QDir::AllEntries | QDir::NoDotAndDotDot).size());
}
