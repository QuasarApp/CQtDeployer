//#
//# Copyright (C) 2018-2021 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#include "targetinfo.h"

TargetInfo::TargetInfo() {

}

QString TargetInfo::getPackage() const {
    return package;
}

void TargetInfo::setPackage(const QString &value) {
    package = value;
}


