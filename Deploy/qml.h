//#
//# Copyright (C) 2018-2021 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef QML_DEPLY_H
#define QML_DEPLY_H

#include <QSet>
#include <QStringList>
#include "deploy_global.h"
#include "deploycore.h"

class DEPLOYSHARED_EXPORT QML {
private:

    QStringList extractImportsFromFile(const QString &filepath) const;

    bool extractImportsFromDir(const QString &path, bool recursive = false);

    QString getPathFromImport(const QString& import, bool checkVersions = true);

    bool deployPath( const QString& path, QStringList& res);
    bool scanQmlTree(const QString& qmlTree);
    QStringList extractImportsFromQmlModule(const QString& module) const;
    QString _qmlRoot = "";
    QSet<QString> _imports;
    QSet<QString> secondVersions;
    QtMajorVersion _qtVersion = QtMajorVersion::Qt5;

    QStringList extractImportLine(const QString &line) const;
    
public:
    QML(const QString& qmlRoot, QtMajorVersion isQt6);

    bool scan(QStringList &res, const QString &_qmlProjectDir);
    void setQtVersion(const QtMajorVersion &qtVersion);

    friend class deploytest;
};

#endif // QML_DEPLY_H
