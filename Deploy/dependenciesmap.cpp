#include "dependenciesmap.h"

DependenciesMap::DependenciesMap() {

}

DependenciesMap &DependenciesMap::operator +=(const DependenciesMap &other) {

}

DependenciesMap &DependenciesMap::operator -=(const DependenciesMap &other) {

}

DeployCore::QtModule DependenciesMap::qtModules() const {
    return _qtModules;
}

const QSet<QString>& DependenciesMap::neadedLibs() const {
    return _neadedLibs;
}

const QSet<QString> &DependenciesMap::systemLibs() const {
    return _systemLibs;
}
