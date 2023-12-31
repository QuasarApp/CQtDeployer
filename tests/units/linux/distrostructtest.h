//#
//# Copyright (C) 2020-2024 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#ifndef DISTRO_STRUCT_TEST_H
#define DISTRO_STRUCT_TEST_H
#include "testbase.h"
#include "testutils.h"

#include <QtTest>

class DistroStructTest: public TestBase, protected TestUtils
{
public:
    void test() override;

};

#endif // DISTRO_STRUCT_TEST_H
