//#
//# Copyright (C) 2018-2020 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#include "targetinfo.h"

TargetInfo::TargetInfo() {

}

QString TargetInfo::getSufix() const {
    return sufix;
}

void TargetInfo::setSufix(const QString &value) {
    sufix = value;
}


