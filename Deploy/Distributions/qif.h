#ifndef QIF_H
#define QIF_H

#include "idistribution.h"


/**
 * @brief The QIF class provides interface betvin deployment targets and Qt Installer Framework
 */
class QIF: public iDistribution
{
public:
    QIF();
    Envirement toolKitLocation();
    QString getConfig();
    QString runCmd();

private:

    QString findBinaryCreator();

};

#endif // QIF_H
