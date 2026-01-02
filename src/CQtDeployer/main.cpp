/*
 * Copyright (C) 2018-2026 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

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
        qWarning() << "Wrong parameters. Please use the 'help' of 'h' option to show the help page.";
        exit(4);
    }

    Deploy deploy;

    deploy.init();

    int code = deploy.run();

    if (code) {
        qCritical() << Deploy::codeString(code);
    }

    return code;
}
