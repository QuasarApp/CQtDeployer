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
#include <qalogger.h>

int main(int argc, char *argv[]) {

    QCoreApplication::setOrganizationName("QuasarApp");
    QCoreApplication::setOrganizationDomain("https://github.com/QuasarApp");
    QCoreApplication::setApplicationName("CQtDeployer");

    // The shell expands the unquoted '*' glob of the command line (for
    // example the 'cqtdeployer -bin *.deb qtifw' command) into the separate
    // file arguments before the tool starts (issue #311, the manual
    // testing of the PR #928): merge the expanded file list back into the
    // single value of the option, so the tool works with the unquoted glob
    // the same way as with the quoted one.
    QStringList params;
    for (int i = 1; i < argc; ++i) {
        params.append(QString::fromUtf8(argv[i]));
    }

    if (!QuasarAppUtils::Params::parseParams(DeployCore::fixShellExpandedGlobs(params),
                                             DeployCore::avilableOptions())) {
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
