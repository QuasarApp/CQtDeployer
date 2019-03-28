#ifndef QML_H
#define QML_H

#include <QSet>
#include <QStringList>

struct qmlDep {
    QStringList qmlFiles;
    QStringList qmlcFiles;
    QStringList libCFiles;
};

class QML
{
private:
    QStringList extractImportsFromFile(const QString &filepath);
    bool extractImportsFromDir(const QString &path, qmlDep& dep, bool recursive = false);
    QString getPathFromImport(const QString& import);

    bool scanQmlTree(const QString& qmlTree);

    QString _qmlRoot = "";
    QSet<QString> secondVersions;

public:
    QML(const QString& qmlRoot);

    friend class deploytest;
};

#endif // QML_H
