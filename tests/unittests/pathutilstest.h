//
// Copyright (C) 2026 QuasarApp.
// Distributed under the GPLv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#ifndef PATHUTILSTEST_H
#define PATHUTILSTEST_H

#include <QtTest>

/**
 * @brief The PathUtilsTest class
 *
 * Fast unit tests for PathUtils (issue #861). These tests do not need
 * any deployment or a running QuasarApp instance, they only check the
 * pure string handling helpers of the PathUtils class.
 */
class PathUtilsTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void getName();
    void popItem();
    void toFullPath();
    void getReleativePath();
    void stripPath();
    void fullStripPath();
    void isReleativePath();
    void getDrive();
    void isAbsalutPath();
    void fixPath();
    void isPath();
    void getRelativeLink();
};

#endif // PATHUTILSTEST_H
