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
    void setDistribution(const QList<iDistribution*> &pakages);
    bool create();
private:

    void collectPackages();

    QList<iDistribution*> _pakage;
    QProcess *_proc = nullptr;
    QHash<QString, QString> _packages;

private slots:
    void handleOutputUpdate();

};

#endif // PACKING_H
