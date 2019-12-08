
//#
//# Copyright (C) 2018-2020 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#include "distrostruct.h"
#include "pathutils.h"
#include <quasarapp.h>

QString DistroStruct::getLibOutDir(const QString &basePath) const {
    return getRelativePath(basePath) + libOutDir;
}

void DistroStruct::setLibOutDir(const QString &value) {
    libOutDir = toFullPath(value);
}

QString DistroStruct::getBinOutDir(const QString &basePath) const {
    return getRelativePath(basePath) + binOutDir;
}

void DistroStruct::setBinOutDir(const QString &value) {
    binOutDir = toFullPath(value);
}

QString DistroStruct::getQmlOutDir(const QString &basePath) const {
    return getRelativePath(basePath) + qmlOutDir;
}

void DistroStruct::setQmlOutDir(const QString &value) {
    qmlOutDir = toFullPath(value);
}

QString DistroStruct::getTrOutDir(const QString &basePath) const {
    return getRelativePath(basePath) + trOutDir;
}

void DistroStruct::setTrOutDir(const QString &value) {
    trOutDir = toFullPath(value);
}

QString DistroStruct::getResOutDir(const QString &basePath) const {
    return getRelativePath(basePath) + resOutDir;
}

void DistroStruct::setResOutDir(const QString &value) {
    resOutDir = toFullPath(value);
}

QString DistroStruct::getPluginsOutDir(const QString &basePath) const {
    return getRelativePath(basePath) + pluginsOutDir;
}

void DistroStruct::setPluginsOutDir(const QString &value) {
    pluginsOutDir = toFullPath(value);
}

QString DistroStruct::getRootDir(const QString &basePath) const {
    return getRelativePath(basePath);
}

void DistroStruct::init() {

#ifdef Q_OS_LINUX

    setBinOutDir(QuasarAppUtils::Params::getStrArg("binOut", "/bin"));
    setLibOutDir(QuasarAppUtils::Params::getStrArg("libOut", "/lib"));

#else
    setBinOutDir(QuasarAppUtils::Params::getStrArg("binOut", "/"));
    setLibOutDir(QuasarAppUtils::Params::getStrArg("libOut", "/"));
#endif


    setQmlOutDir(QuasarAppUtils::Params::getStrArg("qmlOut", "/qml"));
    setTrOutDir(QuasarAppUtils::Params::getStrArg("trOut", "/translations"));
    setPluginsOutDir(QuasarAppUtils::Params::getStrArg("pluginOut", "/plugins"));
    setResOutDir("/resources");

}

QString DistroStruct::toFullPath(QString path) const {
    return PathUtils::toFullPath(path);
}

QString DistroStruct::stripPath(QString path) const {
    return PathUtils::stripPath(path);
}

QString DistroStruct::getRelativePath(QString path) const {
    return PathUtils::getReleativePath(path);
}

DistroStruct::DistroStruct() {


}
