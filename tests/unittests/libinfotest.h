//
// Copyright (C) 2026 QuasarApp.
// Distributed under the GPLv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#ifndef LIBINFOTEST_H
#define LIBINFOTEST_H

#include <QtTest>

/**
 * @brief The LibInfoTest class
 *
 * Fast unit tests for LibInfo (issue #861).
 */
class LibInfoTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void defaultValues();
    void namePathAndFullpath();
    void isValid();
    void platform();
    void dependencies();
    void isDependetOfQt();
    void priority();
    void rpath();
    void winApi();
    void clear();
    void compare();
};

#endif // LIBINFOTEST_H
