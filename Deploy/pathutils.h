//#
//# Copyright (C) 2018-2019 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#include <QString>
#include "deploy_global.h"

#ifndef PATHUTILS_H
#define PATHUTILS_H

/**
 * @brief The PathUtils class
 *
 */
class DEPLOYSHARED_EXPORT PathUtils
{
public:
    PathUtils();
    static QString toFullPath(QString path);
    static QString getReleativePath(QString path);
    static QString stripPath(QString path) ;
    static QString getRelativeLink(QString from, QString to);
    static bool isPath(const QString &path);
};

#endif // PATHUTILS_H
