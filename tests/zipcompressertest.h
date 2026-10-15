//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#ifndef ZIPCOMPRESSTERTEST_H
#define ZIPCOMPRESSTERTEST_H

#include <QTemporaryDir>
#include <QtTest>

/**
 * @brief The ZipCompresserTest class
 *
 * Fast unit tests for ZipCompresser (issue #861).
 */
class ZipCompresserTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void compress();
    void extract();

private:
    QTemporaryDir m_dir;
    QString m_source;
    QString m_zipFile;
};

#endif // ZIPCOMPRESSTERTEST_H