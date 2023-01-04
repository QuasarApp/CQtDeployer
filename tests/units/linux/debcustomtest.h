//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#ifndef DEB_CUSTOM_TEST_H
#define DEB_CUSTOM_TEST_H
#include "testbase.h"
#include "testutils.h"

#include <QtTest>

class DEBCustomTest: public TestBase, protected TestUtils
{
public:
    void test() override;

};

#endif // DEB_CUSTOM_TEST_H
