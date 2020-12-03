#include "Distributions/idistribution.h"
#include "deployconfig.h"
#include "filemanager.h"
#include "packing.h"
#include "pathutils.h"
#include "quasarapp.h"
#include <QDebug>
#include <QProcess>
#include <QThread>
#include <cassert>

#define TMP_PACKAGE_DIR "tmp_data"

Packing::Packing(FileManager *fileManager) {
    assert(fileManager);

    _fileManager = fileManager;
    _proc = new QProcess(this);

    connect(_proc, SIGNAL(readyReadStandardError()),
            this, SLOT(handleOutputUpdate()));

    connect(_proc, SIGNAL(readyReadStandardOutput()),
            this, SLOT(handleOutputUpdate()));

}

Packing::~Packing() {
    _proc->kill();
}

void Packing::setDistribution(const QList<iDistribution*> &pakages) {
    _pakages = pakages;
}

bool Packing::create() {

    if (!collectPackages()) {
        return false;
    }

    for (auto package : _pakages) {

        if (!package)
            return false;

        if (!package->deployTemplate(*this))
            return false;

        auto commands = package->runCmd();

        for (const auto& cmd: commands) {
            const DeployConfig *cfg = DeployCore::_config;


            QFileInfo cmdInfo(cmd.command);

            auto allExecRight =  QFile::ExeUser | QFile::ExeGroup | QFile::ExeOwner;
            if (!cmdInfo.permission(allExecRight)) {
                QFile::setPermissions(cmdInfo.absoluteFilePath(), cmdInfo.permissions() | allExecRight);
            }

            _proc->setProgram(cmd.command);
            _proc->setProcessEnvironment(_proc->processEnvironment());
            _proc->setArguments(cmd.arguments);
            _proc->setWorkingDirectory(cfg->getTargetDir());

            _proc->start();

            if (!_proc->waitForStarted()) {
                QuasarAppUtils::Params::log(_proc->errorString(), QuasarAppUtils::Error);
                QuasarAppUtils::Params::log(QString("Process error code: %0").arg(_proc->error()),
                                            QuasarAppUtils::Error);
                return false;
            }

            if (!_proc->waitForFinished(-1)) {
                QuasarAppUtils::Params::log(_proc->errorString(), QuasarAppUtils::Error);
                QuasarAppUtils::Params::log(QString("Process error code: %0").arg(_proc->error()),
                                            QuasarAppUtils::Error);


                return false;
            }

            auto exit = QString("exit code = %0").arg(_proc->exitCode());
            QString stdoutLog = _proc->readAllStandardOutput();
            QString erroutLog = _proc->readAllStandardError();
            auto message = QString("message = %0").arg(stdoutLog + " " + erroutLog);

            if (_proc->exitCode() != 0) {
                QuasarAppUtils::Params::log(message, QuasarAppUtils::Error);

                if (QuasarAppUtils::Params::isDebug())
                    return false;
            }
        }

        if (!package->cb()) {
            return false;
        }

        if (!restorePackagesLocations()) {
            return false;
        }

        package->removeTemplate();
        delete package;
    }

    const DeployConfig *cfg = DeployCore::_config;
    return QDir(cfg->getTargetDir() + "/" + TMP_PACKAGE_DIR).removeRecursively();
}

bool Packing::movePackage(const QString &package,
                          const QString &newLocation) {

    if (moveData(_packagesLocations.value(package),
                 newLocation)) {

        _packagesLocations[package] = newLocation;
        return true;
    }

    return false;
}

bool Packing::copyPackage(const QString &package, const QString &newLocation) {
    return _fileManager->copyFolder(_packagesLocations[package], newLocation, {}, nullptr, nullptr, true);
}

bool Packing::collectPackages() {
    const DeployConfig *cfg = DeployCore::_config;

    for (auto it = cfg->packages().begin(); it != cfg->packages().end(); ++it) {
        if (!moveData(cfg->getTargetDir() + "/" + it.key(), cfg->getTargetDir() + "/" + TMP_PACKAGE_DIR + "/" + it.key()))
            return false;

        _packagesLocations.insert(it.key(), cfg->getTargetDir() + "/" + TMP_PACKAGE_DIR + "/" + it.key());
    }

    _defaultPackagesLocations = _packagesLocations;
    return true;
}

bool Packing::moveData(const QString &from, const QString &to, const QString &ignore) const {

    if (from == to )
        return true;

    if (!_fileManager->moveFolder(from, to, ignore)) {
        return false;
    }
    return QDir(from).removeRecursively();
}

bool Packing::restorePackagesLocations() {
    for (auto it = _packagesLocations.begin(); it != _packagesLocations.end(); ++it) {
        if (!moveData(it.value(), _defaultPackagesLocations.value(it.key()))) {
            return false;
        }
    }
    _packagesLocations = _defaultPackagesLocations;

    return true;
}

void Packing::handleOutputUpdate() {

    QByteArray stdoutLog = _proc->readAllStandardOutput();
    QByteArray erroutLog = _proc->readAllStandardError();

    if (stdoutLog.size())
        QuasarAppUtils::Params::log(stdoutLog,
                                    QuasarAppUtils::Info);

    if (erroutLog.size())
        QuasarAppUtils::Params::log(erroutLog,
                                    QuasarAppUtils::Info);
}


