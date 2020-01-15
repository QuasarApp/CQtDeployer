#ifndef QIF_H
#define QIF_H

#include "idistribution.h"


/**
 * @brief The QIF class provides interface betvin deployment targets and Qt Installer Framework
 */
class DEPLOYSHARED_EXPORT QIF: public iDistribution
{
public:
    QIF();
    Envirement toolKitLocation() const;
    QString getConfig() const;
    QString runCmd() const;
    bool deployTemplate() const;
    QStringList runArg() const;
    bool removeTemplate() const;
};

#endif // QIF_H
