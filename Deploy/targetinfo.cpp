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

QString TargetInfo::getIcon() const {
    if (icon.isEmpty()) {
        QFileInfo info(getName());
        if (info.suffix() == "exe") {
            return ":/shared/Distributions/Templates/Icon.ico";
        }

        return ":/shared/Distributions/Templates/Icon.png";

    }

    return icon;
}

void TargetInfo::setIcon(const QString &value) {
    icon = value;
}


