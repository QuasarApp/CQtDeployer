//
// Copyright (C) 2026 QuasarApp.
// Distributed under the GPLv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#ifndef TARGETINFOTEST_H
#define TARGETINFOTEST_H

#include <QtTest>

/**
 * @brief The TargetInfoTest class
 *
 * Fast unit tests for TargetInfo (issue #861).
 */
class TargetInfoTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void package();
    void icon();
    void shortCut();
    void runScript();
    void runScriptFile();
};

#endif // TARGETINFOTEST_H
