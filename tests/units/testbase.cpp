#include "testbase.h"
#include <QProcessEnvironment>
#include "testutils.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QTest>

#ifdef Q_OS_WIN
#include <modules.h>
#endif

const QString TestBase::TestBinDir = TEST_BIN_DIR;
const QString TestBase::TestQtDir = QT_BASE_DIR;

TestBase::TestBase()
{

}

int TestBase::generateLib(const QString &paath) {
    QDir dir;
    dir.mkpath(QFileInfo(paath).absolutePath());
    QFile testLib (paath);

    int size = 0;

    if (testLib.open(QIODevice::ReadWrite| QIODevice::Truncate)) {
        QFile resData(":/debugLib");
        if (resData.open(QIODevice::ReadOnly)) {
            QByteArray tempData = resData.readAll();
            size = tempData.size();
            testLib.write(tempData.data(), tempData.size());
            resData.close();
        }

        testLib.close();
    }

    return size;
}

void TestBase::deleteLib(const QString &paath) {
    QFileInfo info(paath);
    if (info.isDir()) {
        QFile::remove(info.absoluteFilePath());
    } else {
        QDir qt(info.absoluteFilePath());
        qt.removeRecursively();
    }
}

void TestBase::cleanupTestCase() {

}

void TestBase::checkResults(const QSet<QString> &tree, bool noWarnings, bool onlySize) {
    TestUtils utils;

    QVERIFY(DeployCore::_config);
    QVERIFY(!DeployCore::_config->getTargetDir().isEmpty());

    auto resultTree = utils.getTree(DeployCore::_config->getTargetDir());

#ifdef Q_OS_WIN
    // Remove all API-MS-Win libs, because each OS Windows have a own bundle of this api libs.
    // See the https://github.com/QuasarApp/CQtDeployer/issues/481#issuecomment-755156875 post for more information.
    resultTree = TestModule.ignoreFilter(resultTree, "API-MS-Win");

#endif


    auto comapre = utils.compareTree(resultTree, tree);

    if (onlySize) {
        QVERIFY(resultTree.size() > tree.size());
        return;
    }

    auto &filesTree = *FilesTreeService::autoInstance();

    if (comapre.size() != 0) {

        bool bug = false;
        QJsonObject comapreResult;

        for (auto i = comapre.begin(); i != comapre.end(); ++i) {

            if (i.value() == 1) {
                comapreResult[ i.key()] = "Added unnecessary file";
                qCritical() << "added unnecessary file : " + i.key();
                bug = true;
            } else if (filesTree.contains(QFileInfo(i.key()).fileName())) {
                comapreResult[ i.key()] = "Missing";
                qCritical() << "Missing file : " + i.key();
                bug = true;
            } else if (noWarnings)  {
                comapreResult[ i.key()] = " not exits in qt Dir";

                qCritical() << "File : " + i.key() + " not exits in qt Dir";
                bug = true;
            } else {
                comapreResult[ i.key()] = " not exits in qt Dir";
                qWarning() << "File : " + i.key() + " not exits in qt Dir";
            }
        }

        if (!bug) {
            return;
        }

        QJsonObject obj;
        for (const auto &i : qAsConst(resultTree)) {
            obj[i];
        }

        QJsonDocument doc(obj);

        QFile lasttree("./LastTree.json");
        lasttree.open(QIODevice::WriteOnly| QIODevice::Truncate);

        lasttree.write(doc.toJson());
        lasttree.close();

        lasttree.setFileName("./CompareTree.json");
        lasttree.open(QIODevice::WriteOnly| QIODevice::Truncate);

        lasttree.write(QJsonDocument(comapreResult).toJson());
        lasttree.close();

        QVERIFY2(false, "runTestParams fail");


    }

}

void TestBase::costomScript() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QFile f("./" + DISTRO_DIR + "/bin/TestOnlyC");
    auto comapareTree = utils.createTree(
        {"./" + DISTRO_DIR + "/bin/TestOnlyC",
         "./" + DISTRO_DIR + "/bin/qt.conf",
         "./" + DISTRO_DIR + "/TestOnlyC.sh"});
    QString bin = TestBinDir + "TestOnlyC";
    QString scriptPath = "./" + DISTRO_DIR + "/TestOnlyC.sh";

#else
    QFile f("./" + DISTRO_DIR + "/TestOnlyC.exe");
    auto comapareTree = utils.createTree(
        {"./" + DISTRO_DIR + "/TestOnlyC.exe",
         "./" + DISTRO_DIR + "/TestOnlyC.bat",
         "./" + DISTRO_DIR + "/qt.conf"});
    QString bin = TestBinDir + "TestOnlyC.exe";
    QString scriptPath = "./" + DISTRO_DIR + "/TestOnlyC.bat";


#endif

    runTestParams({"-bin", bin, "force-clear", "noOverwrite", "-libOut", "lib"}, &comapareTree);

    QFile script(scriptPath);
    QVERIFY(script.open(QIODevice::ReadOnly));
    auto scriptText = script.readAll();

    QVERIFY(!scriptText.contains("Begin Custom Script"));

    script.close();

    runTestParams({"-bin", bin, "force-clear", "noOverwrite",
                   "-libOut", "lib",
                   "-customScript", "echo 'this is test script'"}, &comapareTree);

    QVERIFY(script.open(QIODevice::ReadOnly));

    scriptText = script.readAll();

    QVERIFY(scriptText.contains("Begin Custom Script"));
    QVERIFY(scriptText.contains("echo 'this is test script'"));
    QVERIFY(scriptText.contains("End Custom Script"));

    script.close();




}

void TestBase::runTestParams(QStringList list, QSet<QString> *tree, bool noWarnings, bool onlySize, exitCodes exitCode, const std::function<void (const DeployConfig *)> &cb) {

    QuasarAppUtils::Params::parseParams(list);

    Deploy deploy;
    int code = deploy.run();
    if (code != exitCode) {
        qDebug() << "Needed exit Code = " << exitCode;
        qDebug() << "Exit Code = " << code;

        QVERIFY(false && "exit code not valid");
    }

    if (tree) {
        checkResults(*tree, noWarnings, onlySize);
    }

    if (cb)
        cb(DeployCore::_config);
}
