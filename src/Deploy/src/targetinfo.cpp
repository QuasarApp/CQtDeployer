//#
//# Copyright (C) 2018-2023 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#include "targetinfo.h"

TargetInfo::TargetInfo() {

}

const QString & TargetInfo::getPackage() const {
    return _package;
}

void TargetInfo::setPackage(const QString &value) {
    _package = value;
}

QString TargetInfo::getIcon() const {
    if (_icon.isEmpty()) {
        QFileInfo info(getName());
        if (info.suffix() == "exe") {
            return ":/shared/Distributions/Templates/Icon.ico";
        }

        return ":/shared/Distributions/Templates/Icon.png";

    }

    return _icon;
}

void TargetInfo::setIcon(const QString &value) {
    _icon = value;
}

bool TargetInfo::getShortCut() const {
    QFileInfo info(fullPath());
    QString compleSufix = info.completeSuffix();

    bool defaultRule = compleSufix.compare("exe", Qt::CaseInsensitive) == 0 || compleSufix.isEmpty();

    return _fEnableShortCut && defaultRule;
}

void TargetInfo::setShortCut(bool shortcut) {
    _fEnableShortCut = shortcut;
}

void TargetInfo::disableShortCut() {
    setShortCut(false);
}

QString TargetInfo::getRunScript() const {
    if (_runScript.isEmpty()) {
        QFileInfo info(getName());
        if (info.suffix() == "exe") {
            return ":/Scripts/ScriptsTemplates/windows.bat";
        }

        return ":/Scripts/ScriptsTemplates/linux.sh";
    }

    return _runScript;
}

void TargetInfo::setRunScript(const QString &newRunScript) {
    _runScript = newRunScript;
}

QString TargetInfo::getRunScriptFile() const {

    if (fEnableRunScript()) {
        QFileInfo runscriptInfo(getRunScript());
        QFileInfo info(getName());

        return info.baseName() + "." + runscriptInfo.completeSuffix();
    }

    return getName();
}

bool TargetInfo::fEnableRunScript() const {
    QFileInfo info(fullPath());
    QString compleSufix = info.completeSuffix();

    bool defaultRule = compleSufix.compare("exe", Qt::CaseInsensitive) == 0 || compleSufix.isEmpty();

    return _fEnableRunScript && defaultRule;
}

void TargetInfo::setFEnableRunScript(bool newFEnableRunScript) {
    _fEnableRunScript = newFEnableRunScript;
}

void TargetInfo::disableRunScript() {
    setFEnableRunScript(false);
}
