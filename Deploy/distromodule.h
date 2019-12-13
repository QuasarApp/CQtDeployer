#ifndef DISTROMODULE_H
#define DISTROMODULE_H

#include "distrostruct.h"

#include <QSet>

class DEPLOYSHARED_EXPORT DistroModule: public DistroStruct
{
public:
    DistroModule();
    QSet<QString> targets() const;
    void setTargets(const QSet<QString> &targets);
    void addTarget(const QString& target);
private:

    QSet<QString> _targets;
};

#endif // DISTROMODULE_H
