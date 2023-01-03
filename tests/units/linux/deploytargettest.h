//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#ifndef DeployTarget_TEST_H
#define DeployTarget_TEST_H
#include "testbase.h"
#include "testutils.h"

#include <QtTest>

class DeployTargetTest: public TestBase, protected TestUtils
{
public:
    DeployTargetTest();
    ~DeployTargetTest();

    void test();

};

#endif // DeployTarget_TEST_H
