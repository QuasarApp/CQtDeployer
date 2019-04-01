#ifndef QML_DEPLY_H
#define QML_DEPLY_H

#include <QSet>
#include <QStringList>

class QML {
private:
    QStringList extractImportsFromFile(const QString &filepath);
    bool extractImportsFromDir(const QString &path, bool recursive = false);
    QString getPathFromImport(const QString& import);
    bool deployPath( const QString& path, QStringList& res);
    bool scanQmlTree(const QString& qmlTree);
    void addImport();
    QString _qmlRoot = "";
    QSet<QString> _imports;
    QSet<QString> secondVersions;

public:
    QML(const QString& qmlRoot);

    bool scan(QStringList &res, const QString &_qmlProjectDir);

    friend class deploytest;
};

#endif // QML_DEPLY_H
