#ifndef PACKING_H
#define PACKING_H

#include <QObject>
#include <QProcess>
#include "deploy_global.h"
#include "packagecontrol.h"

/**
 * @brief The Packing class - this class have interface for configure package and
 * create package from deployment distibution.
 */

class ConfigParser;
class iDistribution;
class FileManager;

class DEPLOYSHARED_EXPORT Packing : public QObject, public PackageControl
{
    Q_OBJECT
public:
    Packing(FileManager *fileManager);
    ~Packing();
    void setDistribution(const QList<iDistribution*> &pakages);
    bool create();

    bool movePackage(const QString &package, const QString &newLocation);

private:

    bool collectPackages();
    bool moveData(const QString& from, const QString& to,
                  const QString &ignore = "") const;

    bool restorePackagesLocations();

    QList<iDistribution*> _pakage;
    QProcess *_proc = nullptr;
    QHash<QString, QString> _packagesLocations;
    QHash<QString, QString> _defaultPackagesLocations;

    FileManager* _fileManager = nullptr;

private slots:
    void handleOutputUpdate();


};

#endif // PACKING_H
