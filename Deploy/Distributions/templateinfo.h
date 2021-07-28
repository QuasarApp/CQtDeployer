#ifndef TEMPLATEINFO_H
#define TEMPLATEINFO_H

#include <QString>
#include <QHash>
#include <deploy_global.h>

/**
 * @brief The TemplateInfo struct This structure contains information about distribution packages.
 */
struct DEPLOYSHARED_EXPORT TemplateInfo
{
    QString Name;
    QString Description;
    QString Version;
    QString ReleaseData;
    QString Icon;
    QString Publisher;
    QString Homepage;
    QString Prefix;
    QString InstallDirDEB;
    QString InstallDeirQIFW() const;

    QHash<QString, QString> Custom;

};

#endif // TEMPLATEINFO_H
