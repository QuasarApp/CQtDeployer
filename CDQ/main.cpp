#include <QCoreApplication>
#include "quasarapp.h"
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include "deploy.h"
#include <QList>

void help() {
    qInfo() << "";
    qInfo() << "Usage: CDQ <app-binary> [options]";
    qInfo() << "";
    qInfo() << "Options:";
    qInfo() << "   -help / -h        : show help.";
    qInfo() << "   -always-overwrite        : Copy files even if the target file exists.";
    qInfo() << "   -bin                     : deployment binry.";
    qInfo() << "   -qmlDir                  : qml datadir. for example ~/Qt/5.11.1/gcc_64/qml";
    qInfo() << "   -noStrip                 : no strip deployed lib";
    qInfo() << "   -deploy-not-qt           : deploy all libs";
    qInfo() << "   -qmake                   : qmake path. for example";
    qInfo() << "                            | for example ~/Qt/5.11.1/gcc_64/bin/qmake";

}

bool parseQt(Deploy& deploy) {
    auto qmake = QuasarAppUtils::getStrArg("qmake");
    QString basePath = "";
    QFileInfo info(qmake);
    if (!info.isFile() || (info.baseName() != "qmake")) {
        return false;
    }
    basePath = info.path();
    deploy.setQmake(qmake);

    auto bin = QuasarAppUtils::getStrArg("bin");

    info.setFile(bin);
    if (!info.isFile()) {
        return false;
    }

    if (!deploy.setTarget(bin)) {
        qCritical() << "error init targeet dir";
        return false;
    }

    auto qmlDir = QuasarAppUtils::getStrArg("qmlDir");

    QDir dir(basePath);

    if (qmlDir.isEmpty()) {
        deploy.setDeployQml(false);
    } else {
        auto scaners = dir.entryList(QStringList() << "qmlimportscanner");
        if (!scaners.size()) {
            qWarning () << "qmlimportscanner not installed";
        } else {
            deploy.setQmlScaner(scaners.first());
        }
    }

    if (!dir.cdUp()) {
        return false;
    }

    deploy.setQtDir(dir.path());

    return true;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if (!QuasarAppUtils::parseParams(argc, argv)) {
        qWarning() << "wrong parametrs";
        help();
        exit(0);

    };

    if (QuasarAppUtils::isEndable("h") ||
            QuasarAppUtils::isEndable("help")) {
        help();
        exit(0);
    }

    Deploy deploy;

    if (!parseQt(deploy)) {
        qCritical() << "qt parse error";
        help();
        exit(1);
    }

    deploy.deploy();

    return a.exec();
}
