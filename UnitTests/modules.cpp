/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "modules.h"
#include <configparser.h>

Modules::Modules()
{

}

Modules &Modules::instance() {
    static Modules* val = new Modules();

    return *val;
}



