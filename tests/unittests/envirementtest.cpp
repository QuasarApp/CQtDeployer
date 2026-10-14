//
// Copyright (C) 2026 QuasarApp.
// Distributed under the GPLv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#include "envirementtest.h"
#include "envirement.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSet>
#include <QTemporaryDir>
#include "deploycore.h"
#include "pathutils.h"


QString EnvirementTest::normalize(const QString& path) const {
    // Envirement stores the normalized path, see Envirement::addEnv.
    return PathUtils::fixPath(QFileInfo(path).absoluteFilePath());
}

void EnvirementTest::initTestCase() {
    if (!m_dir.isValid())
        qFatal("failed to create temporary dir");

    m_root = m_dir.path();
    if (!QDir(m_root).mkdir("libA"))
        qFatal("failed to create libA");
    if (!QDir(m_root).mkpath("libB/sub"))
        qFatal("failed to create libB/sub");
    if (!QDir(m_root).mkdir("libC"))
        qFatal("failed to create libC");

    QFile file(m_root + "/libA/libfoo.so");
    if (!file.open(QIODevice::WriteOnly))
        qFatal("failed to create libfoo.so");
    file.close();
}

void EnvirementTest::cleanupTestCase() {
}

void EnvirementTest::addAndRemoveEnv() {
    Envirement env;
    QCOMPARE(env.size(), 0);
    QVERIFY(env.environmentList().isEmpty());

    env.addEnv(m_root);
    QCOMPARE(env.size(), 1);
    QVERIFY(env.environmentList().contains(normalize(m_root)));

    // Adding the same environment twice does not change the size.
    env.addEnv(m_root);
    QCOMPARE(env.size(), 1);

    env.addEnv(m_root + "/libA");
    env.addEnv(m_root + "/libB/sub");
    QCOMPARE(env.size(), 3);

    // Non existing directories are not added.
    env.addEnv("/non/existent/environment");
    QCOMPARE(env.size(), 3);
}

void EnvirementTest::inThisEnvirement() {
    Envirement env;
    env.addEnv(m_root);

    // The environment root itself is an environment directory.
    QVERIFY(env.inThisEnvirement(m_root));

    // For files the parent directory is checked against the
    // environment set, so a file in a sub directory that is not
    // added is not part of the environment.
    QVERIFY(!env.inThisEnvirement(m_root + "/libA/libfoo.so"));

    env.addEnv(m_root + "/libA");
    // Now the file inside libA is part of the environment.
    QVERIFY(env.inThisEnvirement(m_root + "/libA/libfoo.so"));

    // Directories that are not in the set are not part of it.
    QVERIFY(!env.inThisEnvirement(m_root + "/libB"));

    // A non existing file inside an existing directory is not part
    // of the environment.
    QVERIFY(!env.inThisEnvirement(m_root + "/libC/missing.so"));

    // A path outside of the environment.
    QVERIFY(!env.inThisEnvirement("/some/other/place/file.txt"));
}

void EnvirementTest::concatEnv() {
    Envirement empty;
    QCOMPARE(empty.concatEnv(), QString());

    const auto separator = DeployCore::getEnvSeparator();

    Envirement env;
    env.addEnv(m_root);
    QCOMPARE(env.concatEnv(), normalize(m_root) + separator + normalize(m_root));

    Envirement multi;
    multi.addEnv(m_root);
    multi.addEnv(m_root + "/libA");

    // Implementation behavior: the first entry of the internal set is
    // listed twice. The iteration order of the set does not match the
    // insertion order, so compare the result as a multiset.
    const auto parts = multi.concatEnv().split(separator, Qt::SkipEmptyParts);
    QCOMPARE(parts.size(), 3);
    QCOMPARE(parts[0], parts[1]);
    QCOMPARE((QSet<QString>(parts.begin(), parts.end())),
             (QSet<QString>{normalize(m_root), normalize(m_root + "/libA")}));
}

void EnvirementTest::ignoreEnvList() {
    Envirement env;
    QVERIFY(!env.isIgnore(m_root));
    QVERIFY(!env.isIgnore(m_root + "/libB"));

    env.setIgnoreEnvList({m_root + "/libB"});
    QCOMPARE(env.ignoreEnvList().size(), 1);
    QVERIFY(env.isIgnore(m_root + "/libB"));
    QVERIFY(!env.isIgnore(m_root));

    // Environments from the ignore list are not added.
    env.addEnv(m_root + "/libB");
    QCOMPARE(env.size(), 0);
}

void EnvirementTest::setIgnoreEnvListRecursive() {
    Envirement env;
    env.setIgnoreEnvListRecursive({m_root + "/libB"}, -1);
    QCOMPARE(env.ignoreEnvList().size(), 2);
    QVERIFY(env.isIgnore(m_root + "/libB"));
    QVERIFY(env.isIgnore(m_root + "/libB/sub"));
}

void EnvirementTest::separator() {
    Envirement env;
    const auto separator = DeployCore::getEnvSeparator();
    env.addEnv(m_root + separator + m_root + "/libA");
    QCOMPARE(env.size(), 2);
    QVERIFY(env.environmentList().contains(normalize(m_root)));
    QVERIFY(env.environmentList().contains(normalize(m_root + "/libA")));
}

void EnvirementTest::addEnvRec() {
    Envirement env;
    env.addEnvRec(m_root, -1);
    // root, libA, libB, libB/sub and libC.
    QCOMPARE(env.size(), 5);
    QVERIFY(env.inThisEnvirement(m_root + "/libB/sub"));
}

void EnvirementTest::recursiveInvairement() {
    QDir dir(m_root);
    const auto res = Envirement::recursiveInvairement(dir, 0, -1);
    const auto set = QSet<QString>(res.begin(), res.end());

    const QSet<QString> expected = {
        normalize(m_root),
        normalize(m_root + "/libA"),
        normalize(m_root + "/libB"),
        normalize(m_root + "/libB/sub"),
        normalize(m_root + "/libC"),
    };
    QCOMPARE(set, expected);
    // The root is added last.
    QCOMPARE(res.last(), normalize(m_root));
}

void EnvirementTest::clear() {
    Envirement env;
    env.addEnv(m_root);
    env.setIgnoreEnvList({m_root + "/libB"});

    env.clear();
    QCOMPARE(env.size(), 0);
    QVERIFY(!env.isIgnore(m_root + "/libB"));
}
