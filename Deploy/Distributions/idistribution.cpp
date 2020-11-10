#include "idistribution.h"
#include <typeinfo>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <cassert>
#include <filemanager.h>
#include "deploycore.h"
#include "pathutils.h"
#include <QDate>
#include <QMap>
#include <deployconfig.h>
#include <distromodule.h>

iDistribution::~iDistribution() = default;

iDistribution::iDistribution(FileManager *fileManager) {
    _fileManager = fileManager;
    assert(_fileManager);
}

bool iDistribution::cb() const  {
    return true;
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
                              const QStringList &sufixes,
                              const QHash<QString, QString> &folderNewNames) const {


    QDir res(resource);
    auto list = res.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);

    for (const auto & item :list) {

        if (item.isFile()) {
            if (!unpackFile(item, target, info, sufixes)) {
                return false;
            }
        } else {
            QString targetName = item.fileName();
            if (folderNewNames.contains(targetName)) {
                targetName = folderNewNames.value(targetName, "");
            }

            if (!unpackDir(item.absoluteFilePath(),
                           target + "/" + targetName,
                           info, sufixes, folderNewNames)) {

                return false;
            }
        }
    }

    return true;
}

bool iDistribution::copyDir(const QString &resource,
                            const QString &target) const {

    return _fileManager->copyFolder(resource, target, {}, nullptr, nullptr, true);
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

void iDistribution::registerOutFiles() const {
    auto files = outPutFiles();

    for (const auto& i : files) {
        _fileManager->addToDeployed(i);
    }
}

bool iDistribution::collectInfo(
        const QHash<QString, DistroModule>::const_iterator& it,
        const DeployConfig * cfg,
        TemplateInfo &info,
        bool &fDefaultPakcage) {

    auto package = it.value();

    info.Name = PathUtils::stripPath(it.key());
    fDefaultPakcage = cfg->getDefaultPackage() == info.Name;

    if (fDefaultPakcage) {
        QFileInfo targetInfo(*package.targets().begin());
        info.Name = targetInfo.baseName();
    }

    if (!package.name().isEmpty()) {
        info.Name = package.name();
    }

    auto localData = dataLocation(info.Name);

    info.Description = "This package contains the " + info.Name;
    if (!package.description().isEmpty())
        info.Description = package.description();

    info.Version = "1.0";
    if (!package.version().isEmpty())
        info.Version = package.version();

    info.ReleaseData = QDate::currentDate().toString("yyyy-MM-dd");
    if (!package.releaseData().isEmpty())
        info.ReleaseData = package.releaseData();

    info.Icon = "icons/Icon.png";
    if (package.icon().isEmpty()) {
        if (!copyFile(":/Templates/QIF/Distributions/Templates/qif/Icon.png",
                      localData + "/icons/", false)) {
            return false;
        }
    } else {
        QFileInfo iconInfo(package.icon());
        info.Icon = info.Name + "/icons/" + iconInfo.fileName();
        if (!copyFile(package.icon(), localData + "/icons/", false)) {
            return false;
        }
    }

    info.Publisher = "Company";
    if (!package.publisher().isEmpty())
        info.Publisher = package.publisher();

    QString cmdArray = "[";
    int initSize = cmdArray.size();
    for (const auto &target :package.targets()) {
        auto fileinfo =  QFileInfo(target);
        if (fileinfo.suffix().compare("exe", ONLY_WIN_CASE_INSENSIATIVE) == 0 || fileinfo.suffix().isEmpty()) {
            if (cmdArray.size() > initSize) {
                cmdArray += ",";
            }
            cmdArray += "\"" + info.Name + "/" + fileinfo.fileName() + "\"";
        }
    }
    cmdArray += "]";

    info.Custom = {{"[\"array\", \"of\", \"cmds\"]", cmdArray},
                   {"$LOCAL_ICON", info.Name + "/icons/" + QFileInfo(info.Icon).fileName()}};


    if (info.Name.isEmpty()) {
        info.Name = "Application";
    }

    return true;
}


