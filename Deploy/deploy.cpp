/*
 * Copyright (C) 2018-2020 QuasarApp.
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
    _scaner = new DependenciesScanner();
    _paramsParser = new ConfigParser(_fileManager, _scaner);

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

    if (_scaner) {
        delete _scaner;
    }
}

bool Deploy::prepare() {
    if ( !_paramsParser->parseParams()) {
        return false;
    }

    _extracter = new Extracter(_fileManager, _paramsParser, _scaner);

    return true;
}

int Deploy::deploy() {

    _fileManager->loadDeployemendFiles(_paramsParser->config()->targetDir);

    switch (DeployCore::getMode() ) {
    case RunMode::Deploy:
        _extracter->deploy();
        break;
    case RunMode::Clear:
        _extracter->clear();
        break;
    default:
        break;
    }
    _fileManager->saveDeploymendFiles(_paramsParser->config()->targetDir);

    return 0;
}
