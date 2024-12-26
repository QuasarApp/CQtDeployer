/*
 * Copyright (C) 2018-2024 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef MODULES_H
#define MODULES_H

#include <configparser.h>
#include <QSet>

#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
    #include "modulesqt6_8.h"
    class Modules: public ModulesQt6_8
#elif QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
    #include "modulesqt6_6.h"
    class Modules: public ModulesQt6_6
#elif QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    #include "modulesqt6_5.h"
    class Modules: public ModulesQt6_5

#elif QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
    #include "modulesqt6_4.h"
    class Modules: public ModulesQt6_4

#elif QT_VERSION >= QT_VERSION_CHECK(6, 3, 0)
    #include "modulesqt6_3.h"
    class Modules : public ModulesQt63
#elif QT_VERSION >= QT_VERSION_CHECK(5, 15, 2)
    #include "modulesqt5152.h"
    class Modules : public ModulesQt5152

#elif QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    #include "modulesqt515.h"
    class Modules : public ModulesQt515

#elif QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    #include "modulesqt514.h"
    class Modules : public ModulesQt514

#elif QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
    #include "modulesqt513.h"
    class Modules : public ModulesQt513

#else
    #include "modulesqt513.h"
    class Modules : public ModulesQt513

#endif

{
public:
    Modules();

    static Modules& instance();
};

#define TestModule Modules::instance()

#endif // MODULES_H
