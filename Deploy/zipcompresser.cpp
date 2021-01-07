/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "zipcompresser.h"

#include <QDir>
#include "zip.h"

ZipCompresser::ZipCompresser() {

}

bool ZipCompresser::compress(const QString &path, const QString &distArrhive) const {

    QFileInfo arrInfo(distArrhive);
    QFileInfo srcInfo(path);

    if (arrInfo.exists()) {
        QFile::remove(arrInfo.absoluteFilePath());
    }

    if (!QDir().mkpath(arrInfo.absolutePath())) {
        return false;
    }

    if (!srcInfo.exists()) {
        return false;
    }


    auto stdstr = arrInfo.absoluteFilePath().toStdString();
    const char *zipArr = stdstr.c_str();

    struct zip_t *zip = zip_open(zipArr, ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');

    if (!add(srcInfo.absoluteFilePath(), "", zip)) {

        zip_close(zip);
        QFile::remove(arrInfo.absoluteFilePath());

        return false;
    }

    zip_close(zip);

    return true;
}

int on_extract_entry(const char *filename, void *) {
    printf("Extracted: %s \n", filename);

    return 0;
}

bool ZipCompresser::extract(const QString &arrhive, const QString &distDir) const {
    auto stdstr_arr = arrhive.toStdString();
    auto stdstr_dir = distDir.toStdString();

    return zip_extract(stdstr_arr.c_str(),
                       stdstr_dir.c_str(),
                       on_extract_entry, nullptr) == 0;
}

bool ZipCompresser::add(const QString& root, const QString& path, zip_t * zip) const {
    QFileInfo info(root + "/" + path);

    if (info.isFile()) {
        std::string _file_tmp = path.toStdString();

        const char *file = _file_tmp.c_str();

        if (zip_entry_open(zip, file) != 0) {
            return false;
        }

        QFile src(info.absoluteFilePath());
        if (!src.open(QIODevice::ReadOnly)) {
            return false;
        }
        auto data = src.readAll();
        if (zip_entry_write(zip, data.data(), data.size()) != 0) {
            return false;
        }

        src.close();

        return zip_entry_close(zip) == 0;
    }

    QDir dir(info.absoluteFilePath());

    auto list = dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
    for (const auto& entry: list) {
        if (!add(root, (path.size())? path + "/" + entry: entry, zip)) {
            return false;
        }
    }

    return true;
}
