/*
 * Copyright (C) 2018-2024 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef PACKING_H
#define PACKING_H

#include <QHash>
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
    ~Packing() override;
    void setDistribution(const QList<iDistribution*> &pakages);
    bool create();

    bool movePackage(const QString &package, const QString &newLocation) override;
    bool copyPackage(const QString &package, const QString &newLocation) override;

    /**
     * @brief extractTemplates This method extract all default temlates. See the getDefaultTemplate for get more information.
     * @return true if function finished successful.
     */
    bool extractTemplates();

protected:
    QStringList availablePackages() const override;

private:

    bool collectPackages();
    bool prepareTemplatesForExtract();

    bool moveData(const QString& from, const QString& to,
                  const QString &ignore = "") const;

    bool restorePackagesLocations();

    /**
     * @brief calcDistributiveHash This method calc md hashes for all packages of the @a distro distributive
     * @param distro This is pointer to the distributive object.
     */
    void calcDistributiveHash(const iDistribution* distro);

    /**
     * @brief calcHash This method calc md5 hash of the file.
     * @param file This is a file path
     * @return string value of the file hash. Using the hex.
     */
    QByteArray calcHash(const QString& file);

    QList<iDistribution*> _pakages;
    QProcess *_proc = nullptr;
    QHash<QString, QString> _packagesLocations;
    QHash<QString, QString> _defaultPackagesLocations;

    FileManager* _fileManager = nullptr;

private slots:
    void handleOutputUpdate();

};

#endif // PACKING_H
