/*
 * Copyright (C) 2018 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "deploy.h"
#include "quasarapp.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QList>

void help() {
    qInfo() << "";
    qInfo() << "Usage: cqtdeployer <-bin    [params]> [options]";
    qInfo() << "";
    qInfo() << "Options:";
    qInfo() << "   help / h                 : show help.";
    qInfo() << "   always-overwrite         : Copy files even if the target file exists.";
    qInfo() << "   -bin    [params]         : deployment binry.";
    qInfo() << "   -qmlDir [params]         : qml datadir of project. for example -qmlDir ~/my/project/qml";
    qInfo() << "   deploy-not-qt            : deploy all libs";
    qInfo() << "   -qmake  [params]         : qmake path. for example";
    qInfo() << "                            | -qmake ~/Qt/5.11.1/gcc_64/bin/qmake";
    qInfo() << "   -ignore [list,params]    : ignore filter for libs";
    qInfo() << "                            | for example -ignore libicudata.so.56,libicudata2.so.56";
    qInfo() << "   clear                    : delete all old deploy data";
    qInfo() << "   -runScript [params]      : set new name of out file (AppRun.sh by default)";
    qInfo() << "                            | for example -runScript myApp.sh";
    qInfo() << "   allQmlDependes           : This flag will force to extract all qml libraries.";
    qInfo() << "                            | (not recommended, as it takes up a lot of memory)";
    qInfo() << "   -libDir [list,params]    : set additional path for extralib of app.";
    qInfo() << "                            | for example -libDir ~/myLib,~/newLibs";
    qInfo() << "   -extraPlugin[list,params]: set additional path for extraPlugin of app";
    qInfo() << "   -recursiveDepth [params] : set Depth for recursive search of libs (default 0)";

    qInfo() << "   verbose                  : show debug log";


    qInfo() << "";
    qInfo() << "Example: cqtdeployer -bin myApp -qmlDir ~/Qt/5.11.1/gcc_64/qml -qmake ~/Qt/5.11.1/gcc_64/bin/qmake clear";
    qInfo() << "Example (only C libs): cqtdeployer -bin myApp clear";
}

void verboseLog(const QString &str) {
    if (QuasarAppUtils::Params::isEndable("verbose")) {
        qDebug() << str;
    }
}

bool parseQt(Deploy &deploy) {

    auto bin = QuasarAppUtils::Params::getStrArg("bin");

    QFileInfo info(bin);
    if (!info.isFile()) {
        verboseLog(QDir::homePath());
        verboseLog(QDir("./").absolutePath());

        verboseLog("bin file is not file path: " + bin);
        return false;
    }

    if (!deploy.setTarget(bin)) {
        qCritical() << "error init targeet dir";
        return false;
    }

    deploy.initEnvirement();

    int limit = 0;

    if (QuasarAppUtils::Params::isEndable("recursiveDepth")) {
        bool ok;
        limit = QuasarAppUtils::Params::getArg("recursiveDepth").toInt(&ok);
        if (!ok) {
            limit = 0;
            qWarning() << "recursiveDepth is invalid! use default value 0";
        }
    }

    deploy.setDepchLimit(limit);

    if (QuasarAppUtils::Params::isEndable("clear")) {
        qInfo() << "clear old data";
        deploy.clear();
    }

    auto listLibDir = QuasarAppUtils::Params::getStrArg("libDir").split(",");
    auto listExtraPlugin =
        QuasarAppUtils::Params::getStrArg("extraPlugin").split(",");
    deploy.setExtraPath(listLibDir);
    deploy.setExtraPlugins(listExtraPlugin);

    if (!deploy.initDirs()) {
        qCritical() << "error init targeet dir";
        return false;
    }

    auto qmake = QuasarAppUtils::Params::getStrArg("qmake");
    QString basePath = "";
    info.setFile(qmake);
    if (!info.isFile() || (info.baseName() != "qmake")) {
        qInfo() << "deploy only C libs because qmake is not found";
        deploy.setOnlyCLibs(true);
        return true;
    }

    basePath = info.absolutePath();
    deploy.setQmake(qmake);
    auto scaner = basePath + QDir::separator() + "qmlimportscanner";

    auto qmlDir = QuasarAppUtils::Params::getStrArg("qmlDir");

    QDir dir(basePath);

    if (QFileInfo::exists(qmlDir) && QFileInfo::exists(scaner)) {

        deploy.setDeployQml(true);
        deploy.setQmlScaner(scaner);

    } else if (QuasarAppUtils::Params::isEndable("allQmlDependes")) {
        deploy.setDeployQml(true);
    } else {
        qCritical() << "wrong qml dir!";
    }

    if (!dir.cdUp()) {
        return false;
    }

    deploy.setQtDir(dir.absolutePath());

    return true;
}
int main(int argc, char *argv[]) {

    QCoreApplication::setOrganizationName("QuasarApp");
    QCoreApplication::setOrganizationDomain("https://github.com/QuasarApp");
    QCoreApplication::setApplicationName("CQtDeployer");

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
