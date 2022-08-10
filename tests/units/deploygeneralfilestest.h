//#
//# Copyright (C) 2020-2022 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#ifndef DEPLOY_GENERAL_FILES_TEST_H
#define DEPLOY_GENERAL_FILES_TEST_H
#include "testbase.h"
#include "testutils.h"

#include <QtTest>

class DeployGeneralFilesTest: public TestBase, protected TestUtils
{
public:
    void test() override;

};

#endif // DEPLOY_GENERAL_FILES_TEST_H
