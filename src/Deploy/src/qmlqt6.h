//#
//# Copyright (C) 2018-2026 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef QML_QT6_DEPLY_H
#define QML_QT6_DEPLY_H

#include <QMap>
#include <QSet>
#include <QStringList>
#include "iqml.h"

class DEPLOYSHARED_EXPORT QMLQt6: public iQML {

public:
    QMLQt6(const QString& qmlRoot);
    ~QMLQt6() override;

    bool scan(QStringList &res, const QString &qmlProjectDir) override;

    // Reads the "module <uri>" line of a qmldir file and returns the module uri.
    // Returns an empty string if the qmldir has no module line.
    static QString extractModuleName(const QString &qmldirPath);


private:

    QStringList extractImportsFromFile(const QString &filepath) const;

    bool extractImportsFromDir(const QString &path, bool recursive = false);

    QString getPathFromImport(const QString& import, bool checkVersions = true);

    bool deployPath( const QString& path, QStringList& res);
    bool scanQmlTree(const QString& qmlTree);
    QStringList extractImportsFromQmlModule(const QString& module) const;
    QStringList extractImportLine(const QString &line) const;

    // Scans the qml project dir for qmldir files and maps the declared module
    // uri to the directory that contains the module. Custom auto generated qml
    // modules (created by qt_add_qml_module without the NO_PLUGIN option) are
    // built into the project build dir and are not part of the Qt distribution,
    // so they cannot be resolved against the qml root.
    // See the https://github.com/QuasarApp/CQtDeployer/issues/847 issue.
    void scanProjectModules();

    QSet<QString> _imports;
    QSet<QString> secondVersions;
    QString _qmlProjectDir;
    QMap<QString, QString> _moduleDirs;

    friend class deploytest;
    friend class QmlScanerTest;
    friend class QMLExtractTest;
};

#endif // QML_QT6_DEPLY_H
