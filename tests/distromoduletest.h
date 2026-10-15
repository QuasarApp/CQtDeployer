//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#ifndef DISTROMODULETEST_H
#define DISTROMODULETEST_H

#include <QtTest>

/**
 * @brief The DistroModuleTest class
 *
 * Fast unit tests for DistroModule (issue #861).
 */
class DistroModuleTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void key();
    void isValid();
    void targets();
    void qmlInput();
    void metaData();
    void plugins();
    void extraData();
    void translation();
    void outputs();
    void copy();
    void inheritedDistroStruct();
};

#endif // DISTROMODULETEST_H