#ifndef LIBINFO_H
#define LIBINFO_H
#include "deploycore.h"


class DEPLOYSHARED_EXPORT LibInfo {
private:
    QSet<LibInfo> allDep;

    Platform platform = Platform::UnknownPlatform;
    QString name;
    QString path;
    QSet<QString> dependncies;
    QString qtPath;
    int abiVersion;
    LibPriority priority = NotFile;

public:

    friend bool operator == (const LibInfo& left ,const LibInfo& right);
    friend bool operator > (const LibInfo& left, const LibInfo& right);
    friend bool operator < (const LibInfo& left, const LibInfo& right);
    friend bool operator >= (const LibInfo& left, const LibInfo& right);
    friend bool operator <= (const LibInfo& left, const LibInfo& right);

    QString fullPath() const;

    void clear();

    bool isValid() const;

    friend class DependenciesScanner;
    const QSet<LibInfo>& getAllDep() const;
    Platform getPlatform() const;
    void setPlatform(const Platform &value);
    QString getName() const;
    void setName(const QString &value);
    QString getPath() const;
    void setPath(const QString &value);
    QSet<QString> getDependncies() const;
    void setDependncies(const QSet<QString> &value);
    void addDependncies(const QString &value);
    void removeDependncies(const QString &value);

    LibPriority getPriority() const;
    void setPriority(const LibPriority &value);
    QString getQtPath() const;
    void setQtPath(const QString &value);
    int getAbiVersion() const;
    void setAbiVersion(int value);
};

uint qHash(const LibInfo& info);
#endif // LIBINFO_H
