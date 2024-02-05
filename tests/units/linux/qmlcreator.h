/*
 * Copyright (C) 2018-2024 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef QMLCREATOR_H
#define QMLCREATOR_H

#include <QMap>
#include <QStringList>



class QmlCreator
{
private:
    QString path;
    QStringList copyedQml;

    QMap<QString, QStringList> qmlImports;
    void createQml(const QString& resLib, const QStringList& imports);

    void initQml();
public:
    QmlCreator(const QString &path);
    ~QmlCreator();
    QMap<QString, QStringList> getQmlImports() const;
    QStringList getCopyedQml() const;
};

#endif // QMLCREATOR_H
