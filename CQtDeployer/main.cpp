/*
 * Copyright (C) 2018-2019 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "deploy.h"
#include "quasarapp.h"
#include "deployutils.h"
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QList>


int main(int argc, const char *argv[]) {

    QCoreApplication::setOrganizationName("QuasarApp");
    QCoreApplication::setOrganizationDomain("https://github.com/QuasarApp");
    QCoreApplication::setApplicationName("CQtDeployer");

    if (!QuasarAppUtils::Params::parseParams(argc, argv)) {
        qWarning() << "wrong parametrs";
        DeployUtils::help();
        exit(0);
    };

    if (!(QuasarAppUtils::Params::isEndable("v") ||
            QuasarAppUtils::Params::isEndable("version"))) {
        DeployUtils::printVersion();
        exit(0);
    }

    if (QuasarAppUtils::Params::isEndable("h") ||
        QuasarAppUtils::Params::isEndable("help")) {
        DeployUtils::help();
        exit(0);
    }

    Deploy deploy;

    if (!DeployUtils::parseQt(&deploy)) {
        qCritical() << "error parse imput data";
        DeployUtils::help();
        exit(1);
    }

    deploy.deploy();

    qInfo() << "deploy done!";

    return 0;
}
