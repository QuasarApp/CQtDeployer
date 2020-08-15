#include "Distributions/idistribution.h"
#include "deployconfig.h"
#include "filemanager.h"
#include "packing.h"
#include "pathutils.h"
#include "quasarapp.h"
#include <QDebug>
#include <QProcess>
#include <QThread>

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
    _pakage = pakages;
}

bool Packing::create() {

    if (!collectPackages()) {
        return false;
    }

    for (auto package : _pakage) {

        if (!package)
            return false;

        if (!package->deployTemplate(*this))
            return false;

        if (package->runCmd().size()) {
            const DeployConfig *cfg = DeployCore::_config;

            QFileInfo cmdInfo(package->runCmd());

            auto allExecRight =  QFile::ExeUser | QFile::ExeGroup | QFile::ExeOwner;
            if (!cmdInfo.permission(allExecRight)) {
                QFile::setPermissions(cmdInfo.absoluteFilePath(), cmdInfo.permissions() | allExecRight);
            }

            _proc->setProgram(package->runCmd());
            _proc->setProcessEnvironment(_proc->processEnvironment());
            _proc->setArguments(package->runArg());
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

            if (_proc->exitCode() != 0) {
                QuasarAppUtils::Params::log(message, QuasarAppUtils::Error);

                if (QuasarAppUtils::Params::isDebug())
                    return false;
            }
        }

        if (!restorePackagesLocations()) {
            return false;
        }

        package->removeTemplate();
    }

    return true;
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

QMultiMap<int ,QPair<QString, const DistroModule*>>
sortPackages(const QHash<QString, DistroModule> &input) {
    QMultiMap<int, QPair<QString, const DistroModule *>> result;
    for (auto it = input.cbegin(); it != input.cend(); ++it ) {
        result.insert(0xFFFF - it.key().size(), {it.key(), &it.value()});
    }

    return result;
}

bool Packing::collectPackages() {
    const DeployConfig *cfg = DeployCore::_config;

    auto sortedMap = sortPackages(cfg->packages());

    for (auto &it : sortedMap) {
        auto package = it.second;

        QString Name = PathUtils::stripPath(it.first);

        if (Name.isEmpty()) {
            QFileInfo targetInfo(*package->targets().begin());
            Name = targetInfo.baseName();
        }

        if (!package->name().isEmpty()) {
            Name = package->name();
        }

        QString tmpPakcageLocation = "tmp_data";
        auto location = cfg->getTargetDir() + "/"  + tmpPakcageLocation + "/" +
                ((it.first.isEmpty())? "Application": Name);


        _packagesLocations.insert(it.first, location);

        if (!moveData(cfg->getTargetDir() + "/" + it.first, location, tmpPakcageLocation)) {
            return false;
        }
    }

    _defaultPackagesLocations = _packagesLocations;
    return true;
}

bool Packing::moveData(const QString &from, const QString &to, const QString &ignore) const {
    return _fileManager->moveFolder(from, to, ignore);
}

bool Packing::restorePackagesLocations() {
    for (auto it = _packagesLocations.begin(); it != _packagesLocations.end(); ++it) {
        if (!moveData(it.value(), _defaultPackagesLocations.value(it.key()))) {
            return false;
        }
    }

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


