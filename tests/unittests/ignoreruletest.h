//
// Copyright (C) 2026 QuasarApp.
// Distributed under the GPLv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#ifndef IGNORERULETEST_H
#define IGNORERULETEST_H

#include <QtTest>

/**
 * @brief The IgnoreRuleTest class
 *
 * Fast unit tests for IgnoreRule and IgnoreData (issue #861).
 */
class IgnoreRuleTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void label();
    void platform();
    void priority();
    void environmentRule();
};

#endif // IGNORERULETEST_H
