//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#include "filemanagertest.h"
#include "filemanager.h"
#include "settings.h"

bool FileManagerTest::createFile(const QString &path, const QString &content) const {
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;
    f.write(content.toUtf8());
    return true;
}

void FileManagerTest::initTestCase() {
    QVERIFY(m_dir.isValid());

    m_source = m_dir.path() + "/source";
    m_target = m_dir.path() + "/target";
    QVERIFY(QDir(m_dir.path()).mkpath("source"));
    QVERIFY(QDir(m_dir.path()).mkpath("target"));

    // The Settings service is needed for save/load of the deployed files.
    QVERIFY(QuasarAppUtils::Settings::autoInstance() != nullptr);
}

void FileManagerTest::cleanupTestCase() {
}

void FileManagerTest::initDir() {
    FileManager fm;
    const auto deep = m_dir.path() + "/deep/a/b";

    QVERIFY(fm.initDir(deep));
    QVERIFY(QFileInfo(deep).isDir());

    // An existing dir is created (returned) as is.
    QVERIFY(fm.initDir(m_dir.path() + "/deep"));

    const auto deepNorm = QFileInfo(deep).absoluteFilePath();
    const auto baseNorm = QFileInfo(m_dir.path() + "/deep").absoluteFilePath();
    QVERIFY(fm.getDeployedFiles().contains(deepNorm));
    QVERIFY(fm.getDeployedFiles().contains(baseNorm));
}

void FileManagerTest::copyFile() {
    QVERIFY(createFile(m_source + "/file1.txt", "one"));

    FileManager fm;
    QVERIFY(fm.copyFile(m_source + "/file1.txt", m_target));
    QVERIFY(QFile::exists(m_target + "/file1.txt"));

    // Overwrite the destination file with the new content.
    QVERIFY(createFile(m_source + "/file1.txt", "one-again"));
    QVERIFY(fm.copyFile(m_source + "/file1.txt", m_target));

    // The file must be closed before the next copy: on Windows an
    // open handle makes the remove of the destination file inside
    // copyFile fail with a sharing violation.
    {
        QFile dst(m_target + "/file1.txt");
        QVERIFY(dst.open(QIODevice::ReadOnly));
        QCOMPARE(dst.readAll(), QByteArray("one-again"));
    }

    // A missing source file can not be copied.
    QVERIFY(!fm.copyFile(m_source + "/missing.txt", m_target));

    // A mask means that only the matching files are copied.
    QStringList mask = {"file1"};
    QVERIFY(fm.copyFile(m_source + "/file1.txt", m_target, &mask));
    QVERIFY(QFile::exists(m_target + "/file1.txt"));

    QVERIFY(createFile(m_source + "/file2.txt", "two"));
    QVERIFY(fm.copyFile(m_source + "/file2.txt", m_target, &mask));
    QVERIFY(!QFile::exists(m_target + "/file2.txt"));
}

void FileManagerTest::copyFiles() {
    QVERIFY(createFile(m_source + "/file1.txt", "one"));
    QVERIFY(createFile(m_source + "/file2.txt", "two"));

    FileManager fm;
    QStringList copied;
    QVERIFY(fm.copyFiles({m_source + "/file1.txt", m_source + "/file2.txt"},
                         m_target,
                         0,
                         {},
                         &copied));

    QVERIFY(QFile::exists(m_target + "/file1.txt"));
    QVERIFY(QFile::exists(m_target + "/file2.txt"));
    QCOMPARE(copied,
             (QStringList{m_target + "/file1.txt", m_target + "/file2.txt"}));

    // Files from the filter are ignored.
    const auto filtered = m_target + "/filtered";
    QVERIFY(fm.copyFiles({m_source + "/file1.txt", m_source + "/file2.txt"},
                         filtered,
                         0,
                         {"file1"}));

    QVERIFY(!QFile::exists(filtered + "/file1.txt"));
    QVERIFY(QFile::exists(filtered + "/file2.txt"));
}

void FileManagerTest::copyFolder() {
    QVERIFY(createFile(m_source + "/file1.txt", "one"));
    QVERIFY(QDir().mkpath(m_source + "/sub"));
    QVERIFY(createFile(m_source + "/sub/nested.txt", "nested"));

    FileManager fm;
    const auto dest = m_target + "/cp";
    QVERIFY(fm.copyFolder(m_source, dest));

    QVERIFY(QFile::exists(dest + "/file1.txt"));
    QVERIFY(QFile::exists(dest + "/sub/nested.txt"));
}

void FileManagerTest::cp() {
    QVERIFY(createFile(m_source + "/file1.txt", "one"));
    QVERIFY(QDir().mkpath(m_source + "/sub"));
    QVERIFY(createFile(m_source + "/sub/nested.txt", "nested"));

    FileManager fm;

    // A file is copied into the target dir.
    const auto fileDest = m_target + "/cpfile";
    QVERIFY(fm.cp(m_source + "/file1.txt", fileDest));
    QVERIFY(QFile::exists(fileDest + "/file1.txt"));

    // A folder is copied to <to>/<name of folder>.
    const auto dirDest = m_target + "/cpdir";
    QVERIFY(fm.cp(m_source, dirDest));
    QVERIFY(QFile::exists(dirDest + "/source/file1.txt"));
    QVERIFY(QFile::exists(dirDest + "/source/sub/nested.txt"));

    // A missing source can not be copied.
    QVERIFY(!fm.cp(m_source + "/missing", m_target));
}

void FileManagerTest::moveFile() {
    QVERIFY(createFile(m_source + "/file1.txt", "one"));

    FileManager fm;
    const auto dest = m_target + "/mv";
    QVERIFY(fm.moveFile(m_source + "/file1.txt", dest));

    QVERIFY(!QFile::exists(m_source + "/file1.txt"));
    QVERIFY(QFile::exists(dest + "/file1.txt"));

    // The moved file is added to the deployed files.
    const auto moved = QFileInfo(dest + "/file1.txt").absoluteFilePath();
    QVERIFY(fm.getDeployedFiles().contains(moved));
}

void FileManagerTest::moveFolder() {
    QVERIFY(QDir().mkpath(m_source + "/sub"));
    QVERIFY(createFile(m_source + "/sub/nested.txt", "nested"));

    FileManager fm;
    const auto dest = m_target + "/mf";
    QVERIFY(fm.moveFolder(m_source + "/sub", dest));

    QVERIFY(!QFile::exists(m_source + "/sub/nested.txt"));
    QVERIFY(QFile::exists(dest + "/nested.txt"));

    // Moving a dir into itself is refused.
    QVERIFY(!fm.moveFolder(m_source, m_source + "/sub"));
}

void FileManagerTest::removeFile() {
    const auto file = m_target + "/remove-me.txt";
    QVERIFY(createFile(file, "bye"));

    FileManager fm;
    QVERIFY(fm.addToDeployed(file));

    QVERIFY(fm.removeFile(file));
    QVERIFY(!QFile::exists(file));

    // removeFile() does not manage the deployed list, so the
    // entry is kept and has to be removed separately.
    const auto norm = QFileInfo(file).absoluteFilePath();
    QVERIFY(fm.getDeployedFiles().contains(norm));
    fm.removeFromDeployed(norm);
    QVERIFY(!fm.getDeployedFiles().contains(norm));

    // Removing a missing file is not an error.
    QVERIFY(fm.removeFile(file));
}

void FileManagerTest::deployedFiles() {
    const auto file = m_dir.path() + "/deployed.txt";
    QVERIFY(createFile(file, "x"));

    FileManager fm;
    QVERIFY(fm.addToDeployed(file));

    const auto norm = QFileInfo(file).absoluteFilePath();
    QVERIFY(fm.getDeployedFiles().contains(norm));
    QVERIFY(fm.getDeployedFilesStringList().contains(norm));

    fm.removeFromDeployed(norm);
    QVERIFY(!fm.getDeployedFiles().contains(norm));

    // A missing path is not added.
    QVERIFY(fm.addToDeployed(m_dir.path() + "/does-not-exist"));
    QVERIFY(fm.getDeployedFiles().isEmpty());
}

void FileManagerTest::clear() {
    FileManager fm;

    // A force clear removes the target dir completely.
    const auto forceDir = m_dir.path() + "/force";
    QVERIFY(QDir().mkpath(forceDir + "/nested"));
    QVERIFY(createFile(forceDir + "/nested/file.txt", "x"));
    fm.clear(forceDir, true);
    QVERIFY(!QFileInfo(forceDir).exists());

    // A non force clear removes only the deployed files.
    QVERIFY(createFile(m_target + "/deployed.txt", "d"));
    QVERIFY(createFile(m_target + "/undeployed.txt", "u"));
    fm.addToDeployed(m_target + "/deployed.txt");

    fm.clear(m_target, false);
    QVERIFY(!QFile::exists(m_target + "/deployed.txt"));
    QVERIFY(QFile::exists(m_target + "/undeployed.txt"));
    QVERIFY(fm.getDeployedFiles().isEmpty());
}

void FileManagerTest::strip() {
    FileManager fm;

    const auto dir = m_dir.path() + "/stripme";
    QVERIFY(QDir().mkpath(dir));
    QVERIFY(createFile(dir + "/plain.txt", "text"));

    // Files without a so/dll suffix are skipped and the
    // operation is reported as success.
    QVERIFY(fm.strip(dir));

#ifndef Q_OS_WIN
    // A missing dir is an error on non Windows platforms.
    QVERIFY(!fm.strip(m_dir.path() + "/does-not-exist"));
#else
    QVERIFY(fm.strip(m_dir.path() + "/does-not-exist"));
#endif
}

void FileManagerTest::saveLoad() {
    const auto file = m_dir.path() + "/saved.txt";
    QVERIFY(createFile(file, "x"));

    const auto cfgDir = m_dir.path() + "/cfg";
    QDir().mkpath(cfgDir);

    FileManager fm;
    fm.addToDeployed(file);
    fm.saveDeploymendFiles(cfgDir);

    FileManager other;
    other.loadDeployemendFiles(cfgDir);

    const auto norm = QFileInfo(file).absoluteFilePath();
    QVERIFY(other.getDeployedFiles().contains(norm));
}

// Standard stand-alone QTest entry point, see the Qt test tutorial:
// https://doc.qt.io/qt-6/qttestlib-tutorial1-example.html
QTEST_GUILESS_MAIN(FileManagerTest)
