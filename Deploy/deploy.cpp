/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "configparser.h"
#include "deploy.h"
#include "extracter.h"
#include "filemanager.h"
#include "packing.h"
#include "pluginsparser.h"
#include <quasarapp.h>

Deploy::Deploy() {
    _fileManager = new FileManager();
    _scaner = new DependenciesScanner();
    _packing = new Packing(_fileManager);
    _pluginParser = new PluginsParser();

    _paramsParser = new ConfigParser(_fileManager, _pluginParser, _scaner, _packing);

}

int Deploy::run() {

    if (!prepare()) {
        return PrepareError;
    }

    _fileManager->loadDeployemendFiles(_paramsParser->config()->getTargetDir());

    if (!deploy()) {
        return DeployError;
    }

    if (!packing()) {
        _fileManager->saveDeploymendFiles(_paramsParser->config()->getTargetDir());
        return PackingError;
    }
    _fileManager->saveDeploymendFiles(_paramsParser->config()->getTargetDir());

    return Good;
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

    if (_packing) {
        delete _packing;
    }

    if (_pluginParser) {
        delete _pluginParser;
    }

    DeployCore::_config = nullptr;
}

bool Deploy::prepare() {


    if ( !_paramsParser->parseParams()) {
        return false;
    }

    _extracter = new Extracter(_fileManager, _pluginParser, _paramsParser, _scaner);

    return true;
}

bool Deploy::deploy() {

    _extracter->clear();

    switch (DeployCore::getMode() ) {
    case RunMode::Deploy:
        if (!_extracter->deploy())
            return false;

        break;
    default:
        break;
    }

    return true;
}

bool Deploy::packing() {

    switch (DeployCore::getMode() ) {

    case RunMode::Deploy:
        return _packing->create();

    case RunMode::Template:
        return _packing->extractTemplates();

    default:
        break;
    }

    return true;
}
