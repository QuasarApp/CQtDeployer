//#
//# Copyright (C) 2020-2022 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#ifndef STRIP_TEST_H
#define STRIP_TEST_H
#include "testbase.h"
#include "testutils.h"

#include <QtTest>

class StripTest: public TestBase, protected TestUtils
{
public:
    StripTest();
    ~StripTest();

    void test();

};

#endif // DeployTarget_TEST_H
