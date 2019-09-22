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

bool IgnoreRule::check(const LibInfo &info, const QString& ignoreLabel) {
    if (info.fullPath().contains(ignoreLabel)) {
        QuasarAppUtils::Params::verboseLog(info.fullPath() + " ignored by filter" + ignoreLabel);
        return true;
    }

    return false;
}

bool IgnoreRule::isIgnore(const LibInfo &info) {

    for (auto &ignore : _data) {

        bool checkPlatform = ignore.platform == info.getPlatform() || ignore.platform == UnknownPlatform;
        bool checkPriority = ignore.prority <= info.getPriority();
        bool checkEnvirement = ignore.enfirement.

        if (checkPlatform && ignore.prority <= info.getPriority() && ) {

        }
        ignore.prority >
    }

    check();
}
