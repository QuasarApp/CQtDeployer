/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef EXTRA_H
#define EXTRA_H
#include "deploy_global.h"

#include <QSet>


class DEPLOYSHARED_EXPORT Extra {
    QSet<QString> extraPaths;
    QSet<QString> extraPathsMasks;
    QSet<QString> extraNamesMasks;

    QSet<QString> upper(const QSet<QString> & ) const;

public:
    QSet<QString> getExtraPaths() const;
    void setExtraPaths(const QSet<QString> &value);
    void addExtraPaths(const QSet<QString> &value);

    QSet<QString> getExtraPathsMasks() const;
    void setExtraPathsMasks(const QSet<QString> &value);
    void addExtraPathsMasks(const QSet<QString> &value);

    QSet<QString> getExtraNamesMasks() const;
    void setExtraNamesMasks(const QSet<QString> &value);
    void addtExtraNamesMasks(const QSet<QString> &value);

    bool contains(const QString &path) const;

};
#endif // EXTRA_H
