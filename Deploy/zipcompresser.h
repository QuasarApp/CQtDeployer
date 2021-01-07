/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef ZIPCOMPRESSER_H
#define ZIPCOMPRESSER_H

#include <QString>
#include <deploy_global.h>

class zip_t;

/**
 * @brief The ZipCompresser class - this is cpp wraper for use zip C library
 */
class DEPLOYSHARED_EXPORT ZipCompresser
{
public:
    ZipCompresser();

    /**
     * @brief compress - create a new zip arrhive from folder
     * @param path - path to folder for commpressing
     * @param distArrhive - path to arrhive
     * @return true if new arrhive created successsful
     */
    bool compress(const QString& path, const QString& distArrhive) const;

    /**
     * @brief extract - extract all files from zip arhive
     * @param arrhive - The extracted arrhive
     * @param distDir - the path to extracted files from arrhive
     * @return true if extract a arrhive finished successful
     */
    bool extract(const QString& arrhive, const QString& distDir) const;


private:
    /**
     * @brief add - recursive add files in to zip
     * @param root - root path of arrhived dir
     * @param path - path to file
     * @param zip
     * @return
     */
    bool add(const QString &root, const QString &path, zip_t *zip) const;
};

#endif // ZIPCOMPRESSER_H
