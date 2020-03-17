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
    Envirement toolKitEnv() const override;
    QString runCmd() override;
    bool deployTemplate() override;
    QStringList runArg() const override;
    bool removeTemplate() const override;
    QProcessEnvironment processEnvirement() const override;
    QStringList outPutFiles() const override;

private:
    QString binarycreator;
    TemplateInfo generalInfo;
    QString getStyle(const QString &input) const;
    QString installerFile() const;


};

#endif // QIF_H
