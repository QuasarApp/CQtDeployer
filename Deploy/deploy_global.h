/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef DEPLOY_GLOBAL_H
#define DEPLOY_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DEPLOY_LIBRARY)
#  define DEPLOYSHARED_EXPORT Q_DECL_EXPORT
#else
#  define DEPLOYSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // DEPLOY_GLOBAL_H
