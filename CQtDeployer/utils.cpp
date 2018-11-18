/*
 * Copyright (C) 2018 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */
#include "utils.h"
#include <quasarapp.h>
#include <QDebug>

void Utils::verboseLog(const QString &str) {
    if (QuasarAppUtils::Params::isEndable("verbose")) {
        qDebug() << str;
    }
}

bool Utils::snapCd(QDir &dir, const QString& pathTo) {
    auto path = dir.absolutePath();

    verboseLog("call snapCd for  " + path + " to " + pathTo);

    dir.setPath(pathTo);
    if (dir.exists() && dir.isReadable()) {
        return true;
    }

    verboseLog("exits " + pathTo + " : " + ((dir.exists())? "True" : "false"));
    verboseLog("isReadable " + pathTo + " : " + ((dir.isReadable())? "True" : "false"));

    dir.setPath(path + "/" + pathTo);
    if (dir.exists() && dir.isReadable()) {
        return true;
    }

    verboseLog("exits (word)" + pathTo + " : " + ((dir.exists())? "True" : "false"));
    verboseLog("isReadable (word) " + pathTo + " : " + ((dir.isReadable())? "True" : "false"));

    dir.setPath(path);
    return false;
}

bool Utils::snapCdUp(QDir &dir) {

    auto path = dir.absolutePath();
    verboseLog("call snapCdUp for  " + path);

    auto parent = path.left(path.lastIndexOf('/'));
    dir.setPath(parent);
    if (dir.exists() && dir.isReadable()) {
        return true;
    }

    verboseLog("exits " + parent + " : " + ((dir.exists())? "True" : "false"));
    verboseLog("isReadable " + parent + " : " + ((dir.isReadable())? "True" : "false"));

    dir.setPath(path);
    return false;
}

Utils::Utils() {}

bool Utils::cd(QDir &dir, const QString& pathTo) {
#ifdef Q_OS_LINUX_SNAP
    return snapCd(dir, pathTo);
#else
    return dir.cd(pathTo);
#endif
}

bool Utils::cdUp(QDir &dir) {
#ifdef Q_OS_LINUX_SNAP
    return snapCdUp(dir);
#else
    return dir.cdUp();
#endif
}
