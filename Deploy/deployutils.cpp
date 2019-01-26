/*
 * Copyright (C) 2018-2019 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "deploy.h"
#include "deployutils.h"
#include "quasarapp.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>

QString DeployUtils::qtDir = "";
QStringList DeployUtils::extraPaths = QStringList();

int DeployUtils::getLibPriority(const QString &lib) {

    if (!QFileInfo(lib).isFile()) {
        return 0;
    }

    if (isQtLib(lib)) {
        return 3;
    }

    if (isExtraLib(lib)) {
        return 2;
    }

    return 1;
}

void DeployUtils::verboseLog(const QString &str) {
    if (QuasarAppUtils::Params::isEndable("verbose")) {
        qDebug() << str;
    }
}

void DeployUtils::help() {
    qInfo() << "";
    qInfo() << "Usage: cqtdeployer <-bin    [params]> [options]";
    qInfo() << "";
    qInfo() << "Options:";
    qInfo() << "   help / h                 : show help.";
    qInfo() << "   always-overwrite         : Copy files even if the target file exists.";
    qInfo() << "   -bin    [list, params]   : deployment binry or directory.";
    qInfo() << "                            | example -bin ~/my/project/bin/,~/my/project/bin.exe";
    qInfo() << "   -binDir [params]         : folder with deployment binaries with recursive search";
    qInfo() << "                            | WARNING this flag support only 'so', 'dll' and 'exe' files";
    qInfo() << "                            | if you want deploy linux binary then use '-bin' flag";
    qInfo() << "   -qmlDir [params]         : qml datadir of project. for example -qmlDir ~/my/project/qml";
    qInfo() << "   deploy-not-qt            : deploy all libs";
    qInfo() << "   -qmake  [params]         : qmake path. for example";
    qInfo() << "                            | -qmake ~/Qt/5.11.1/gcc_64/bin/qmake";
    qInfo() << "   -ignore [list,params]    : ignore filter for libs";
    qInfo() << "                            | for example -ignore libicudata.so.56,libicudata2.so.56";
    qInfo() << "   clear                    : delete all old deploy data";
    qInfo() << "                            | for example -runScript myApp.sh";
    qInfo() << "   allQmlDependes           : This flag will force to extract all qml libraries.";
    qInfo() << "                            | (not recommended, as it takes up a lot of memory)";
    qInfo() << "   -libDir [list,params]    : set additional path for extralib of app.";
    qInfo() << "                            | for example -libDir ~/myLib,~/newLibs ";
    qInfo() << "   -extraPlugin[list,params]: set additional path for extraPlugin of app";
    qInfo() << "   -recursiveDepth [params] : set Depth for recursive search of libs (default 0)";
    qInfo() << "   -targetDir [params]      : set target Dir for binaryes (default is path of first target)";
    qInfo() << "   noStrip                  : skip strip step";

    qInfo() << "   verbose                  : show debug log";

    qInfo() << "";
    qInfo() << "Example: cqtdeployer -bin myApp -qmlDir ~/Qt/5.11.1/gcc_64/qml -qmake ~/Qt/5.11.1/gcc_64/bin/qmake clear";
    qInfo() << "Example (only C libs): cqtdeployer -bin myApp clear";
}

bool DeployUtils::parseQt(Deploy *deploy) {

    if (!(QuasarAppUtils::Params::isEndable("bin") ||
            QuasarAppUtils::Params::isEndable("binDir"))) {
        qWarning() << "you need to use -bin or -binDir flags";
        return false;
    }

    auto bin = QuasarAppUtils::Params::getStrArg("bin").split(',');
    bin.removeAll("");

    if (!deploy->setTargets(bin)) {

        auto binDir = QuasarAppUtils::Params::getStrArg("binDir");

        if (!deploy->setTargetsRecursive(binDir)) {
            qCritical() << "error init targeet dir";
            return false;
        }
    }

    if (QuasarAppUtils::Params::isEndable("clear")) {
        qInfo() << "clear old data";
        deploy->clear();
    }

    deploy->initEnvirement();

    int limit = 0;

    if (QuasarAppUtils::Params::isEndable("recursiveDepth")) {
        bool ok;
        limit = QuasarAppUtils::Params::getArg("recursiveDepth").toInt(&ok);
        if (!ok) {
            limit = 0;
            qWarning() << "recursiveDepth is invalid! use default value 0";
        }
    }

    deploy->setDepchLimit(limit);

    auto listLibDir = QuasarAppUtils::Params::getStrArg("libDir").split(",");
    auto listExtraPlugin =
            QuasarAppUtils::Params::getStrArg("extraPlugin").split(",");
    deploy->setExtraPath(listLibDir);
    deploy->setExtraPlugins(listExtraPlugin);

    auto qmake = QuasarAppUtils::Params::getStrArg("qmake");
    QString basePath = "";

    QFileInfo info(qmake);

    if (!info.isFile() || (info.baseName() != "qmake")) {
        qInfo() << "deploy only C libs because qmake is not found";
        deploy->setOnlyCLibs(true);
        return true;
    }

    basePath = info.absolutePath();
    deploy->setQmake(qmake);
#ifdef Q_OS_WIN
    auto scaner = basePath + QDir::separator() + "qmlimportscanner.exe";
#else
    auto scaner = basePath + QDir::separator() + "qmlimportscanner";
#endif
    auto qmlDir = QuasarAppUtils::Params::getStrArg("qmlDir");

    QDir dir(basePath);

    if (QFileInfo::exists(qmlDir) && QFileInfo::exists(scaner)) {

        deploy->setDeployQml(true);
        deploy->setQmlScaner(scaner);

    } else if (QuasarAppUtils::Params::isEndable("allQmlDependes")) {
        deploy->setDeployQml(true);
    } else {
        qCritical() << "wrong qml dir!";
    }

    if (!dir.cdUp()) {
        return false;
    }

    deploy->setQtDir(dir.absolutePath());

    return true;
}

bool DeployUtils::isQtLib(const QString &lib) {
    QFileInfo info(lib);
    return !qtDir.isEmpty() && info.absoluteFilePath().contains(qtDir);

}

bool DeployUtils::isExtraLib(const QString &lib) {
    QFileInfo info(lib);

    for (auto i : extraPaths) {
        if (info.absoluteFilePath().contains(i)) {
            return true;
        }
    }

    return false;
}
