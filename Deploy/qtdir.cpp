/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "pathutils.h"
#include "qtdir.h"

QtMajorVersion QtDir::getQtVersion() const {
    return _qtVersion;
}

void QtDir::setQtVersion(const QtMajorVersion &qtVersion) {
    _qtVersion = qtVersion;
}

QString QtDir::getLibs() const {
    return libs;
}

void QtDir::setLibs(const QString &value) {
    libs = PathUtils::fixPath(value);
}

QString QtDir::getBins() const {
    return bins;
}

void QtDir::setBins(const QString &value) {
    bins =  PathUtils::fixPath(value);
}

QString QtDir::getLibexecs() const {
    return libexecs;
}

void QtDir::setLibexecs(const QString &value) {
    libexecs =  PathUtils::fixPath(value);
}

QString QtDir::getPlugins() const {
    return plugins;
}

void QtDir::setPlugins(const QString &value) {
    plugins =  PathUtils::fixPath(value);
}

QString QtDir::getQmls() const {
    return qmls;
}

void QtDir::setQmls(const QString &value)
{
    qmls =  PathUtils::fixPath(value);
}

QString QtDir::getTranslations() const {
    return translations;
}

void QtDir::setTranslations(const QString &value) {
    translations =  PathUtils::fixPath(value);
}

QString QtDir::getResources() const{
    return resources;
}

void QtDir::setResources(const QString &value) {
    resources =  PathUtils::fixPath(value);
}

Platform QtDir::getQtPlatform() const {
    return qtPlatform;
}

void QtDir::setQtPlatform(const Platform &value) {
    qtPlatform = value;
}

bool QtDir::isQt(QString path) const {

    path =  PathUtils::fixPath(path);

    return
        (!libs.isEmpty() && path.contains(libs, ONLY_WIN_CASE_INSENSIATIVE)) ||
        (!bins.isEmpty() && path.contains(bins, ONLY_WIN_CASE_INSENSIATIVE)) ||
        (!libexecs.isEmpty() && path.contains(libexecs, ONLY_WIN_CASE_INSENSIATIVE)) ||
        (!plugins.isEmpty() && path.contains(plugins, ONLY_WIN_CASE_INSENSIATIVE)) ||
        (!qmls.isEmpty() && path.contains(qmls, ONLY_WIN_CASE_INSENSIATIVE)) ||
        (!translations.isEmpty() && path.contains(translations, ONLY_WIN_CASE_INSENSIATIVE)) ||
        (!resources.isEmpty() && path.contains(resources, ONLY_WIN_CASE_INSENSIATIVE));
}
