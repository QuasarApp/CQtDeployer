/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef DEPLOY_GLOBAL_H
#define DEPLOY_GLOBAL_H

#include <QtCore/qglobal.h>

#define DEFAULT_COFIGURATION_FILE           QString("CQtDeployer.json")
#define DISTRO_DIR                          QString("DistributionKit")


#ifdef Q_OS_WIN
#define ONLY_WIN_CASE_INSENSIATIVE Qt::CaseInsensitive
#else
#define ONLY_WIN_CASE_INSENSIATIVE Qt::CaseSensitive
#endif

#if defined(DEPLOY_LIBRARY)
#  define DEPLOYSHARED_EXPORT Q_DECL_EXPORT
#else
#  define DEPLOYSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // DEPLOY_GLOBAL_H
