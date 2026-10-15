//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#include "pathutilstest.h"
#include "pathutils.h"
#include <QFile>
#include <QTemporaryDir>
#include <utility>

void PathUtilsTest::initTestCase() {
}

void PathUtilsTest::cleanupTestCase() {
}

void PathUtilsTest::getName() {
    QVERIFY2(PathUtils::getName("") == "", "empty string");
    QVERIFY2(PathUtils::getName("test") == "test", "simple name");
    QVERIFY2(PathUtils::getName("t") == "t", "single letter");
    QVERIFY2(PathUtils::getName("/") == "/", "root");
    QVERIFY2(PathUtils::getName("/test") == "test", "simple path");
    QVERIFY2(PathUtils::getName("/t") == "t", "simple path single letter");
    QVERIFY2(PathUtils::getName("/test/") == "test", "path with trailing slash");
    QVERIFY2(PathUtils::getName("/t/") == "t", "path with trailing slash single letter");
    QVERIFY2(PathUtils::getName("/check/test") == "test", "nested path");
    QVERIFY2(PathUtils::getName("/check/t") == "t", "nested path single letter");
    QVERIFY2(PathUtils::getName("/check/test/") == "test", "nested path with trailing slash");
    QVERIFY2(PathUtils::getName("/check/t/") == "t", "nested path with trailing slash single letter");
    QVERIFY2(PathUtils::getName("C:\\") == "C:", "windows root");
    QVERIFY2(PathUtils::getName("\\") == "/", "unix style root");
    QVERIFY2(PathUtils::getName("\\test") == "test", "windows style path");
    QVERIFY2(PathUtils::getName("\\t") == "t", "windows style path single letter");
    QVERIFY2(PathUtils::getName("\\test\\") == "test", "windows style path with trailing slash");
    QVERIFY2(PathUtils::getName("\\t\\") == "t", "windows style path with trailing slash single letter");
}

// PathUtils::popItem() takes the path by non-const reference and removes
// the popped item from it, so a local copy is needed for every check.
static std::pair<QString, QString> popItemChecked(QString path) {
    return std::make_pair(PathUtils::popItem(path), path);
}

void PathUtilsTest::popItem() {
    QCOMPARE(popItemChecked(""), (qMakePair<QString, QString>("", "")));
    QCOMPARE(popItemChecked("test"), (qMakePair<QString, QString>("test", "")));
    QCOMPARE(popItemChecked("t"), (qMakePair<QString, QString>("t", "")));
    QCOMPARE(popItemChecked("/"), (qMakePair<QString, QString>("/", "")));
    QCOMPARE(popItemChecked("/test"), (qMakePair<QString, QString>("test", "/")));
    QCOMPARE(popItemChecked("/t"), (qMakePair<QString, QString>("t", "/")));
    QCOMPARE(popItemChecked("/test/"), (qMakePair<QString, QString>("test", "/")));
    QCOMPARE(popItemChecked("/t/"), (qMakePair<QString, QString>("t", "/")));
    QCOMPARE(popItemChecked("/check/test"), (qMakePair<QString, QString>("test", "/check/")));
    QCOMPARE(popItemChecked("/check/t"), (qMakePair<QString, QString>("t", "/check/")));
    QCOMPARE(popItemChecked("/check/test/"), (qMakePair<QString, QString>("test", "/check/")));
    QCOMPARE(popItemChecked("/check/t/"), (qMakePair<QString, QString>("t", "/check/")));
    QCOMPARE(popItemChecked("C:\\"), (qMakePair<QString, QString>("C:", "")));
    QCOMPARE(popItemChecked("\\"), (qMakePair<QString, QString>("/", "")));
    QCOMPARE(popItemChecked("\\test"), (qMakePair<QString, QString>("test", "/")));
    QCOMPARE(popItemChecked("\\t"), (qMakePair<QString, QString>("t", "/")));
    QCOMPARE(popItemChecked("\\test\\"), (qMakePair<QString, QString>("test", "/")));
    QCOMPARE(popItemChecked("\\t\\"), (qMakePair<QString, QString>("t", "/")));
}

void PathUtilsTest::toFullPath() {
    QVERIFY2(PathUtils::toFullPath("") == "/", "empty string");
    QVERIFY2(PathUtils::toFullPath("a") == "a/", "simple name");
    QVERIFY2(PathUtils::toFullPath("a/b") == "a/b/", "nested path");
    QVERIFY2(PathUtils::toFullPath("a//b") == "a/b/", "duplicated separators");
    QVERIFY2(PathUtils::toFullPath("a\\b") == "a/b/", "windows style separators");
    QVERIFY2(PathUtils::toFullPath("/a/b") == "/a/b/", "absolute path");
    QVERIFY2(PathUtils::toFullPath("/") == "/", "root");
    QVERIFY2(PathUtils::toFullPath("a///b//") == "a/b/", "many duplicated separators");
}

void PathUtilsTest::getReleativePath() {
    QVERIFY2(PathUtils::getReleativePath("a") == "../", "single segment");
    QVERIFY2(PathUtils::getReleativePath("a/b") == "../../", "two segments");
    QVERIFY2(PathUtils::getReleativePath("a/b/c") == "../../" "../", "three segments");
    QVERIFY2(PathUtils::getReleativePath("/a") == "/../", "absolute single segment");
    // Note: this is the actual behavior of the current implementation,
    // a ".." is inserted for every segment including the root one.
    QVERIFY2(PathUtils::getReleativePath("/a/b") == "/../../", "absolute two segments");
}

void PathUtilsTest::stripPath() {
    QVERIFY2(PathUtils::stripPath("") == "", "empty string");
    QVERIFY2(PathUtils::stripPath("a") == "a", "simple name");
    QVERIFY2(PathUtils::stripPath("a/b") == "a/b", "nested path");
    QVERIFY2(PathUtils::stripPath("a/b/") == "a/b", "nested path with trailing slash");
    QVERIFY2(PathUtils::stripPath("/a/b") == "/a/b", "absolute path");
    QVERIFY2(PathUtils::stripPath("C:\\") == "C:", "windows root");
    QVERIFY2(PathUtils::stripPath("\\") == "", "unix style root");
}

void PathUtilsTest::fullStripPath() {
    QVERIFY2(PathUtils::fullStripPath("") == "", "empty string");
    QVERIFY2(PathUtils::fullStripPath("a/b") == "a/b", "nested path");
    QVERIFY2(PathUtils::fullStripPath("/a/b") == "a/b", "absolute path");
    QVERIFY2(PathUtils::fullStripPath("/") == "", "root");
    QVERIFY2(PathUtils::fullStripPath("C://a") == "C:/a", "windows style path");
}

void PathUtilsTest::isReleativePath() {
    QVERIFY2(PathUtils::isReleativePath("."), "dot");
    QVERIFY2(PathUtils::isReleativePath("./a"), "dot slash");
    QVERIFY2(!PathUtils::isReleativePath("a"), "simple name");
    QVERIFY2(!PathUtils::isReleativePath(".a"), "dot without slash");
    QVERIFY2(!PathUtils::isReleativePath(""), "empty string");
    QVERIFY2(!PathUtils::isReleativePath("/a"), "absolute path");
    QVERIFY2(!PathUtils::isReleativePath(".\\a"), "dot with windows separator");
}

void PathUtilsTest::getDrive() {
    QVERIFY2(PathUtils::getDrive("C:/test") == 'C', "windows path");
    QVERIFY2(PathUtils::getDrive("c:\\test") == 'c', "windows path backslash");
    QVERIFY2(PathUtils::getDrive("C:") == 'C', "drive only");
    QVERIFY2(PathUtils::getDrive("/test").isNull(), "unix path");
    QVERIFY2(PathUtils::getDrive("test").isNull(), "simple name");
    QVERIFY2(PathUtils::getDrive("").isNull(), "empty string");
    QVERIFY2(PathUtils::getDrive("a:b") == 'a', "drive like name");
}

void PathUtilsTest::isAbsalutPath() {
    QVERIFY2(PathUtils::isAbsalutPath("/test"), "unix path");
    QVERIFY2(PathUtils::isAbsalutPath("\\test"), "windows path");
    QVERIFY2(PathUtils::isAbsalutPath("C:/test"), "windows path with drive");
    QVERIFY2(PathUtils::isAbsalutPath("C:\\"), "windows root");
    QVERIFY2(!PathUtils::isAbsalutPath("test"), "simple name");
    QVERIFY2(!PathUtils::isAbsalutPath(""), "empty string");
    QVERIFY2(!PathUtils::isAbsalutPath("./a"), "relative path");
}

void PathUtilsTest::fixPath() {
    QVERIFY2(PathUtils::fixPath("") == "", "empty string");
    QVERIFY2(PathUtils::fixPath("A//B/") == "A/B", "duplicated separators and trailing slash");
    QVERIFY2(PathUtils::fixPath("A/B") == "A/B", "nested path");
    QVERIFY2(PathUtils::fixPath("/A/B") == "/A/B", "absolute path");
}

void PathUtilsTest::isPath() {
    QTemporaryDir dir;
    QVERIFY2(dir.isValid(), "create temporary dir");

    QVERIFY2(PathUtils::isPath(dir.path()), "existing directory");
    QVERIFY2(!PathUtils::isPath("no/such/file/exists/here"), "non existing path");

    QFile file(dir.path() + "/test.txt");
    QVERIFY2(file.open(QIODevice::WriteOnly), "create test file");
    file.write("test");
    file.close();
    QVERIFY2(PathUtils::isPath(dir.path() + "/test.txt"), "existing file");
}

void PathUtilsTest::getRelativeLink() {
    struct TestCase {
        QString from;
        QString to;
        QString result;
    };

    // The expected values match the integration test
    // tests/integration/tstRelativelink.cpp.
    // Note: getRelativeLink("", "") is not tested, because
    // PathUtils::getReleativePath() dereferences an empty path.
    const auto testCases = QList<TestCase>{
            TestCase{"/media", "/etc", "./../etc/"},
            TestCase{"/media///", "/etc///", "./../etc/"},
            TestCase{"/media/etc/usr", "/media/etc", "./../"},
            TestCase{"/media/etc", "/media/etc/usr", "./usr/"},
            TestCase{"C:/", "C:/", "./"},
            TestCase{"C:\\", "C:/", "./"},
            TestCase{"C:/", "C:\\", "./"},
            TestCase{"C:/media", "C:/etc", "./../etc/"},
            TestCase{"C:/media//\\\\", "C:/etc///", "./../etc/"},
            TestCase{"C:/media/etc/usr", "C:/media/etc", "./../"},
            TestCase{"C:/media\\etc", "C:/media/etc/usr", "./usr/"},
            TestCase{"/", "/a/b", "./a/b/"},
    };

    for (const auto& test : testCases) {
        const auto actual = PathUtils::getRelativeLink(test.from, test.to);
        QVERIFY2(actual == test.result,
                 qPrintable("getRelativeLink(" + test.from + ", " + test.to + ") == "
                            + actual + ", expected " + test.result));
    }

    // A path with another drive letter is returned as-is.
    QCOMPARE(PathUtils::getRelativeLink("C:/media/etc", "D:/media/etc/usr"),
             QStringLiteral("D:/media/etc/usr"));
}


// Standard stand-alone QTest entry point, see the Qt test tutorial:
// https://doc.qt.io/qt-6/qttestlib-tutorial1-example.html
QTEST_GUILESS_MAIN(PathUtilsTest)
