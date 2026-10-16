//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#ifndef DEPLOYCORETEST_H
#define DEPLOYCORETEST_H

#include <QtTest>

/**
 * @brief The DeployCoreTest class
 *
 * Fast unit tests for the DeployCore platform helpers (issue #855):
 * the runtime flavor compatibility check used by the dependency
 * scanner to resolve libraries and the platform string conversions.
 */
class DeployCoreTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void platformCompatibility();
    void platformStrings();
};

#endif // DEPLOYCORETEST_H