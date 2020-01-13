#include "idistribution.h"
#include <typeinfo>
#include <QFile>
#include <QTextStream>

iDistribution::iDistribution() = default;
iDistribution::~iDistribution() = default;

QString iDistribution::getClassName() const {
    return typeid(*this).name();
}

QString iDistribution::getLocation() const {
    return _location;
}

void iDistribution::setLocation(const QString &location) {
    _location = location;
}

bool iDistribution::unpack(const QString &resource,
                           const QString &target,
                           const TemplateInfo &info) const {
    QFile file(resource);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QString inputData = file.readAll();
    file.close();

    file.setFileName(target);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }

    inputData.replace("$NAME", info.Name);
    inputData.replace("$DESCRIPTION", info.Description);
    inputData.replace("$VERSION", info.Version);
    inputData.replace("$RELEASEDATA", info.ReleaseData);
    inputData.replace("$ICON", info.Icon);

    for (auto it = info.Custom.cbegin(); it != info.Custom.cend(); ++it) {
        inputData.replace(it.key(), it.value());
    }

    QTextStream stream(&file);
    stream << inputData;

    file.close();

    return true;
}
