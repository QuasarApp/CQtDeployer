//#
//# Copyright (C) 2020-2024 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#ifndef CONF_FILE_TEST_H
#define CONF_FILE_TEST_H
#include "testbase.h"
#include "testutils.h"

#include <QtTest>

class ConfFileTest: public TestBase, protected TestUtils
{
public:
    void test() override;

};

#endif // CONF_FILE_TEST_H
