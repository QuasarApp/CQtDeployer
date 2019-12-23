/*
 * Copyright (C) 2018-2020 QuasarApp.
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
    return left.priority < right.priority;
}

bool operator >(const LibInfo &left, const LibInfo &right) {
    return left.priority > right.priority;
}

const QSet<LibInfo> &LibInfo::getAllDep() const {
    return allDep;
}

Platform LibInfo::getPlatform() const {
    return platform;
}

void LibInfo::setPlatform(const Platform &value) {
    platform = value;
}

QString LibInfo::getName() const {
    return name;
}

void LibInfo::setName(const QString &value) {
    name = value;
}

QString LibInfo::getPath() const {
    return path;
}

void LibInfo::setPath(const QString &value) {
    path = value;
}

void LibInfo::addDependncies(const QString &value) {
    dependncies.insert(value);
}

void LibInfo::removeDependncies(const QString &value) {
    dependncies.remove(value);
}

QSet<QString> LibInfo::getDependncies() const {
    return dependncies;
}

void LibInfo::setDependncies(const QSet<QString> &value) {
    dependncies = value;
}

LibPriority LibInfo::getPriority() const {
    return priority;
}

void LibInfo::setPriority(const LibPriority &value) {
    priority = value;
}

QString LibInfo::getQtPath() const
{
    return qtPath;
}

void LibInfo::setQtPath(const QString &value)
{
    qtPath = value;
}

QString LibInfo::fullPath() const {
    return path + "/" + name;
}

void LibInfo::clear() {
    path = "";
    name = "";
    qtPath = "";
    platform = Platform::UnknownPlatform;
    dependncies.clear();
    allDep.clear();
}

bool LibInfo::isValid() const {
    return platform != Platform::UnknownPlatform &&
            name.size() && path.size();
}

uint qHash(const LibInfo &info) {
    return qHash(info.fullPath());
}
