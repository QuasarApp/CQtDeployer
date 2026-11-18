/*
 * Copyright (C) 2018-2026 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "deploycore.h"
#include <QCoreApplication>
#include <deploy.h>
#include <qalogger.h>

int main(int argc, char *argv[]) {

    QCoreApplication::setOrganizationName("QuasarApp");
    QCoreApplication::setOrganizationDomain("https://github.com/QuasarApp");
    QCoreApplication::setApplicationName("CQtDeployer");

    // The command line arguments without the name of the executable file.
    // The arguments are captured before the QCoreApplication construction,
    // the constructor reorders the argv array of the process.
    QStringList params;
    for (int i = 1; i < argc; ++i) {
        params.append(QString::fromUtf8(argv[i]));
    }

    QCoreApplication app(argc, argv);

    // The command line of the tool is parsed with the Qt QCommandLineParser
    // (the official Qt command line parser, the review of the PR #928:
    // instead of the parseParams method of the QuasarAppUtils::Params class
    // of the QuasarAppLib library): the shell-expanded '*' globs of the
    // command line are reconstructed (the DeployCore::fixShellExpandedGlobs
    // function, issue #311), the bare option names of the command line are
    // normalized to the dash form of the parser and the parsed options are
    // stored in the QuasarAppUtils::Params storage that the rest of the
    // tool reads.
    if (!DeployCore::parseCommandLine(params)) {
        qWarning() << "Wrong parameters. Please use the 'help' of 'h' option to show the help page.";
        exit(4);
    }

    // Route all qt log messages (qDebug, qInfo, qWarning, qCritical) through
    // the QALogger handler, which filters them according to the verbose
    // level (issue #875).
    QuasarAppUtils::QALogger logger;
    logger.init();

    Deploy deploy;

    deploy.init();

    int code = deploy.run();

    if (code) {
        qCritical() << Deploy::codeString(code);
    }

    return code;
}
