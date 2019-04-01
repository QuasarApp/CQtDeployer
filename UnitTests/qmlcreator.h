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
