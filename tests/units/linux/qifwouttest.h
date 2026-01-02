//#
//# Copyright (C) 2020-2026 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#ifndef QIFW_QUT_TEST_H
#define QIFW_QUT_TEST_H
#include "testbase.h"
#include "testutils.h"

#include <QtTest>

class QIFWOutTest: public TestBase, protected TestUtils
{
public:
    void test() override;

};

#endif // QIFW_QUT_TEST_H
