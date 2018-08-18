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
    qInfo() << "   help / h                 : show help.";
    qInfo() << "   always-overwrite         : Copy files even if the target file exists.";
    qInfo() << "   -bin    [params]         : deployment binry.";
    qInfo() << "   -qmlDir [params]         : qml datadir. for example -qmlDir ~/Qt/5.11.1/gcc_64/qml";
    qInfo() << "   noStrip                  : no strip deployed lib";
    qInfo() << "   deploy-not-qt            : deploy all libs";
    qInfo() << "   -qmake  [params]         : qmake path. for example";
    qInfo() << "                            | for example  -qmake ~/Qt/5.11.1/gcc_64/bin/qmake";
    qInfo() << "   ignoreCudaLib            : it filter ignore cuda lib of nvidea";
    qInfo() << "   -ignore [list,params]    : ignore filter for libs";
    qInfo() << "                            | for example -ignore libicudata.so.56,libicudata2.so.56";
    qInfo() << "   clear                    : delete all old deploy data";

}

bool parseQt(Deploy& deploy) {
    auto qmake = QuasarAppUtils::getStrArg("qmake");
    QString basePath = "";
    QFileInfo info(qmake);
    if (!info.isFile() || (info.baseName() != "qmake")) {
        return false;
    }
    basePath = info.absolutePath();
    deploy.setQmake(qmake);

    auto bin = QuasarAppUtils::getStrArg("bin");

    info.setFile(bin);
    if (!info.isFile()) {
        return false;
    }

    if (QuasarAppUtils::isEndable("clear")) {
        qInfo() << "clear old data";
        deploy.clear();
    }

    if (!deploy.setTarget(bin)) {
        qCritical() << "error init targeet dir";
        return false;
    }

    auto qmlDir = QuasarAppUtils::getStrArg("qmlDir");

    QDir dir(basePath);

    if (QFileInfo::exists(qmlDir)) {
        deploy.setDeployQml(true);
    } else {
        qCritical () << "wrong qml dir!";
    }

    if (!dir.cdUp()) {
        return false;
    }

    deploy.setQtDir(dir.absolutePath());

    return true;
}

int main(int argc, char *argv[])
{

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

    qInfo() << "deploy done!";

    return 0;
}
