/*
 * Copyright (C) 2018-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "qmlcreator.h"

#include <QFile>
#include <QFileInfo>

QMap<QString, QStringList> QmlCreator::getQmlImports() const {
    return qmlImports;
}

QStringList QmlCreator::getCopyedQml() const {
    return copyedQml;
}

void QmlCreator::createQml(const QString &qmlFile, const QStringList &imports) {
    QFile qml(qmlFile);

    if (qml.open(QIODevice::ReadOnly)) {

        QFile target(path + "/" + QFileInfo(qmlFile).fileName());

        if (target.open(QIODevice::ReadWrite)) {

            auto data = qml.readAll();
            target.write(data.data(), data.size());

            target.close();

            copyedQml.push_back(target.fileName());
            qmlImports.insert(target.fileName(), imports);
        }

        qml.close();
    }
}

void QmlCreator::initQml() {
    createQml(":/qmlFile.qml", {
                  "2#QtQuick",
                  "2#QtQuick/Controls/Material",
                  "2#QtQuick/Controls",
                  "1#QtQuick/Layouts"
              });
}

QmlCreator::QmlCreator(const QString &path) {
    this->path = path;
    initQml();
}


QmlCreator::~QmlCreator() {

    for(auto &lib : copyedQml) {
        QFile::remove(lib);
    }

}
