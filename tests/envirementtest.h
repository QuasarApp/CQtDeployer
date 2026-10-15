//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#ifndef ENVIREMENTTEST_H
#define ENVIREMENTTEST_H

#include <QtTest>

/**
 * @brief The EnvirementTest class
 *
 * Fast unit tests for Envirement (issue #861). All test directories and
 * files are created inside a temporary directory, so no deployment or
 * QuasarApp instance is needed.
 */
class EnvirementTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void addAndRemoveEnv();
    void inThisEnvirement();
    void concatEnv();
    void ignoreEnvList();
    void setIgnoreEnvListRecursive();
    void separator();
    void addEnvRec();
    void recursiveInvairement();
    void clear();

private:
    QTemporaryDir m_dir;
    QString m_root;
    QString normalize(const QString& path) const;
};

#endif // ENVIREMENTTEST_H
