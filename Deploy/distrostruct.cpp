#include "distrostruct.h"

//#
//# Copyright (C) 2018-2019 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

QString DistroStruct::getLibOutDir() const {
    return libOutDir;
}

void DistroStruct::setLibOutDir(const QString &value) {
    libOutDir = value;
}

QString DistroStruct::getBinOutDir() const {
    return binOutDir;
}

void DistroStruct::setBinOutDir(const QString &value) {
    binOutDir = value;
}

QString DistroStruct::getQmlOutDir() const {
    return qmlOutDir;
}

void DistroStruct::setQmlOutDir(const QString &value) {
    qmlOutDir = value;
}

QString DistroStruct::getTrOutDir() const {
    return trOutDir;
}

void DistroStruct::setTrOutDir(const QString &value) {
    trOutDir = value;
}

QString DistroStruct::getResOutDeir() const {
    return resOutDeir;
}

void DistroStruct::setResOutDeir(const QString &value) {
    resOutDeir = value;
}

QString DistroStruct::getPluginsOutDir() const {
    return pluginsOutDir;
}

void DistroStruct::setPluginsOutDir(const QString &value) {
    pluginsOutDir = value;
}

DistroStruct::DistroStruct() {

}
