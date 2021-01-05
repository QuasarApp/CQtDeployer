//#
//# Copyright (C) 2018-2021 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#include "pathutils.h"

#include <QDir>
#include <QFileInfo>

PathUtils::PathUtils()
{

}

QString PathUtils::toFullPath(QString path) {
    path.replace('\\', '/');

    int index = -1;
    do {
        path.replace("//", "/");
    } while ((index = path.indexOf("//")) >= 0);

    if (path.right(1) != '/') {
        path.insert(path.size(), '/');
    }

    return path;
}

QString PathUtils::getRelativeLink(QString from, QString to) {

    auto mainDrive = getDrive(from);

    if (mainDrive != getDrive(to) && !mainDrive.isNull()) {
        return to;
    }

    bool isFile = QFileInfo(to).isFile();
    from = toFullPath(from);
    to = toFullPath(to);

    int lastGeneralDir = 0;
    int index = 0;

    while (from.size() > index && to.size() > index && from[index] == to[index]) {
        if (from[index] == '/') {
            lastGeneralDir = index;
        }
        index++;
    }

    auto result =  "." + getReleativePath(from.mid(lastGeneralDir)) +
            to.mid(lastGeneralDir + 1);

    if (isFile && result.right(1) == '/')
        result = result.left(result.size() - 1);

    return result;
}

// TODO ignore labels may be mistaken for a path, which will lead to improper eating
bool PathUtils::isPath(const QString &path) {
    QFileInfo info(path);
    return info.exists();
//    return path.contains('/') || path.contains('\\') || path == ".";
}

bool PathUtils::isReleativePath(const QString &path) {
    QString temp = QDir::fromNativeSeparators(path);
    if (temp.size() == 1) {
        return path[0] == '.';
    } else if (temp.size() > 1) {
        return path[0] == '.' && path[1] == '/';
    }

    return false;
}

QChar PathUtils::getDrive(QString path) {

    path = stripPath(path);

    if (path.size() > 1 && path[1] == ':') {
        return path[0];
    }

    return 0;
}

bool PathUtils::isAbsalutPath(const QString &path) {
    if (getDrive(path).isNull()) {
        return path.size() && (path.at(0) == "/" ||  path.at(0) == "\\");
    }

    return true;
}

QString PathUtils::fixPath(const QString &path) {
#ifdef Q_OS_WIN
    return stripPath(path.toUpper());
#else
    return stripPath(path);
#endif
}

QString PathUtils::getReleativePath(QString path) {
    path = toFullPath(path);

    int left = path.at(0) == '/';
    int righy = path.indexOf('/', left);

    while (righy > 0) {

        path.replace(left, righy - left, "..");

        left = left + 3;
        righy = path.indexOf('/', left);
    }

    return path;
}

QString PathUtils::stripPath(QString path) {
    path = toFullPath(path);

    if (path.right(1) == '/') {
        return path.remove(path.size() - 1, 1);
    }

    return path;
}

QString PathUtils::fullStripPath(QString path) {
    path = stripPath(path);

    if (path.left(1) == '/') {
        return path.remove(0, 1);
    }

    return path;
}
