#include "idistribution.h"
#include <typeinfo>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <cassert>
#include <filemanager.h>
#include "deploycore.h"
#include "pathutils.h"
#include <QMap>
#include <distromodule.h>

iDistribution::~iDistribution() = default;

iDistribution::iDistribution(FileManager *fileManager) {
    _fileManager = fileManager;
    assert(_fileManager);
}

QString iDistribution::getClassName() const {
    return typeid(*this).name();
}

QString iDistribution::getLocation() const {
    return _location;
}

void iDistribution::setLocation(const QString &location) {
    _location = location;
}

bool iDistribution::unpackFile(const QFileInfo &resource,
                               const QString &target,
                               const TemplateInfo &info,
                               const QStringList& sufixes) const {
    QFile file(resource.absoluteFilePath());
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray inputData = file.readAll();
    file.close();
    if (!QDir().mkpath(target))
        return false;

    file.setFileName(target + "/" +  resource.fileName());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }

    if (sufixes.contains(resource.suffix(), Qt::CaseInsensitive)) {
        QString inputText(inputData);

        inputText.replace("$NAME", info.Name);
        inputText.replace("$DESCRIPTION", info.Description);
        inputText.replace("$VERSION", info.Version);
        inputText.replace("$RELEASEDATA", info.ReleaseData);
        inputText.replace("$ICON", info.Icon);
        inputText.replace("$PUBLISHER", info.Publisher);

        for (auto it = info.Custom.cbegin(); it != info.Custom.cend(); ++it) {
            inputText.replace(it.key(), it.value());
        }

        QTextStream stream(&file);
        stream << inputText;
    } else {
        file.write(inputData);
    }

    file.close();

    return _fileManager->addToDeployed(target + "/" +  resource.fileName());
}

bool iDistribution::unpackDir(const QString &resource,
                              const QString &target,
                              const TemplateInfo &info,
                              const QStringList &sufixes) const {


    QDir res(resource);
    auto list = res.entryInfoList();

    for (const auto & item :list) {

        if (item.isFile()) {
            if (!unpackFile(item, target, info, sufixes)) {
                return false;
            }
        } else {
            if (!unpackDir(item.absoluteFilePath(),
                           target + "/" + item.fileName(),
                           info, sufixes)) {

                return false;
            }
        }
    }

    return true;
}

bool iDistribution::moveData(const QString &from,
                             const QString &to,
                             const QString& ignore) const {
    return _fileManager->moveFolder(from, to, ignore);
}

bool iDistribution::copyFile(const QString &from, const QString &to, bool isFileTarget) const {
    if (isFileTarget) {
        _fileManager->removeFile(to);
    }
    return _fileManager->copyFile(from, to, nullptr, isFileTarget);
}

QMap<int ,QPair<QString, const DistroModule*>>
iDistribution::sortPackages(const QHash<QString, DistroModule> &input) {
    QMap<int, QPair<QString, const DistroModule *>> result;
    for (auto it = input.cbegin(); it != input.cend(); ++it ) {
        result.insertMulti(0xFFFF - it.key().size(), {it.key(), &it.value()});
    }

    return result;
}
