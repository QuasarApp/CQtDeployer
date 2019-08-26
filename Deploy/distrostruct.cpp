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

QString DistroStruct::getResOutDeir(const QString &basePath) const {
    return getRelativePath(basePath) + resOutDeir;
}

void DistroStruct::setResOutDeir(const QString &value) {
    resOutDeir = fixPath(value);
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

    int count = path.count('/') - 1;

    for (int i = 0; i < count; ++i) {
        path += "../";
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

    QuasarAppUtils::Params::isEndable()

    setBinOutDir("/bin");
    setLibOutDir("/lib");

#else
    setBinOutDir("/");
    setLibOutDir("/");

    appDir = QuasarAppUtils::Params::getStrArg("appPath");
#endif

    setQmlOutDir("/qml");
    setResOutDeir("/res");
    setPluginsOutDir("/plugins");
    setTrOutDir("/translations");


}
