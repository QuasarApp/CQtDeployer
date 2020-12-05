#ifndef DEFINES_H
#define DEFINES_H
#include <QtGlobal>

#if QT_VERSION > QT_VERSION_CHECK(5, 13, 0)
            #define splitbehavior Qt::SkipEmptyParts
#else
            #define splitbehavior QString::SkipEmptyParts
#endif

template<class Container, class Key, class T>
T& valueLink(Container& ctr, const Key& key, const T& defaultValue) {
    auto it = ctr.find(key);
    if (it != ctr.end()) {
        return *it;
    }

    return *ctr.insert(key, defaultValue);
};

#endif // DEFINES_H
