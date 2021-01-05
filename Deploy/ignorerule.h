//#
//# Copyright (C) 2018-2021 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef IGNORERULE_H
#define IGNORERULE_H

#include "envirement.h"
#include "libinfo.h"

#include <QString>
#include <deploycore.h>


/**
 * @brief The IgnoreData struct
 * ignore file with label and othe rooles
 */
struct IgnoreData{
    IgnoreData(const QString& label = "");

    QString label;
    Platform platform = UnknownPlatform;
    LibPriority prority = NotFile;
    Envirement enfirement;
};


class IgnoreRule
{
private:
    QList<IgnoreData> _data;

    bool checkOnlytext(const QString& lib);

    bool check(const LibInfo &info, const QString &ignoreLabel) const;
public:
    IgnoreRule();
    void addRule(const IgnoreData& rule);

    /**
     * @brief isIgnore
     * @param info
     * @return const ptr to ignore data
     */
    const IgnoreData *isIgnore(const LibInfo& info) const;
};

#endif // IGNORERULE_H
