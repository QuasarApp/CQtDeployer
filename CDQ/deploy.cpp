#include "deploy.h"
#include <QFileInfo>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <quasarapp.h>
#include <QProcess>


bool Deploy::getDeployQml() const {
    return deployQml;
}

void Deploy::setDeployQml(bool value) {
    deployQml = value;
}

QString Deploy::getQmlScaner() const {
    return qmlScaner;
}

void Deploy::setQmlScaner(const QString &value) {
    qmlScaner = value;
    deployQml = QFileInfo(qmlScaner).isFile();
}

QString Deploy::getQmake() const {
    return qmake;
}

void Deploy::setQmake(const QString &value) {
    qmake = value;
}

QString Deploy::getTarget() const {
    return target;
}

bool Deploy::setTarget(const QString &value) {
    target = value;
    targetDir = QFileInfo(target).absolutePath();

    if (!QFileInfo::exists(targetDir + QDir::separator() + "lib") &&
            !QDir(targetDir).mkdir("lib")) {
        return false;
    }

    if (QuasarAppUtils::isEndable("qmlDir") &&
            !QFileInfo::exists(targetDir  + QDir::separator() + "qml") &&
            !QDir(targetDir).mkdir("qml")){
        return false;
    }

    return true;
}

void Deploy::deploy() {
    qInfo() << "target deploy started!!";
    extract(target);
    copyFiles(QtLibs, targetDir + QDir::separator() + "lib");

    if (QuasarAppUtils::isEndable("deploy-not-qt")) {
        copyFiles(noQTLibs, targetDir + QDir::separator() + "lib");
    }

    if (!QuasarAppUtils::isEndable("noStrip")) {
        strip(targetDir + QDir::separator() + "lib");
    }

}

QString Deploy::getQtDir() const {
    return qtDir;
}

void Deploy::setQtDir(const QString &value) {
    qtDir = value;
}

bool Deploy::isQtLib(const QString &lib) const {
    QFileInfo info(lib);
    return info.absolutePath().contains(qtDir);
}

void Deploy::copyFiles(const QStringList &files , const QString& target) {
    for (auto file : files) {
        if (!copyFile(file, target)) {
            qWarning() << file + " not copied";
        }
    }
}

bool Deploy::copyFile(const QString &file, const QString& target) {

    auto info = QFileInfo(file);

    auto name = info.baseName();
    info.setFile(target + QDir::separator() + name);

    if (QuasarAppUtils::isEndable("always-overwrite") &&
            info.exists() && !QFile::remove(target + QDir::separator() + name)){
        return false;
    }

    qInfo() << "copy :" << target + QDir::separator() + name;

    return QFile::copy(file, target + QDir::separator() + name);

}

void Deploy::extract(const QString &file) {

    qInfo() << "extract lib :" << file;

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("LD_LIBRARY_PATH", qtDir + "/lib");
    env.insert("LD_LIBRARY_PATH", targetDir);
    env.insert("QML_IMPORT_PATH", qtDir + "/qml");
    env.insert("QML2_IMPORT_PATH", qtDir + "/qml");
    env.insert("QT_PLUGIN_PATH", qtDir + "/plugins");
    env.insert("QT_QPA_PLATFORM_PLUGIN_PATH", qtDir + "/plugins/platforms");

    QProcess P;
    P.setProcessEnvironment(env);
    P.start("ldd " + file, QProcess::ReadOnly);

    if (!P.waitForStarted()) return;
    if (!P.waitForFinished()) return;

    auto data = QString(P.readAll());
    QStringList libs;

    for (QString &line : data.split("\n", QString::SkipEmptyParts)) {
        line = line.simplified();
        auto innerlist = line.split(" ");

        if (innerlist.count() < 3) continue;
        line = innerlist[2];

        if (!line.startsWith("/")) {
            continue;
        }

        if (isQtLib(line) && !QtLibs.contains(line)) {
            QtLibs << line;
            extract(line);
            continue;
        }

        if (QuasarAppUtils::isEndable("deploy-not-qt") &&
                !noQTLibs.contains(line)) {
            noQTLibs << line;
            extract(line);
        }

    }
}

void Deploy::strip(const QString& dir) {
    QProcess P;

    P.setWorkingDirectory(dir);
    P.setArguments(QStringList() << "*");
    P.start("strip", QProcess::ReadOnly);


    if (!P.waitForStarted()) return;
    if (!P.waitForFinished()) return;

    auto a = P.errorString();
    qInfo() << a;

}

Deploy::Deploy(){

}
