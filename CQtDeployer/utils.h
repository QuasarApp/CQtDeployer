/*
 * Copyright (C) 2018 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef UTILS_H
#define UTILS_H

#include <QDir>

class Utils
{
private:
    static inline bool snapCd(QDir &dir, const QString& pathTo);
    static inline bool snapCdUp(QDir &dir);

public:
    explicit Utils();
    static bool cd(QDir &dir, const QString &pathTo);
    static bool cdUp(QDir& dir);
    static void verboseLog(const QString &str);
};
#endif // UTILS_H
