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
#include "packing.h"
#include <quasarapp.h>

Deploy::Deploy() {
    _fileManager = new FileManager();
    _scaner = new DependenciesScanner();
    _packing = new Packing();
    _paramsParser = new ConfigParser(_fileManager, _scaner, _packing);

    connect(this, &Deploy::sigStart, this, &Deploy::handleStart, Qt::QueuedConnection);
}

int Deploy::run(bool async) {
    if (async) {
        emit sigStart();
        return 0;
    }

    return runPrivate();
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

bool Deploy::deploy() {

    _fileManager->loadDeployemendFiles(_paramsParser->config()->getTargetDir());

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
    _fileManager->saveDeploymendFiles(_paramsParser->config()->getTargetDir());

    return true;
}

bool Deploy::packing(bool async) {
    if (async) {
        connect(_packing, &Packing::sigFinished, this, &Deploy::sigFinish, Qt::QueuedConnection);
    }

    return _packing->create(async);
}

int Deploy::runPrivate(bool async) {
    if (!prepare()) {
        return 1;
    }

    if (!deploy()) {
        return 2;
    }

    if (!packing(async))
        return 3;

    return 0;
}

void Deploy::handleStart() {
    if (int exit = runPrivate(true)) {
        emit sigFinish(exit);
    }
}
