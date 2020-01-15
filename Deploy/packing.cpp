#include "Distributions/idistribution.h"
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

    _proc->setProgram(_pakage->runCmd());
    auto env = QProcessEnvironment::systemEnvironment();
    env.insert("PATH", _pakage->toolKitLocation().concatEnv());
    _proc->setProcessEnvironment(env);
    _proc->setArguments(_pakage->runArg());

    _proc->start();

    if (!_proc->waitForStarted(1000)) {
        return false;
    }

    if (!_proc->waitForFinished(60000 * 10)) {
        return false;
    }

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

