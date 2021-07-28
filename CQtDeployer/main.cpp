/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "extracter.h"
#include "quasarapp.h"
#include "deploycore.h"
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QList>
#include <deploy.h>

int main(int argc, char *argv[]) {

    QCoreApplication::setOrganizationName("QuasarApp");
    QCoreApplication::setOrganizationDomain("https://github.com/QuasarApp");
    QCoreApplication::setApplicationName("CQtDeployer");


    if (!QuasarAppUtils::Params::parseParams(argc, argv, DeployCore::avilableOptions())) {
        QuasarAppUtils::Params::log("Wrong parameters. Please use the 'help' of 'h' option for show help page.", QuasarAppUtils::Warning);
        exit(4);
    }

    Deploy deploy;

    int code = deploy.run();

    if (code) {
        QuasarAppUtils::Params::log(Deploy::codeString(code), QuasarAppUtils::Error);
    }

    return code;
}
