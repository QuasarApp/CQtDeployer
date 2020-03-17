/*
 * Copyright (C) 2018-2019 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "deploycore.h"
#include "metafilemanager.h"

#include <quasarapp.h>
#include <QDir>
#include <configparser.h>
#include "filemanager.h"

#include <assert.h>

bool MetaFileManager::createRunScriptWindows(const QString &target) {

    if (DeployCore::_config->distroStruct.getBinOutDir() ==
            DeployCore::_config->distroStruct.getLibOutDir() ) {
        return true;
    }

    QString content =
            "@echo off \n"
            "SET BASE_DIR=%~dp0\n"
            "SET PATH=%BASE_DIR%" + DeployCore::_config->distroStruct.getLibOutDir() + ";%PATH%\n"
            "call \"%BASE_DIR%" + DeployCore::_config->distroStruct.getBinOutDir() + "%0\" %1 \n";

    content = content.arg(QFileInfo(target).fileName()).arg("%*");
    content = QDir::toNativeSeparators(content);

    QString fname = DeployCore::_config->targetDir + QDir::separator() + QFileInfo(target).baseName()+ ".bat";

    QFile F(fname);
    if (!F.open(QIODevice::WriteOnly)) {
        return false;
    }

    F.write(content.toUtf8());
    F.flush();
    F.close();

    _fileManager->addToDeployed(fname);

    return F.setPermissions(QFileDevice::ExeOther | QFileDevice::WriteOther |
                            QFileDevice::ReadOther | QFileDevice::ExeUser |
                            QFileDevice::WriteUser | QFileDevice::ReadUser |
                            QFileDevice::ExeOwner | QFileDevice::WriteOwner |
                            QFileDevice::ReadOwner);
}

bool MetaFileManager::createRunScriptLinux(const QString &target) {
    QString content =
            "#!/bin/sh\n"
            "BASE_DIR=$(dirname \"$(readlink -f \"$0\")\")\n"
            "export "
            "LD_LIBRARY_PATH=\"$BASE_DIR\"" + DeployCore::_config->distroStruct.getLibOutDir() + ":\"$BASE_DIR\":$LD_LIBRARY_PATH\n"
            "export QML_IMPORT_PATH=\"$BASE_DIR\"" + DeployCore::_config->distroStruct.getQmlOutDir() + ":QML_IMPORT_PATH\n"
            "export QML2_IMPORT_PATH=\"$BASE_DIR\"" + DeployCore::_config->distroStruct.getQmlOutDir() + ":QML2_IMPORT_PATH\n"
            "export QT_PLUGIN_PATH=\"$BASE_DIR\"" + DeployCore::_config->distroStruct.getPluginsOutDir() + ":QT_PLUGIN_PATH\n"
            "export QTWEBENGINEPROCESS_PATH=\"$BASE_DIR\"" + DeployCore::_config->distroStruct.getBinOutDir() + "/QtWebEngineProcess\n"
            "export QTDIR=\"$BASE_DIR\"\n"
            "export "
            "QT_QPA_PLATFORM_PLUGIN_PATH=\"$BASE_DIR\"" + DeployCore::_config->distroStruct.getPluginsOutDir() +
            "/platforms:QT_QPA_PLATFORM_PLUGIN_PATH\n"
            "%2"
            "\"$BASE_DIR\"" + DeployCore::_config->distroStruct.getBinOutDir() + "%1 \"$@\"";

    content = content.arg(QFileInfo(target).fileName());
    int ld_index = DeployCore::find("ld-linux", _fileManager->getDeployedFilesStringList());

    if (ld_index >= 0 && QuasarAppUtils::Params::isEndable("deploySystem-with-libc")) {

        content = content.arg(QString("\nexport LD_PRELOAD=\"$BASE_DIR\"" + DeployCore::_config->distroStruct.getLibOutDir() + "%0\n").
            arg(QFileInfo(_fileManager->getDeployedFilesStringList()[ld_index]).fileName()));
    } else {
        content = content.arg("");
    }

    QString fname = DeployCore::_config->targetDir + QDir::separator() + QFileInfo(target).baseName()+ ".sh";

    QFile F(fname);
    if (!F.open(QIODevice::WriteOnly)) {
        return false;
    }

    F.write(content.toUtf8());
    F.flush();
    F.close();

    _fileManager->addToDeployed(fname);

    return F.setPermissions(QFileDevice::ExeOther | QFileDevice::WriteOther |
                            QFileDevice::ReadOther | QFileDevice::ExeUser |
                            QFileDevice::WriteUser | QFileDevice::ReadUser |
                            QFileDevice::ExeOwner | QFileDevice::WriteOwner |
                            QFileDevice::ReadOwner);
}

MetaFileManager::MetaFileManager(FileManager *manager):
    _fileManager(manager)
{
    assert(_fileManager);
}

bool MetaFileManager::createRunScript(const QString &target) {

    QFileInfo info(target);
    auto sufix = info.completeSuffix();

    if (sufix.contains("exe", Qt::CaseSensitive)) {
        return createRunScriptWindows(target);
    }

    if (sufix.isEmpty()) {
        return createRunScriptLinux(target);
    }

    return true;

}

bool MetaFileManager::createQConf() {

    QString content =
            "[Paths]\n"
            "Prefix= ." + DeployCore::_config->distroStruct.getRootDir(DeployCore::_config->distroStruct.getBinOutDir()) + "\n"
            "Libraries= ." + DeployCore::_config->distroStruct.getLibOutDir() + "\n"
            "Plugins= ." + DeployCore::_config->distroStruct.getPluginsOutDir() + "\n"
            "Imports= ." + DeployCore::_config->distroStruct.getQmlOutDir() + "\n"
            "Translations= ." + DeployCore::_config->distroStruct.getTrOutDir() + "\n"
            "Qml2Imports= ." + DeployCore::_config->distroStruct.getQmlOutDir() + "\n";


    content.replace("//", "/");
    content = QDir::fromNativeSeparators(content);

    QString fname = DeployCore::_config->targetDir + DeployCore::_config->distroStruct.getBinOutDir() + "qt.conf";

    QFile F(fname);
    if (!F.open(QIODevice::WriteOnly)) {
        return false;
    }

    F.write(content.toUtf8());
    F.flush();
    F.close();

    _fileManager->addToDeployed(fname);

    return F.setPermissions(QFileDevice::ExeOther | QFileDevice::WriteOther |
                            QFileDevice::ReadOther | QFileDevice::ExeUser |
                            QFileDevice::WriteUser | QFileDevice::ReadUser |
                            QFileDevice::ExeOwner | QFileDevice::WriteOwner |
                            QFileDevice::ReadOwner);
}

void MetaFileManager::createRunMetaFiles() {

    for (auto i = DeployCore::_config->targets.cbegin(); i != DeployCore::_config->targets.cend(); ++i) {

        if (!createRunScript(*i)) {
            qCritical() << "run script not created!";
        }
    }

    if (!createQConf()) {
        QuasarAppUtils::Params::verboseLog("create qt.conf failr", QuasarAppUtils::Warning);
    }
}
