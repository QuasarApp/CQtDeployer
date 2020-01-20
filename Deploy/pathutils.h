//#
//# Copyright (C) 2018-2020 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#



#ifndef PATHUTILS_H
#define PATHUTILS_H

#include <QString>
#include "deploy_global.h"

#ifdef Q_OS_WIN
#define ONLY_WIN_CASE_INSENSIATIVE Qt::CaseInsensitive
#else
#define ONLY_WIN_CASE_INSENSIATIVE Qt::CaseSensitive
#endif

/**
 * @brief The PathUtils class
 *
 */
class DEPLOYSHARED_EXPORT PathUtils
{
public:
    PathUtils();
    /**
     * @brief toFullPath remove all dublicate separators and insert separators into begin and end
     * @param path
     * @return return full path
     */
    static QString toFullPath(QString path);

    /**
     * @brief getReleativePath
     * @param path
     * @return return releative path from "path" to root dir
     */
    static QString getReleativePath(QString path);

    /**
     * @brief stripPath remove all dublicate separators and remove separators from begin and end
     * @param path
     * @return return strip patth
     */
    static QString stripPath(QString path);

    /**
     * @brief getRelativeLink return releative path
     * @param from
     * @param to
     * @return releative path
     */
    static QString getRelativeLink(QString from, QString to);

    /**
     * @brief isPath
     * @param path
     * @return return true if imput value is path
     */
    static bool isPath(const QString &path);

    /**
     * @brief isPath
     * @param path
     * @return return true if imput value is Releative path
     */
    static bool isReleativePath(const QString &path);

    /**
     * @brief getDrive
     * @param path
     * @return char of drive in windows systems
     */
    static QChar getDrive(QString path);

    /**
     * @brief isAbsalutPath
     * @param path
     * @return return true if path is absolute
     */
    static bool isAbsalutPath(const QString &path);

    /**
     * @brief fixPath - this method make upper case for all symbols of path on windows os. on linux doing nothing
     * @param path - a input path
     * @return return a fixed path
     */
    static QString fixPath(const QString & path);

};

#endif // PATHUTILS_H
