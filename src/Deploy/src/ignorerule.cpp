/*
 * Copyright (C) 2018-2026 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "ignorerule.h"
#include <quasarapp.h>

bool IgnoreRule::checkOnlytext(const QString &lib) {
    for (const auto &ignore : std::as_const(_data)) {
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
    if (info.fullPath().contains(ignoreLabel, DeployCore::getCaseSensitivity())) {
        QuasarAppUtils::Params::log(info.fullPath() + " ignored by filter" + ignoreLabel);
        return true;
    }

    return false;
}

const IgnoreData* IgnoreRule::isIgnore(const LibInfo &info) const {

    for (const auto &ignore : _data) {

        // MinGW runtime flavors (issue #855) must not prevent a platform
        // scoped rule (e.g. "Win") from matching flavored Windows libraries.
        // A rule that itself carries a MinGW flavor only matches libraries
        // of the identical flavor.
        const int minGWFlavors = int(Platform::MinGW_Msvcrt) | int(Platform::MinGW_Ucrt);
        const int libPlatform = int(info.getPlatform());
        const int rulePlatform = int(ignore.platform);

        bool checkPlatform = ignore.platform == UnknownPlatform;
        if (!checkPlatform) {
            const bool baseMatch = ((rulePlatform & ~minGWFlavors) &
                                    (libPlatform & ~minGWFlavors)) ==
                                   (libPlatform & ~minGWFlavors);
            const bool flavorMatch = !(rulePlatform & minGWFlavors) ||
                                     ((libPlatform & minGWFlavors) ==
                                      (rulePlatform & minGWFlavors));
            checkPlatform = baseMatch && flavorMatch;
        }
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
