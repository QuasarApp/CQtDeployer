/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "ignorerule.h"
#include <quasarapp.h>

bool IgnoreRule::checkOnlytext(const QString &lib) {
    for (const auto &ignore : qAsConst(_data)) {
        if (lib.contains(ignore.label)) {
            return true;
        }
    }

    return false;
}

IgnoreRule::IgnoreRule() {

}

void IgnoreRule::addRule(const IgnoreData &rule) {
    _data.push_back(rule);
}

bool IgnoreRule::check(const LibInfo &info, const QString& ignoreLabel) const {
    if (info.fullPath().contains(ignoreLabel, ONLY_WIN_CASE_INSENSIATIVE)) {
        QuasarAppUtils::Params::log(info.fullPath() + " ignored by filter" + ignoreLabel);
        return true;
    }

    return false;
}

const IgnoreData* IgnoreRule::isIgnore(const LibInfo &info) const {

    for (const auto &ignore : _data) {

        bool checkPlatform = ((ignore.platform & info.getPlatform()) == info.getPlatform()) || ignore.platform == UnknownPlatform;
        bool checkPriority = (ignore.prority <= info.getPriority()) || ignore.prority == NotFile;
        bool checkEnvirement = !ignore.enfirement.size() || ignore.enfirement.inThisEnvirement(info.fullPath());

        if (checkPlatform && checkPriority && checkEnvirement && check(info, ignore.label)) {
            return &ignore;
        }

    }

    return nullptr;
}

IgnoreData::IgnoreData(const QString &label) {
    this->label = label;
}
