/*
 * Copyright (C) 2018-2019 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef DEPLOY_H
#define DEPLOY_H

#include "deploy_global.h"

class CQT;
class Extracter;
class FileManager;

class DEPLOYSHARED_EXPORT Deploy
{
private:

    CQT * _paramsParser = nullptr;
    Extracter *_extracter = nullptr;
    FileManager *_fileManager = nullptr;

    bool prepare();
    int deploy();

public:
    Deploy();
    int run();
    ~Deploy();


};

#endif // DEPLOY_H
