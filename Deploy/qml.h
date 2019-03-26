#ifndef QML_H
#define QML_H

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

    QString _qmlRoot = "";

public:
    QML(const QString& qmlRoot);

    friend class deploytest;
};

#endif // QML_H
