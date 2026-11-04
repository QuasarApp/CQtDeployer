//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#ifndef FILEMANAGERTTEST_H
#define FILEMANAGERTTEST_H

#include <QTemporaryDir>
#include <QtTest>

/**
 * @brief The FileManagerTest class
 *
 * Fast unit tests for FileManager (issue #861). All file operations
 * are performed inside a temporary directory.
 */
class FileManagerTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void initDir();
    void copyFile();
    void copyFiles();
    void copyFolder();
    void symlinkCopy();
    void saveLoad();
    void cp();
    void moveFile();
    void moveFolder();
    void removeFile();
    void deployedFiles();
    void clear();
    void strip();

private:
    bool createFile(const QString& path, const QString& content) const;

    QTemporaryDir m_dir;
    QString m_source;
    QString m_target;
};

#endif // FILEMANAGERTTEST_H