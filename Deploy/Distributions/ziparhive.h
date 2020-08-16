#ifndef ZIPARHIVE_H
#define ZIPARHIVE_H
#include "idistribution.h"

/**
 * @brief The ZipArhive class provide interface for reate final zip arhive
 */
class DEPLOYSHARED_EXPORT ZipArhive: public iDistribution
{
public:
    ZipArhive(FileManager *fileManager);

    // iDistribution interface
public:
    bool deployTemplate(PackageControl &pkg) override;
    bool removeTemplate() const override;
    Envirement toolKitEnv() const override;
    QProcessEnvironment processEnvirement() const override;
    QString runCmd() override;
    QStringList runArg() const override;
    QStringList outPutFiles() const override;
private:
    QStringList outFiles;
};

#endif // ZIPARHIVE_H
