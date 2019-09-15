#ifndef MODULES_H
#define MODULES_H

#include "testutils.h"

#include <QSet>



class Modules
{
private:
public:
    Modules();

    static QSet<QString> qtLibs();
    static QSet<QString> qmlLibs();
    static QSet<QString> qtWithoutTr();

};

#endif // MODULES_H
