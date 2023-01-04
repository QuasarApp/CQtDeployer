/*
 * Copyright (C) 2018-2023 QuasarApp.
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

/**
 * @brief The exitCodes enum contains all general erro codes of the CQtDeployer tool.
 */
enum exitCodes {
    /// CQtDeployer are  deployed project successful. (no error)
    Good =          0x0,
    /// CQtDeployer failed in the reading arguments. (fail to parse of input arguments)
    PrepareError =  0x1,
    /// CQtDeployer failed in the deploy step. Fail to copy deployed files.
    DeployError =   0x2,
    /// CQtDeployer failed in the prepare package step. Fail to create a result pacakge.
    PackingError =  0x3,


};

inline void initCQtDeployerResources() {
    Q_INIT_RESOURCE(DeployResources);
}

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
    bool init();
    ~Deploy();

    /**
     * @brief codeString This method retun string message about input code.
     * @param code This is error code;
     * @return Message of code.
     */
    static QString codeString(int code);
    friend class deploytest;
    friend class CheckQtTest;
    friend class QIFWBinaryCreatorTest;
};

#endif // DEPLOY_H
