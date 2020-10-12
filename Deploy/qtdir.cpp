#include "pathutils.h"
#include "qtdir.h"

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
