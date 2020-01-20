#include "Distributions/idistribution.h"
#include "deployconfig.h"
#include "packing.h"
#include "quasarapp.h"
#include <QDebug>
#include <QProcess>
#include <QThread>

Packing::Packing() {
    _proc = new QProcess();

    connect(_proc, &QProcess::readyReadStandardError,
            this, &Packing::handleOutputUpdate, Qt::QueuedConnection);

    connect(_proc, &QProcess::readyReadStandardOutput,
            this, &Packing::handleOutputUpdate, Qt::QueuedConnection);

    moveToThread( new QThread(this));
}

Packing::~Packing() {
    if (_pakage)
        delete _pakage;

    _proc->kill();

    delete _proc;
}

void Packing::setDistribution(iDistribution *pakage) {
    _pakage = pakage;
}

bool Packing::create() {

    if (!_pakage)
        return false;

    if (!_pakage->deployTemplate())
        return false;

    if (!_pakage->runCmd().size())
        return true;

    const DeployConfig *cfg = DeployCore::_config;

    _proc->setProgram(_pakage->runCmd());
    _proc->setProcessEnvironment(_proc->processEnvironment());
    _proc->setArguments(_pakage->runArg());
    _proc->setWorkingDirectory(cfg->getTargetDir());

    _proc->start();

    if (!_proc->waitForStarted(1000)) {
        return false;
    }

    if (!_proc->waitForFinished(-1)) {
        return false;
    }

    auto exit = QString("exit code = %0").arg(_proc->exitCode());
    QString stdoutLog = _proc->readAllStandardOutput();
    QString erroutLog = _proc->readAllStandardError();
    auto message = QString("message = %0").arg(stdoutLog + " " + erroutLog);

    QuasarAppUtils::Params::verboseLog(message,
                                     QuasarAppUtils::Info);

    if (!_pakage->removeTemplate()) {
        return false;
    }

    return true;
}

void Packing::handleOutputUpdate() {
    QByteArray stdoutLog = _proc->readAllStandardOutput();
    QByteArray erroutLog = _proc->readAllStandardError();

    if (stdoutLog.size())
        qInfo() << stdoutLog;

    if (erroutLog.size())
        qInfo() << erroutLog;
}

