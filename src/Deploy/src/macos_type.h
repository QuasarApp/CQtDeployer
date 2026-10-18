//#
//# Copyright (C) 2018-2026 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef MACOSTYPE_H
#define MACOSTYPE_H

#include "igetlibinfo.h"

/**
 * @brief The MacOS class
 *
 * Mach-O (macOS) binary scanner. It parses the Mach-O load commands with
 * the LIEF library and extracts the linked dylibs (LC_LOAD_DYLIB) and the
 * binary platform (issue #553).
 */
class DEPLOYSHARED_EXPORT MacOS : public IGetLibInfo
{
public:
    MacOS();

    bool getLibInfo(const QString &lib, LibInfo &info) const override;
};

#endif // MACOSTYPE_H