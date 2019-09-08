/*
 * Copyright (C) 2018-2019 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "cqt.h"
#include "deploy.h"
#include "extracter.h"
#include "filemanager.h"
#include <quasarapp.h>

Deploy::Deploy() {
    _fileManager = new FileManager();
    _paramsParser = new CQT(_fileManager);
    _extracter = new Extracter(_fileManager);
}

bool Deploy::prepare() {
    return _paramsParser->parseParams();
}

int Deploy::deploy() {
    _fileManager->loadDeployemendFiles(_paramsParser->config()->targetDir);
    _extracter->deploy();
    _fileManager->saveDeploymendFiles(_paramsParser->config()->targetDir);

    return 0;
}
