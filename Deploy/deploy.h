/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef DEPLOY_H
#define DEPLOY_H

#include "deploy_global.h"


class ConfigParser;
class Extracter;
class FileManager;
class DependenciesScanner;
class PluginsParser;
class Packing;


enum exitCodes {
    Good =          0x0,
    PrepareError =  0x1,
    DeployError =   0x2,
    PackingError =  0x3,


};

class DEPLOYSHARED_EXPORT Deploy
{
private:

    ConfigParser * _paramsParser = nullptr;
    Extracter *_extracter = nullptr;
    FileManager *_fileManager = nullptr;
    DependenciesScanner *_scaner = nullptr;
    PluginsParser *_pluginParser = nullptr;

    Packing *_packing = nullptr;

    bool prepare();
    bool deploy();
    bool packing();


public:
    Deploy();
    int run();
    ~Deploy();

    friend class deploytest;
};

#endif // DEPLOY_H
