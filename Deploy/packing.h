#ifndef PACKING_H
#define PACKING_H

#include <QObject>
#include <QProcess>
#include "deploy_global.h"

/**
 * @brief The Packing class - this class have interface for configure package and
 * create package from deployment distibution.
 */

class ConfigParser;
class iDistribution;

class DEPLOYSHARED_EXPORT Packing : public QObject

{
    Q_OBJECT
public:
    Packing();
    ~Packing();
    void setDistribution(iDistribution *pakage);
    bool create();
private:
    iDistribution *_pakage = nullptr;
    QProcess *_proc = nullptr;

private slots:
    void handleOutputUpdate();
};

#endif // PACKING_H
