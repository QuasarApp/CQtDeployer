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

void IgnoreRule::check() {
    for (auto ignore : DeployCore::_config->ignoreList) {
        if (line.fullPath().contains(ignore)) {
            QuasarAppUtils::Params::verboseLog(line.fullPath() + " ignored by filter" + ignore);
            isIgnore = true;
            continue;
        }
    }
}

bool IgnoreRule::isIgnore(const LibInfo &info) {




    check();
}
