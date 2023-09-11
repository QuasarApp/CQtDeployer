//#
//# Copyright (C) 2018-2023 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef QML_QT5_DEPLY_H
#define QML_QT5_DEPLY_H

#include <QSet>
#include <QStringList>
#include "iqml.h"

class DEPLOYSHARED_EXPORT QMLQt5: public iQML {
private:

    QStringList extractImportsFromFile(const QString &filepath) const;

    bool extractImportsFromDir(const QString &path, bool recursive = false);

    QString getPathFromImport(const QString& import, bool checkVersions = true);

    bool deployPath( const QString& path, QStringList& res);
    bool scanQmlTree(const QString& qmlTree);
    QStringList extractImportsFromQmlModule(const QString& module) const;
    QSet<QString> _imports;
    QSet<QString> secondVersions;

    QStringList extractImportLine(const QString &line) const;
    
public:
    QMLQt5(const QString& qmlRoot);
    ~QMLQt5() override;

    bool scan(QStringList &res, const QString &_qmlProjectDir) override;

    friend class deploytest;
    friend class QmlScanerTest;
    friend class QMLExtractTest;
};

#endif // QML_QT5_DEPLY_H
