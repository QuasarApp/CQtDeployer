#ifndef TEMPLATEINFO_H
#define TEMPLATEINFO_H

#include <QString>
#include <QHash>
#include <deploy_global.h>

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

    QHash<QString, QString> Custom;

};

#endif // TEMPLATEINFO_H
