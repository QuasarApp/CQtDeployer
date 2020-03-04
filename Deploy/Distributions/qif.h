#ifndef QIF_H
#define QIF_H

#include "idistribution.h"


/**
 * @brief The QIF class provides interface betvin deployment targets and Qt Installer Framework
 */
class DEPLOYSHARED_EXPORT QIF: public iDistribution
{
public:
    QIF(FileManager *fileManager);
    Envirement toolKitEnv() const;
    QString getConfig() const;
    QString runCmd();
    bool deployTemplate();
    QStringList runArg() const;
    bool removeTemplate() const;
    QProcessEnvironment processEnvirement() const;
private:
    QString binarycreator;
    TemplateInfo generalInfo;
    QString getStyle(const QString &input) const;

};

#endif // QIF_H
