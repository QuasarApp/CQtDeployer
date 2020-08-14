#ifndef ZIPARHIVE_H
#define ZIPARHIVE_H
#include "idistribution.h"

/**
 * @brief The ZipArhive class provide interface for reate final zip arhive
 */
class ZipArhive: public iDistribution
{
public:
    ZipArhive(FileManager *fileManager);

    // iDistribution interface
public:
    bool deployTemplate();
    bool removeTemplate() const;
    Envirement toolKitEnv() const;
    QProcessEnvironment processEnvirement() const;
    QString runCmd();
    QStringList runArg() const;
    QStringList outPutFiles() const;
private:
    QStringList outFiles;
};

#endif // ZIPARHIVE_H
