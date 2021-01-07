//#
//# Copyright (C) 2018-2021 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#include "dependencymap.h"
#include "deploycore.h"
#include "quasarapp.h"

DependencyMap::DependencyMap() {
    _qtModules = DeployCore::QtModule::NONE;
}

DependencyMap &DependencyMap::operator +=(const DependencyMap &other) {
    this->_qtModules = this->_qtModules | other._qtModules;
    this->_neadedLibs = this->_neadedLibs + other._neadedLibs;
    this->_systemLibs = this->_systemLibs + other._systemLibs;
    this->_extraData = this->_extraData + other._extraData;

    return *this;
}

DependencyMap &DependencyMap::operator -=(const DependencyMap &other) {
    this->_qtModules = this->_qtModules & (~other._qtModules);
    this->_neadedLibs = this->_neadedLibs - other._neadedLibs;
    this->_systemLibs = this->_systemLibs - other._systemLibs;
    this->_extraData = this->_extraData - other._extraData;

    return *this;
}

DeployCore::QtModule DependencyMap::qtModules() const {
    return _qtModules;
}

const QSet<QString>& DependencyMap::neadedLibs() const {
    return _neadedLibs;
}

const QSet<QString> &DependencyMap::systemLibs() const {
    return _systemLibs;
}

void DependencyMap::addModule(DeployCore::QtModule module) {
    this->_qtModules = this->_qtModules | module;

}

void DependencyMap::addSystemLib(const QString &lib) {
    _systemLibs.insert(lib);
}

void DependencyMap::addNeadedLib(const QString &lib) {
    _neadedLibs.insert(lib);
    DeployCore::addQtModule(_qtModules, lib);
}

void DependencyMap::addExtraData(const QString &data) {
    _extraData += data;
}

void DependencyMap::removeModule(DeployCore::QtModule module) {
    _qtModules = _qtModules & (~module);
}

void DependencyMap::removeSystemLib(const QString &lib) {
    _systemLibs.remove(lib);
}

void DependencyMap::removeNeadedLib(const QString &lib) {
    _neadedLibs.remove(lib);
}

void DependencyMap::removeExtraData(const QString &data) {
    _extraData -= data;
}

bool DependencyMap::containsSysLib(const QString &lib) const {
    return _systemLibs.contains(lib);
}

bool DependencyMap::containsModule(DeployCore::QtModule module) const {
    return _qtModules & module;
}

bool DependencyMap::containsNeadedLib(const QString &lib) const {
    return _neadedLibs.contains(lib);
}

bool DependencyMap::containsExtraData(const QString &data) const {
    return _extraData.contains(data);
}

const QSet<QString> DependencyMap::targets() const {
    return _targets;
}


const QSet<QString> DependencyMap::extraData() const {
    return _extraData;
}


