#ifndef LIBINFO_H
#define LIBINFO_H

#include <QString>
#include <QStringList>

enum Platform {
    UnknownPlatform = 0x0,
    Win32,
    Win64,
    Unix32,
    Unix64
};

struct LibInfo {
    Platform platform = Platform::UnknownPlatform;
    QString name;
    QString path;
    QStringList dependncies;

    bool operator == (const LibInfo& other);

    QString fullPath();

    void clear();

    bool isValid() const;
};

#endif // LIBINFO_H
