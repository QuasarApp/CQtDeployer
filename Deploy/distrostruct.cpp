#include "distrostruct.h"

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
    libOutDir = value;
}

QString DistroStruct::getBinOutDir(const QString &basePath) const {
    return getRelativePath(basePath) + binOutDir;
}

void DistroStruct::setBinOutDir(const QString &value) {
    binOutDir = value;
}

QString DistroStruct::getQmlOutDir(const QString &basePath) const {
    return getRelativePath(basePath) + qmlOutDir;
}

void DistroStruct::setQmlOutDir(const QString &value) {
    qmlOutDir = value;
}

QString DistroStruct::getTrOutDir(const QString &basePath) const {
    return getRelativePath(basePath) + trOutDir;
}

void DistroStruct::setTrOutDir(const QString &value) {
    trOutDir = value;
}

QString DistroStruct::getResOutDeir(const QString &basePath) const {
    return getRelativePath(basePath) + resOutDeir;
}

void DistroStruct::setResOutDeir(const QString &value) {
    resOutDeir = value;
}

QString DistroStruct::getPluginsOutDir(const QString &basePath) const {
    return getRelativePath(basePath) + pluginsOutDir;
}

void DistroStruct::setPluginsOutDir(const QString &value) {
    pluginsOutDir = value;
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

DistroStruct::DistroStruct() {

}
