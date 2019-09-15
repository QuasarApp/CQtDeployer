/*
 * Copyright (C) 2018-2019 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "configparser.h"
#include "deploy.h"
#include "extracter.h"
#include "filemanager.h"
#include <quasarapp.h>

Deploy::Deploy() {
    _fileManager = new FileManager();
    _paramsParser = new ConfigParser(_fileManager);
}

int Deploy::run() {

    if (!prepare()) {
        return 1;
    }

    return deploy();
}

Deploy::~Deploy() {

    if (_extracter) {
        delete _extracter;
    }

    if (_paramsParser) {
        delete _paramsParser;
    }

    if (_fileManager) {
        delete _fileManager;
    }
}

bool Deploy::prepare() {
    if ( !_paramsParser->parseParams()) {
        return false;
    }

    _extracter = new Extracter(_fileManager, _paramsParser);

    return true;
}

int Deploy::deploy() {

    if (DeployCore::getMode() != RunMode::Deploy) {
        return 0;
    }

    _fileManager->loadDeployemendFiles(_paramsParser->config()->targetDir);
    _extracter->deploy();
    _fileManager->saveDeploymendFiles(_paramsParser->config()->targetDir);

    return 0;
}
