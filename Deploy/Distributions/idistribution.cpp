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
#include <quasarapp.h>

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
    if (!QDir().mkpath(target)) {
        QuasarAppUtils::Params::log(QString("impossible to create path : %0 ").arg(target),
                                    QuasarAppUtils::Error);
        return false;

    }

    file.setFileName(target + "/" +  resource.fileName());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QuasarAppUtils::Params::log(QString("impossible to write in file: %0 ").arg(file.fileName()),
                                    QuasarAppUtils::Error);
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
        inputText.replace("$HOMEPAGE", info.Homepage);
        inputText.replace("$PREFIX", info.Prefix);


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

bool iDistribution::collectInfoWithDeployIcons(const DistroModule &pkg,
                                               TemplateInfo &info) {

    if (!collectInfo(pkg, info)) {
        return false;
    }

    return deployIcon(info, pkg);

}

bool iDistribution::collectInfo(const DistroModule& pkg,
                                TemplateInfo &info) {

    info.Name = getName(pkg);

    info.Description = "This package contains the " + info.Name;
    if (!pkg.description().isEmpty())
        info.Description = pkg.description();

    if (!pkg.homePage().isEmpty())
        info.Homepage = pkg.homePage();

    info.Version = "1.0";
    if (!pkg.version().isEmpty())
        info.Version = pkg.version();

    info.ReleaseData = QDate::currentDate().toString("yyyy-MM-dd");
    if (!pkg.releaseData().isEmpty())
        info.ReleaseData = pkg.releaseData();

    info.Publisher = "Company";
    if (!pkg.publisher().isEmpty())
        info.Publisher = pkg.publisher();

    info.Homepage = "";
    if (!pkg.homePage().isEmpty())
        info.Homepage = pkg.homePage();

    info.Prefix = releativeLocation(pkg);

    QString cmdArray = "[";
    QString bashArray = "";

    int initSize = cmdArray.size();
    for (const auto &target :pkg.targets()) {
        auto fileinfo =  QFileInfo(target);
        if (fileinfo.suffix().compare("exe", ONLY_WIN_CASE_INSENSIATIVE) == 0 || fileinfo.suffix().isEmpty()) {
            if (cmdArray.size() > initSize) {
                cmdArray += ",";
                bashArray += " ";
            }
            cmdArray += "\"" + releativeLocation(pkg) + "/" + fileinfo.baseName() + "\"";
            bashArray += fileinfo.baseName();
        }
    }
    cmdArray += "]";

    info.Custom = {{"[\"array\", \"of\", \"cmds\"]", cmdArray}};

    info.Custom["$BASH_ARRAY_APPLICATIONS"] = bashArray;

    if (info.Name.isEmpty()) {
        info.Name = "Application";
    }

    return true;
}

QString iDistribution::getName(const DistroModule& pkg) const {

    QString name = PathUtils::stripPath(pkg.key());

    auto targets = pkg.targets();
    if (pkg.isDefaultModule() && targets.size()) {
        QFileInfo targetInfo(*pkg.targets().begin());
        name = targetInfo.baseName();
    }

    if (!pkg.name().isEmpty()) {
        name = pkg.name();
    }

    return name;
}

bool iDistribution::deployIcon(TemplateInfo &info, const DistroModule& pkg) {
    auto localData = dataLocation(pkg);
    const DeployConfig *cfg = DeployCore::_config;

    info.Icon = "icons/Icon.png";

    for (const auto& target: pkg.targets()) {
        auto icon = cfg->targets().value(target).getIcon();

        QFileInfo iconInfo(icon);
        info.Icon = releativeLocation(pkg) + "/icons/" + iconInfo.fileName();
        if (!copyFile(icon, localData + "/icons/", false)) {

            QuasarAppUtils::Params::log(QString("fail to copy icon: %0 ").arg(icon),
                                        QuasarAppUtils::Error);

            return false;
        }
    }

    return true;
}

QString iDistribution::releativeLocation(const DistroModule &module) const {
    if (module.isDefaultModule()) {
        return module.prefix();
    }

    if (!module.prefix().isEmpty())
        return module.prefix();


    if (!module.name().isEmpty())
        return module.name();

    return module.key();

}



