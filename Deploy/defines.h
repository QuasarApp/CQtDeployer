/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef DEFINES_H
#define DEFINES_H
#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
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
