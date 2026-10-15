//
// Copyright (C) 2026 QuasarApp.
// Distributed under the GPLv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#include "zipcompressertest.h"
#include "zipcompresser.h"

void ZipCompresserTest::initTestCase() {
    QVERIFY(m_dir.isValid());

    m_source = m_dir.path() + "/src";
    m_zipFile = m_dir.path() + "/archive.zip";

    QVERIFY(QDir().mkpath(m_source + "/sub"));
    QFile f(m_source + "/file1.txt");
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("one");
    f.close();

    f.setFileName(m_source + "/sub/nested.txt");
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("nested");
    f.close();
}

void ZipCompresserTest::cleanupTestCase() {
}

void ZipCompresserTest::compress() {
    ZipCompresser zip;
    QVERIFY(zip.compress(m_source, m_zipFile));

    QFileInfo fi(m_zipFile);
    QVERIFY(fi.exists());
    QVERIFY(fi.size() > 0);

    // A missing source dir can not be compressed.
    QVERIFY(!zip.compress(m_dir.path() + "/does-not-exist", m_zipFile));
}

void ZipCompresserTest::extract() {
    ZipCompresser zip;
    QVERIFY(zip.compress(m_source, m_zipFile));

    const auto out = m_dir.path() + "/out";
    QVERIFY(zip.extract(m_zipFile, out));

    // The archive contains the content of the source dir.
    QVERIFY(QFile::exists(out + "/file1.txt") ||
            QFile::exists(out + "/src/file1.txt"));

    // A missing archive can not be extracted.
    QVERIFY(!zip.extract(m_dir.path() + "/does-not-exist.zip", out));
}