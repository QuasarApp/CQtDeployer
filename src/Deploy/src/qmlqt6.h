//#
//# Copyright (C) 2018-2022 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef QML_QT6_DEPLY_H
#define QML_QT6_DEPLY_H

#include <QSet>
#include <QStringList>
#include "iqml.h"

class DEPLOYSHARED_EXPORT QMLQt6: public iQML {

public:
    QMLQt6(const QString& qmlRoot);
    ~QMLQt6() override;

    bool scan(QStringList &res, const QString &_qmlProjectDir) override;


private:

    QStringList extractImportsFromFile(const QString &filepath) const;

    bool extractImportsFromDir(const QString &path, bool recursive = false);

    QString getPathFromImport(const QString& import, bool checkVersions = true);

    bool deployPath( const QString& path, QStringList& res);
    bool scanQmlTree(const QString& qmlTree);
    QStringList extractImportsFromQmlModule(const QString& module) const;
    QStringList extractImportLine(const QString &line) const;
    
    QSet<QString> _imports;
    QSet<QString> secondVersions;

    friend class deploytest;
    friend class QmlScanerTest;
    friend class QMLExtractTest;
};

#endif // QML_QT6_DEPLY_H
