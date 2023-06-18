/*
 * Copyright (C) 2018-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "libinfo.h"
#include "pathutils.h"

bool operator ==(const LibInfo &left, const LibInfo &right) {
    return left.fullPath() == right.fullPath();
}

bool operator <=(const LibInfo &left, const LibInfo &right){
    return !operator>(left, right);
}

bool operator >=(const LibInfo &left, const LibInfo &right) {
    return !operator<(left, right);
}

bool operator <(const LibInfo &left, const LibInfo &right){
    return left._priority < right._priority;
}

bool operator >(const LibInfo &left, const LibInfo &right) {
    return left._priority > right._priority;
}

const QSet<LibInfo> &LibInfo::getAllDep() const {
    return _allDep;
}

Platform LibInfo::getPlatform() const {
    return _platform;
}

void LibInfo::setPlatform(const Platform &value) {
    _platform = value;
}

QString LibInfo::getName() const {
    return _name;
}

void LibInfo::setName(const QString &value) {
    _name = value;
}

QString LibInfo::getPath() const {
    return _path;
}

void LibInfo::setPath(const QString &value) {
    _path = value;
}

void LibInfo::addDependncies(const QString &value) {
    _dependncies.insert(value);
}

void LibInfo::addDepends(const QString &value) {
    addDependncies(value);
}

void LibInfo::addDependncies(const QSet<QString> &value) {
    _dependncies += value;
}

void LibInfo::removeDependncies(const QString &value) {
    _dependncies.remove(value);
}

QSet<QString> LibInfo::getDependncies() const {
    return _dependncies;
}

void LibInfo::setDependncies(const QSet<QString> &value) {
    _dependncies = value;
}

LibPriority LibInfo::getPriority() const {
    return _priority;
}

void LibInfo::setPriority(const LibPriority &value) {
    _priority = value;
}

const QString &LibInfo::getRPath() const
{
    return _rpath;
}

void LibInfo::setRPath(const QString &value)
{
    _rpath = value;
}

WinAPI LibInfo::getWinApi() const {
    return _winApi;
}

void LibInfo::setWinApi(WinAPI winApi) {
    _winApi = winApi;
}

QtMajorVersion LibInfo::isDependetOfQt() const {
    for (const auto& i : _dependncies) {
        if (QtMajorVersion result = DeployCore::isQtLibName(i)) {
            return result;
        }
    }

    return QtMajorVersion::NoQt;
}

QString LibInfo::toString() const {


#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    QStringList dependenciesList = _dependncies.toList();
#else
    QStringList dependenciesList{_dependncies.begin(), _dependncies.end()};
#endif

    return QString("LibInfo: path: '%0', name: '%1', qtPath: '%2', platform: '%3', dependencies: '%4'").
            arg(_path,
                _name,
                _rpath,
                DeployCore::platformToString(_platform),
                dependenciesList.join(", "));
}

QString LibInfo::fullPath() const {
    return _path + "/" + _name;
}

void LibInfo::clear() {
    _path = "";
    _name = "";
    _rpath = "";
    _platform = Platform::UnknownPlatform;
    _dependncies.clear();
    _allDep.clear();
}

bool LibInfo::isValid() const {
    return _platform != Platform::UnknownPlatform &&
            _name.size() && _path.size();
}

bool LibInfo::isScaned() const {
    return _allDep.size();
}

uint qHash(const LibInfo &info) {
    return qHash(info.fullPath());
}
