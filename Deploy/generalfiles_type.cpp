//#
//# Copyright (C) 2021-2021 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#include "generalfiles_type.h"

GeneralFiles::GeneralFiles()
{

}

bool GeneralFiles::getLibInfo(const QString &lib, LibInfo &info) const {

    QFileInfo fileInfo(lib);

    info.setPlatform(GeneralFile);
    info.setName(fileInfo.fileName());
    info.setPath(fileInfo.absolutePath());

    return true;
}
