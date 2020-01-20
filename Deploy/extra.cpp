#include "extra.h"
#include "pathutils.h"

#include <QFileInfo>



QSet<QString> Extra::getExtraPathsMasks() const {
    return extraPathsMasks;
}

void Extra::setExtraPathsMasks(const QSet<QString> &value) {
    extraPathsMasks = upper(value);
}

void Extra::addExtraPathsMasks(const QSet<QString> &value) {
    extraPathsMasks += upper(value);
}

QSet<QString> Extra::getExtraNamesMasks() const {
    return extraNamesMasks;
}

void Extra::setExtraNamesMasks(const QSet<QString> &value) {
    extraNamesMasks = upper(value);
}

void Extra::addtExtraNamesMasks(const QSet<QString> &value) {
    extraNamesMasks += upper(value);
}

QSet<QString> Extra::upper(const QSet<QString>& set) const {
    QSet<QString> res;
    for (const auto &i : set) {
        res.insert(PathUtils::fixPath(i));
    }
    return res;
}

QSet<QString> Extra::getExtraPaths() const {
    return extraPaths;
}

void Extra::setExtraPaths(const QSet<QString> &value) {
    extraPaths = upper(value);
}

void Extra::addExtraPaths(const QSet<QString> &value) {
    extraPaths += upper(value);
}


bool Extra::contains(const QString &path) const {
    QFileInfo info(path);
    if (extraPaths.contains(PathUtils::fixPath(info.absolutePath()))) {
        return true;
    }

    for (auto i: extraPathsMasks) {
        if (PathUtils::fixPath(info.absoluteFilePath()).contains(i)) {
            return true;
        }
    }

    for (auto i: extraNamesMasks) {
        if (PathUtils::fixPath(info.fileName()).contains(i)) {
            return true;
        }
    }

    return false;
}
