/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "packagecontrol.h"

#include <QDir>
#include <QFileInfo>


PackageControl::PackageControl()
{

}

PackageControl::~PackageControl(){

}

bool PackageControl::isEmpty(const QString &package) const {
    QFileInfo info(package);
    QDir dirInfo(package);
    return !(info.exists() && dirInfo.entryList(QDir::AllEntries | QDir::NoDotAndDotDot).size());
}
