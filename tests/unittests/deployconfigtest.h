//
// Copyright (C) 2026 QuasarApp.
// Distributed under the GPLv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#ifndef DEPLOYCONFIGTEST_H
#define DEPLOYCONFIGTEST_H

#include <QtTest>

/**
 * @brief The DeployConfigTest class
 *
 * Fast unit tests for DeployConfig (issue #861).
 */
class DeployConfigTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void targetDirAndIgnoreList();
    void getTargetDir();
    void targetsEdit();
    void packagesEdit();
    void getDistro();
    void platform();
    void defaultPackage();
    void isNeededQt();
    void customPlatform();
    void reset();
};

#endif // DEPLOYCONFIGTEST_H
