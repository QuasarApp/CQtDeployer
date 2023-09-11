//#
//# Copyright (C) 2023-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef EXTRADEPENDSTEST_H
#define EXTRADEPENDSTEST_H

#include "testbase.h"
#include "testutils.h"

#include <QtTest>

class ExtraDependsTest: public TestBase, protected TestUtils
{
public:
    ExtraDependsTest();
    void test() override;

};

#endif // EXTRADEPENDSTEST_H
