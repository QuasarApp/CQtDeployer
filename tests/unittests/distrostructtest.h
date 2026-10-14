//
// Copyright (C) 2026 QuasarApp.
// Distributed under the GPLv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#ifndef DISTROSTRUCTTEST_H
#define DISTROSTRUCTTEST_H

#include <QtTest>

/**
 * @brief The DistroStructTest class
 *
 * Fast unit tests for DistroStruct (issue #861).
 */
class DistroStructTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void defaults();
    void outDirs();
    void customBasePath();
    void empty();
};

#endif // DISTROSTRUCTTEST_H