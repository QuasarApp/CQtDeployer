#ifndef DEFINES_H
#define DEFINES_H
#include <QtGlobal>

#if QT_VERSION > QT_VERSION_CHECK(5, 13, 0)
            #define splitbehavior Qt::SkipEmptyParts
#else
            #define splitbehavior QString::SkipEmptyParts
#endif

#endif // DEFINES_H
