/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef DEPLOY_H
#define DEPLOY_H

#include "deploy_global.h"

#include <QObject>



class ConfigParser;
class Extracter;
class FileManager;
class DependenciesScanner;
class Packing;

enum exitCodes {
    Good =          0x0,
    PrepareError =  0x1,
    DeployError =   0x2,
    PackingError =  0x3,

    ASync = 0x10,

    ASyncGood = Good | ASync,

};

class DEPLOYSHARED_EXPORT Deploy : public QObject
{
    Q_OBJECT
private:

    ConfigParser * _paramsParser = nullptr;
    Extracter *_extracter = nullptr;
    FileManager *_fileManager = nullptr;
    DependenciesScanner *_scaner = nullptr;
    Packing *_packing = nullptr;

    bool prepare();
    bool deploy();
    bool packing(bool async);
    int runPrivate(bool async = false);

private slots:
    void handleStart();

signals:
    void sigFinish(int code);
    void sigStart();

public:
    Deploy();
    int run(bool async = false);
    ~Deploy();


    friend class deploytest;
};

#endif // DEPLOY_H
