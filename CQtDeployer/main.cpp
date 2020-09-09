/*
 * Copyright (C) 2018-2019 QuasarApp.
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

#include <signal.h>
#include <unistd.h>
#include <execinfo.h>

void handler(int sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}

int main(int argc, const char *argv[]) {

    signal(SIGSEGV, handler);   // install our handler


    QCoreApplication::setOrganizationName("QuasarApp");
    QCoreApplication::setOrganizationDomain("https://github.com/QuasarApp");
    QCoreApplication::setApplicationName("CQtDeployer");


    if (!QuasarAppUtils::Params::parseParams(argc, argv)) {
        qWarning() << "wrong parametrs";
        DeployCore::help();
        exit(0);
    }
    QuasarAppUtils::Params::setEnable("noWriteInFileLog", true);


    Deploy deploy;
    return deploy.run();

}
