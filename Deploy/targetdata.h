/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef TARGETDATA_H
#define TARGETDATA_H

#include "targetinfo.h"


/**
 * @brief The TargetData struct simple info structo about target.
 */
struct TargetData {
    QString target;
    TargetInfo targetInfo;
};

#endif // TARGETDATA_H
