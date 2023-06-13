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
#if QT_VERSION <= QT_VERSION_CHECK(6, 0, 0)
#include <QTextCodec>
#endif

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
    if (!_fileManager->initDir(target)) {
        QuasarAppUtils::Params::log(QString("Failed to create path : %0 ").arg(target),
                                    QuasarAppUtils::Error);
        return false;

    }

    file.setFileName(target + "/" +  resource.fileName());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QuasarAppUtils::Params::log(QString("Failed to open file for writing: %0. %1").arg(file.fileName(),
                                                                                       file.errorString()),
                                    QuasarAppUtils::Error);
        return false;
    }

    if (sufixes.contains(resource.suffix(), Qt::CaseInsensitive)) {
        QString inputText(inputData);

        inputText.replace("$NAME", info.Name);
        inputText.replace("$DESCRIPTION", info.Description);
        inputText.replace("$VERSION", info.Version);
        inputText.replace("$RELEASEDATA", info.ReleaseData);
        inputText.replace("$PUBLISHER", info.Publisher);
        inputText.replace("$HOMEPAGE", info.Homepage);
        inputText.replace("$PREFIX", info.Prefix);
        inputText.replace("$CQT_INSTALL_DIR", info.InstallDeirQIFW());
        inputText.replace("$CQT_INSTALL_DEB_DIR", info.InstallDirDEB);


        for (auto it = info.Custom.cbegin(); it != info.Custom.cend(); ++it) {
            inputText.replace(it.key(), it.value());
        }

        QTextStream stream(&file);
#if QT_VERSION <= QT_VERSION_CHECK(6, 0, 0)
        stream.setCodec(QTextCodec::codecForName("UTF-8"));
#else
        stream.setEncoding(QStringConverter::Utf8);
#endif
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

    return deployIcon(pkg);

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

    info.InstallDirDEB = "/opt";
    if (!pkg.installDirDEB().isEmpty())
        info.InstallDirDEB = pkg.installDirDEB();

    info.debOut = info.Name + ".deb";
    if (!pkg.debOut().isEmpty())
        info.debOut = pkg.debOut();

    info.zipOut = info.Name + ".zip";
    if (!pkg.zipOut().isEmpty())
        info.zipOut = pkg.zipOut();

    info.Prefix = releativeLocation(pkg);

    QString cmdArray = "[";
    QString bashArray = "(";
    QString cmdShortCutsArray = "[";
    QString bashShortCutsArray = "(";

    int initSize = cmdArray.size();
    for (const auto &target :pkg.targets()) {
        const DeployConfig *cfg = DeployCore::_config;
        auto fileinfo = QFileInfo(target);
        auto targetInfo =  cfg->targets().value(target);

        if (targetInfo.getShortCut()) {

            if (cmdArray.size() > initSize) {
                cmdArray += ",";
                bashArray += " ";
                cmdShortCutsArray += ",";
                bashShortCutsArray += " ";
            }

            cmdArray += "\"" + fileinfo.baseName() + "\"";
            bashArray += "\"" + fileinfo.baseName() + "\"";


            cmdShortCutsArray += "\"" + targetInfo.getRunScriptFile() + "\"";
            bashShortCutsArray += "\"" + targetInfo.getRunScriptFile() + "\"";
        }
    }

    cmdArray += "]";
    bashArray += ")";
    cmdShortCutsArray += "]";
    bashShortCutsArray += ")";

    info.Custom = {{"[\"array\", \"of\", \"cmds\"]", cmdArray}};
    info.Custom["[\"array\", \"of\", \"shortcut\", \"cmds\"]"] = cmdShortCutsArray;

    info.Custom["$BASH_ARRAY_APPLICATIONS"] = bashArray;
    info.Custom["$BASH_ARRAY_SHORTCUTS_APPLICATIONS"] = bashShortCutsArray;

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

bool iDistribution::deployIcon(const DistroModule& pkg) {
    auto localData = dataLocation(pkg);
    const DeployConfig *cfg = DeployCore::_config;

    QuasarAppUtils::Params::log(QString("Deploy icons for package %0. count targets: %1").
                                arg(pkg.name()).arg(pkg.targets().count()),
                                QuasarAppUtils::Debug);

    for (const auto& target: pkg.targets()) {
        auto targetObject = cfg->targets().value(target);

        if (!targetObject.isValid()) {
            QuasarAppUtils::Params::log(QString("The target '%0' is not detected in the target list."
                                                " Available Target List : %1"
                                                " So icon will be copy by Default.").
                                        arg(target, cfg->targets().keys().join(',')),
                                        QuasarAppUtils::Warning);
        }

        auto icon = targetObject.getIcon();

        QuasarAppUtils::Params::log(QString("%0: %1").arg(target, icon),
                                    QuasarAppUtils::Debug);

        if (!targetObject.getShortCut()) {
            QuasarAppUtils::Params::log(QString("%0: %1 Ignored").arg(target, icon),
                                        QuasarAppUtils::Debug);
            continue;
        }

        QFileInfo iconInfo(icon);
        QFileInfo runScript(targetObject.getRunScriptFile());

        QString dist = localData + "/icons/" + runScript.baseName() + "." + iconInfo.suffix();

        if (!copyFile(icon, dist, true)) {

            QuasarAppUtils::Params::log(QString("Failed to copy icon: %0.").arg(icon),
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

