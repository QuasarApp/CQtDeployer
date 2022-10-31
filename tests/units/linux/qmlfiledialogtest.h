//#
//# Copyright (C) 2020-2022 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#ifndef QML_FILE_DIALOG_TEST_H
#define QML_FILE_DIALOG_TEST_H
#include "testbase.h"
#include "testutils.h"

#include <QtTest>

class QmlFileDialogTest: public TestBase, protected TestUtils
{
public:
    void test() override;

};

#endif // QML_FILE_DIALOG_TEST_H
