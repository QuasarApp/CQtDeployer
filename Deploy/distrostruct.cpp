#include "distrostruct.h"
#include <quasarapp.h>

#include <QRegExp>

//#
//# Copyright (C) 2018-2019 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

QString DistroStruct::getLibOutDir(const QString &basePath) const {
    return getRelativePath(basePath) + libOutDir;
}

void DistroStruct::setLibOutDir(const QString &value) {
    libOutDir = fixPath(value);
}

QString DistroStruct::getBinOutDir(const QString &basePath) const {
    return getRelativePath(basePath) + binOutDir;
}

void DistroStruct::setBinOutDir(const QString &value) {
    binOutDir = fixPath(value);
}

QString DistroStruct::getQmlOutDir(const QString &basePath) const {
    return getRelativePath(basePath) + qmlOutDir;
}

void DistroStruct::setQmlOutDir(const QString &value) {
    qmlOutDir = fixPath(value);
}

QString DistroStruct::getTrOutDir(const QString &basePath) const {
    return getRelativePath(basePath) + trOutDir;
}

void DistroStruct::setTrOutDir(const QString &value) {
    trOutDir = fixPath(value);
}

QString DistroStruct::getResOutDir(const QString &basePath) const {
    return getRelativePath(basePath) + resOutDir;
}

void DistroStruct::setResOutDir(const QString &value) {
    resOutDir = fixPath(value);
}

QString DistroStruct::getPluginsOutDir(const QString &basePath) const {
    return getRelativePath(basePath) + pluginsOutDir;
}

void DistroStruct::setPluginsOutDir(const QString &value) {
    pluginsOutDir = fixPath(value);
}

QString DistroStruct::getRootDir(const QString &basePath) const {
    return getRelativePath(basePath);
}

QString DistroStruct::getRelativePath(QString path) const {

    path.replace('\\', '/');

    int index = -1;
    do {
        path.replace("//", "/");
    } while ((index = path.indexOf("//")) >= 0);

    if (path.left(1) != '/') {
        path.insert(0, '/');
    }

    if (path.right(1) != '/') {
        path.insert(path.size(), '/');
    }

    int left = path.indexOf('/', 0) + 1;
    int righy = path.indexOf('/', left);

    while (righy > 0) {

        path.replace(left, righy - left, "..");

        left = left + 3;
        righy = path.indexOf('/', left);
    }

    return path;
}

QString DistroStruct::fixPath(const QString &path) const{

    if (path.right(1) != '/')
        return path + '/';

    return path;
}

DistroStruct::DistroStruct() {

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
