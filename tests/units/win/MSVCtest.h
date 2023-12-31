//#
//# Copyright (C) 2020-2024 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#ifndef MSVC_TEST_H
#define MSVC_TEST_H
#include "testbase.h"
#include "testutils.h"

#include <QtTest>

class MSVCTest: public TestBase, protected TestUtils
{
public:
    MSVCTest();
    ~MSVCTest();

    void test();

};

#endif // MSVC_TEST_H
