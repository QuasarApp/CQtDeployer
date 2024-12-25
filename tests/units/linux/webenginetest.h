//#
//# Copyright (C) 2020-2024 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#ifndef WEBENGINE_TEST_H
#define WEBENGINE_TEST_H
#include "testbase.h"
#include "testutils.h"

#include <QtTest>

// webenginetest
class WebEngineTest: public TestBase, protected TestUtils
{
public:
    void test() override;

};

#endif // WEBENGINE_TEST_H
