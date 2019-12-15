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

    QSet<QString> qmlInput() const;
    void setQmlInput(const QSet<QString> &qmlInput);
    void addQmlInput(const QString& target);


private:

    QSet<QString> _targets;
    QSet<QString> _qmlInput;
};

#endif // DISTROMODULE_H
