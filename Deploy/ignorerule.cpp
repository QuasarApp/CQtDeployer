#include "ignorerule.h"
#include <quasarapp.h>

bool IgnoreRule::checkOnlytext(const QString &lib) {
    for (auto ignore : _data) {
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
    if (info.fullPath().contains(ignoreLabel)) {
        QuasarAppUtils::Params::verboseLog(info.fullPath() + " ignored by filter" + ignoreLabel);
        return true;
    }

    return false;
}

bool IgnoreRule::isIgnore(const LibInfo &info) const {

    for (auto &ignore : _data) {

        bool checkPlatform = ignore.platform == info.getPlatform() || ignore.platform == UnknownPlatform;
        bool checkPriority = ignore.prority <= info.getPriority();
        bool checkEnvirement = ignore.enfirement.inThisEnvirement(info.fullPath());

        if (checkPlatform && checkPriority && checkEnvirement) {
            return check(info, ignore.label);
        }
    }

    return false;
}

IgnoreData::IgnoreData(const QString &label) {
    this->label = label;
}
