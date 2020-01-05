#include "idistribution.h"
#include <typeinfo>
#include <QFile>

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
