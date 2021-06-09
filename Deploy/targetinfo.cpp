//#
//# Copyright (C) 2018-2021 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#include "targetinfo.h"
#include "deploycore.h"
#include "deployconfig.h"

TargetInfo::TargetInfo() {

}

TargetInfo::~TargetInfo() {
    if (_fEnableRunScript)
        delete _fEnableRunScript;
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
    return _fEnableShortCut;
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

    if (_fEnableRunScript) {
        QFileInfo runscriptInfo(_runScript);
        QFileInfo info(getName());

        return info.baseName() + "." + runscriptInfo.completeSuffix();
    }

    return getName();
}

bool TargetInfo::fEnableRunScript() const {

    if (!_fEnableRunScript) {
        QFileInfo info(fullPath());
        QString compleSufix = info.completeSuffix();
        return compleSufix.compare(".exe", Qt::CaseInsensitive) == 0 || compleSufix.isEmpty();
    }

    return _fEnableRunScript;
}

void TargetInfo::setFEnableRunScript(bool newFEnableRunScript) {
    if (!_fEnableRunScript)
        _fEnableRunScript = new bool;

    *_fEnableRunScript = newFEnableRunScript;
}

void TargetInfo::disableRunScript() {
    setFEnableRunScript(false);
}


