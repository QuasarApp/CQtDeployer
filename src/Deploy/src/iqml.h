//#
//# Copyright (C) 2022-2022 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef IQML_H
#define IQML_H

#include <QString>
#include "deploy_global.h"

/**
 * @brief The iQML class Bae interface for scannin qml depends.
 */
class DEPLOYSHARED_EXPORT iQML
{
public:

    /**
     * @brief iQML base constructor
     * @param qmlRoot This is qml root dir of Qt destribution. Example: Qt/6.4.0/gcc_64/qml
     */
    iQML(const QString& qmlRoot);
    virtual ~iQML() = default;

    /**
     * @brief scan This method scane qml files in the qmlRoot and return needed to deploy files
     * @param res This is result of scan.  List of needed to deploy files
     * @param _qmlProjectDir This is
     * @return true if function finished succesful
     */
    virtual bool scan(QStringList &res, const QString &_qmlProjectDir) = 0;

protected:
    QString _qmlRoot;
};

#endif // IQML_H
