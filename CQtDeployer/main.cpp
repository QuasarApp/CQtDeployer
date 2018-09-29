/*
 * Copyright (C) 2018 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include <QCoreApplication>
#include "quasarapp.h"
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include "deploy.h"
#include <QList>

void help() {
    qInfo() << "";
    qInfo() << "Usage: CDQ <-bin    [params]> [options]";
    qInfo() << "";
    qInfo() << "Options:";
    qInfo() << "   help / h                 : show help.";
    qInfo() << "   always-overwrite         : Copy files even if the target file exists.";
    qInfo() << "   -bin    [params]         : deployment binry.";
    qInfo() << "   -qmlDir [params]         : qml datadir of project. for example -qmlDir ~/my/project/qml";
    qInfo() << "   noStrip                  : no strip deployed lib";
    qInfo() << "   deploy-not-qt            : deploy all libs";
    qInfo() << "   -qmake  [params]         : qmake path. for example";
    qInfo() << "                            | -qmake ~/Qt/5.11.1/gcc_64/bin/qmake";
    qInfo() << "   ignoreCudaLib            : it filter ignore cuda lib of nvidea";
    qInfo() << "   -ignore [list,params]    : ignore filter for libs";
    qInfo() << "                            | for example -ignore libicudata.so.56,libicudata2.so.56";
    qInfo() << "   clear                    : delete all old deploy data";
    qInfo() << "   -runScript [params]      : set new name of out file (AppRun.sh by default)";
    qInfo() << "                            | for example -runScript myApp.sh";
    qInfo() << "   allQmlDependes           : This flag will force to extract all qml libraries.";
    qInfo() << "                            | (not recommended, as it takes up a lot of memory)";



    qInfo() << "";
    qInfo() << "Example: CDQ -bin myApp -qmlDir ~/Qt/5.11.1/gcc_64/qml -qmake ~/Qt/5.11.1/gcc_64/bin/qmake clear";
}

bool parseQt(Deploy& deploy) {
    auto qmake = QuasarAppUtils::Params::getStrArg("qmake");
    QString basePath = "";
    QFileInfo info(qmake);
    if (!info.isFile() || (info.baseName() != "qmake")) {
        return false;
    }
    basePath = info.absolutePath();
    deploy.setQmake(qmake);
    auto scaner = basePath + QDir::separator() + "qmlimportscanner";

    auto bin = QuasarAppUtils::Params::getStrArg("bin");

    info.setFile(bin);
    if (!info.isFile()) {
        return false;
    }

    if (!deploy.setTarget(bin)) {
        qCritical() << "error init targeet dir";
        return false;
    }

    if (QuasarAppUtils::Params::isEndable("clear")) {
        qInfo() << "clear old data";
        deploy.clear();
    }

    if (!deploy.initDirs()) {
        qCritical() << "error init targeet dir";
        return false;
    }

    auto qmlDir = QuasarAppUtils::Params::getStrArg("qmlDir");

    QDir dir(basePath);

    if (QFileInfo::exists(qmlDir) && QFileInfo::exists(scaner)) {

        deploy.setDeployQml(true);
        deploy.setQmlScaner(scaner);

    } else if (QuasarAppUtils::Params::isEndable("allQmlDependes")) {
        deploy.setDeployQml(true);
    } else {
        qCritical () << "wrong qml dir!";
    }

    if (!dir.cdUp()) {
        return false;
    }

    deploy.setQtDir(dir.absolutePath());

    return true;
}
int main(int argc, char *argv[])
{

    if (!QuasarAppUtils::Params::parseParams(argc, argv)) {
        qWarning() << "wrong parametrs";
        help();
        exit(0);

    };

    if (QuasarAppUtils::Params::isEndable("h") ||
            QuasarAppUtils::Params::isEndable("help")) {
        help();
        exit(0);
    }

    Deploy deploy;

    if (!parseQt(deploy)) {
        qCritical() << "qt parse error";
        help();
        exit(1);
    }

    deploy.deploy();

    qInfo() << "deploy done!";

    return 0;
}
