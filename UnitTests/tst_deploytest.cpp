/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include <QtTest>
#include <quasarapp.h>
#include <deploycore.h>
#include <extracter.h>
#include <dependenciesscanner.h>
#include <qml.h>
#include <deploy.h>
#include <configparser.h>
#include <QCryptographicHash>
#include <distrostruct.h>
#include <pathutils.h>
#include <dependencymap.h>
#include <packing.h>
#include <pluginsparser.h>
#include <zipcompresser.h>
#include <QStorageInfo>

#include <Distributions/qif.h>

#include <QMap>
#include <QByteArray>
#include <QDir>
#include <thread>
#include "libcreator.h"
#include "modules.h"
#include "qmlcreator.h"
#include "testutils.h"

// add necessary includes here


static const QString TestBinDir = TEST_BIN_DIR;
static const QString TestQtDir = QT_BASE_DIR;

class deploytest : public QObject
{
    Q_OBJECT

private:
    QSet<QString> filesTree;

    void runTestParams(QStringList list,
                       QSet<QString> *tree = nullptr,
                       bool noWarnings = false,
                       bool onlySize = false,
                       exitCodes exitCode = exitCodes::Good,
                       const std::function<void (const DeployConfig *)> &cb = {});

    void checkResults(const QSet<QString> &tree,
                      bool noWarnings,
                      bool onlySize = false);

    void createTree(const QStringList& tree);

public:
    deploytest();
    /**
     * @brief generateLib
     * @param paath
     * @return size of lib
     */
    int generateLib(const QString& paath);
    void deleteLib(const QString& paath);

    ~deploytest();

private slots:
    void initTestCase();
    void cleanupTestCase();

    // old tests (not valid)
    void testDeployTarget();
    void testStrip();
    void testExtractLib();
    void testRelativeLink();
    void testCheckQt();

    void testQmlExtrct();
    void testSetTargetDir();

    //    void mainTests();

    // end old tests

    // tested flags customScript

    void testZip();
    void costomScript();
    void testDistroStruct();

    // tested flags clear noOvervrite
    void testOverwrite();
    void testOverwriteWithPacking();

    // tested flags binDir
    void testextraData();

    // tested flags qmlDir qmake
    void testQt();


    void testWebEngine();

    // tested flags confFile
    void testConfFile();

    // tested flags targetPackage
    void testPackages();

    // tested clear force clear in clear mode
    void testClear();

    // tested flags ignore
    void testIgnore();

    // tested flags ignore ignoreEnv
    void testIgnoreEnv();

    // tested flags libDir recursiveDepth
    void testLibDir();

    // tested flag extraPlugin
    void testExtraPlugins();

    // tested flag targetDir
    void testTargetDir();

    // tested flag noLibc deploySystem
    void testSystemLib();

    // tested flag qmlOut libOut trOut pluginOut binOut
    void testOutDirs();

    void testMSVC();

    // qif flags
    void testQIF();
    void testQIFMulti();
    void testQIFCustom();

    // zip flags
    void testZIP();
    void testZIPMulti();

    // deb flags
    void testDEB();
    void testDEBMulti();
    void testDEBCustom();

    // qif and zip flags
    void testMultiPacking();

    // init flags
    void testInit();

    void testDependencyMap();

    void testQmlScaner();

    void testPrefix();
    // test configure empty packages
    void testallowEmptyPackages();
    // test skip empty packages
    void testEmptyPackages();

    void testRunScripts();
    void testOverridingDefaultTemplateQIF();
    void testOverridingDefaultTemplateDEB();

    void testDeployGeneralFiles();
    void testTr();
    void testVirtualKeyBoard();
    // Attention! This test only covers 40% of icon functions
    void testIcons();
    void testPathUtils();
    void testBinPrefix();

    void testMd5();

    void testDisableShortcuts();
    void testDisableRunScripts();
    void testQifOut();
    void testIgnoreEnvWithLibDir();

    // note: this test checking in manual mode only.
    void testInstallDirsOptions();

    void testQIFResources();

    void testCustomPlatform();
    void testQifArchiveFormat();
    void testQifBinaryCreator();

    void customTest();
};

deploytest::deploytest() {

    qputenv("QTEST_FUNCTION_TIMEOUT", "1800000");
    QString qifwPath = qgetenv("PATH") + DeployCore::getEnvSeparator() + TestQtDir + "../../Tools/QtInstallerFramework/4.0/bin/";
    qifwPath += qifwPath + DeployCore::getEnvSeparator() + TestQtDir + "../../Tools/QtInstallerFramework/4.1/bin/";
    qifwPath += qifwPath + DeployCore::getEnvSeparator() + TestQtDir + "../../Tools/QtInstallerFramework/4.2/bin/";
    qifwPath += qifwPath + DeployCore::getEnvSeparator() + TestQtDir + "../../Tools/QtInstallerFramework/4.3/bin/";

    qputenv("PATH", qifwPath.toLatin1().data());
    TestUtils utils;

    QStringList pathList = QProcessEnvironment::systemEnvironment().
            value("PATH").split(DeployCore::getEnvSeparator());

    for (const auto& path: qAsConst(pathList)) {
        filesTree += utils.getFilesSet(path, 1);
    }

    filesTree += utils.getFilesSet(TestQtDir);

}

int deploytest::generateLib(const QString &paath)
{
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

void deploytest::deleteLib(const QString &paath)
{
    QFileInfo info(paath);
    if (info.isDir()) {
        QFile::remove(info.absoluteFilePath());
    } else {
        QDir qt(info.absoluteFilePath());
        qt.removeRecursively();
    }
}

deploytest::~deploytest(){}

void deploytest::initTestCase() {
    QDir qt;

    QDir("./" + DISTRO_DIR).removeRecursively();

    qt.mkpath("./test/Qt/5.12/");
    qt.mkpath("./test/extraPath/");
    qt.mkpath("./test/extra/");
    qt.mkpath("./test/warning/");
    qt.mkpath("./test/bins/");

    QFile f( "./test/Qt/5.12/generalLib.so");
    if (f.open(QIODevice::WriteOnly| QIODevice::Truncate)) {
        f.write("lib", 3);
        f.close();
    }

    f.setFileName("./test/extraPath/ExtraLib.so");
    if (f.open(QIODevice::WriteOnly| QIODevice::Truncate)) {
        f.write("lib", 3);
        f.close();
    }

    f.setFileName("./test/extra/ExtraLib.so");
    if (f.open(QIODevice::WriteOnly| QIODevice::Truncate)) {
        f.write("lib", 3);
        f.close();
    }

    f.setFileName("./test/warning/WarningLib.so");
    if (f.open(QIODevice::WriteOnly| QIODevice::Truncate)) {
        f.write("lib", 3);
        f.close();
    }

    f.setFileName("./test/bins/execTarget.exe");
    if (f.open(QIODevice::WriteOnly| QIODevice::Truncate)) {
        f.write("exec", 3);
        f.close();
    }

    f.setFileName("./test/bins/execTarget");
    if (f.open(QIODevice::WriteOnly| QIODevice::Truncate)) {
        f.write("exec", 3);
        f.close();
    }
}

void deploytest::cleanupTestCase() {
    QDir qt("./test");
    qt.removeRecursively();

}

void deploytest::testDeployTarget() {

    FileManager file;
    DependenciesScanner scan;
    Packing pac(&file);
    PluginsParser _pluginParser;

    ConfigParser *deploy = new ConfigParser(&file, &_pluginParser, &scan, &pac);

    QStringList targets;
    targets << "./test/bins/execTarget.exe";
    QVERIFY(deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new ConfigParser(&file, &_pluginParser, &scan, &pac);
    targets << "./test/bins/execTarget";
    QVERIFY(deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new ConfigParser(&file, &_pluginParser, &scan, &pac);
    targets << "./test/bins/execTarget.exe" << "./test/bins/execTarget";
    QVERIFY(deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new ConfigParser(&file, &_pluginParser, &scan, &pac);
    targets << "./test/bns/execTarget.exe";
    QVERIFY(!deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new ConfigParser(&file, &_pluginParser, &scan, &pac);
    targets << "./test/bins/";
    QVERIFY(deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new ConfigParser(&file, &_pluginParser, &scan, &pac);
    targets << "./test/bins/" << "./test/warning/";
    QVERIFY(deploy->setTargets(targets));

    delete deploy;
    targets.clear();
}

void deploytest::testStrip() {

#ifdef Q_OS_UNIX
    //for one lib
    qint64 sizeBefor = generateLib("./test/binTargetDir/debugLib.so");
    qint64 sizeAfter = 0;

    FileManager *deploy = new FileManager();
    QVERIFY(deploy->strip("./test/binTargetDir/debugLib.so"));

    QFile testLib ("./test/binTargetDir/debugLib.so");
    if (testLib.open(QIODevice::ReadOnly)) {
        sizeAfter = testLib.size();
        testLib.close();
    }

    deleteLib("./test/binTargetDir");
    delete deploy;

    QVERIFY(sizeBefor > sizeAfter);


    //for folder
    QStringList libList = {
        ("./test/binTargetDir/debugLib1.so"),
        ("./test/binTargetDir/debugLib2.so.1.2"),
        ("./test/binTargetDir/debugLib3.so.1"),
        ("./test/binTargetDir/debugLib4.so.1.0.0"),
        ("./test/binTargetDir/debugLib.dll"),
        ("./test/binTargetDir/debugLib1.dll")
    };
    QList<qint64> sizeBeforList = {};

    for (const auto & i: libList) {
        sizeBeforList.push_back(generateLib(i));
    }

    QList<qint64> sizeAfterList;

    deploy = new FileManager();
    QVERIFY(deploy->strip("./test/binTargetDir"));

    for(const auto &i: libList) {
        QFile testLib (i);
        if (testLib.open(QIODevice::ReadOnly)) {
            sizeAfterList.push_back(testLib.size());
            testLib.close();
        }
    }

    deleteLib("./test/binTargetDir");

    QVERIFY(sizeBeforList.size() == sizeAfterList.size());

    for (int i = 0; i < sizeAfterList.size(); ++i) {
        QVERIFY2(sizeBeforList[i] > sizeAfterList[i],
                 QString("index %0, lib: %1 size befor:%2, sizeAfter:%3").
                 arg(i).arg(libList[i]).arg(sizeBeforList[i]).arg(sizeAfterList[i]).
                 toLatin1());
    }

#endif
}

void deploytest::testExtractLib() {
    LibCreator creator("./");
    auto libs = creator.getLibs();
    auto deb = creator.getLibsDep();
    auto platforms = creator.getLibplatform();

    DependenciesScanner scaner;

    LibInfo info;

    for (const auto &lib : libs) {
        QVERIFY(scaner.fillLibInfo(info, lib));
        QVERIFY(info.getName() == QFileInfo(lib).fileName());
        QVERIFY(info.getPath() == QFileInfo(lib).absolutePath());
        QVERIFY(info.fullPath() == QFileInfo(lib).absoluteFilePath());
        QVERIFY(info.getPlatform() == platforms.value(lib));

        for (const auto &dep : deb.value(lib)) {
            QString depName = dep;
            if (info.getPlatform() & Platform::Win) {
                depName = dep.toUpper();
            }

            bool test = info.getDependncies().contains(depName);
            QVERIFY(test);
        }

    }

}

void deploytest::testMSVC() {
    QString testPath = "./Qt/5.11.2/msvc2017_64/bin/";

    QDir d;
    QDir oldDir("./Qt");
    oldDir.removeRecursively();
    QVERIFY(d.mkpath(testPath));


    auto msvc = DeployCore::getMSVC(testPath);

    QVERIFY(msvc & MSVCVersion::MSVC_17);
    QVERIFY(msvc & MSVCVersion::MSVC_x64);

    QDir dir("./Qt");
    dir.removeRecursively();


}

void deploytest::testWebEngine() {
#ifdef Q_OS_UNIX
    TestUtils utils;

    QString bin = TestBinDir + "quicknanobrowser";
    QString qmake = TestQtDir + "bin/qmake";

    auto comapareTree = TestModule.qtWebEngine();

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-qmlDir", TestBinDir + "/../quicknanobrowser"}, &comapareTree);


    bin = TestBinDir + "webui";
    qmake = TestQtDir + "bin/qmake";

    comapareTree = TestModule.qtWebEngineWidgets();

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake}, &comapareTree);


#endif
}

void deploytest::testQIF() {
    TestUtils utils;
#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";

    QString qmake = TestQtDir + "bin/qmake";
    auto comapareTree = utils.createTree({
                                             "./" + DISTRO_DIR + "/InstallerTestOnlyC.run",
                                             "./" + DISTRO_DIR + "/InstallerTestOnlyC.run.md5",
                                         });

#else
    QString bin = TestBinDir + "TestOnlyC.exe";

    QString qmake = TestQtDir + "bin/qmake.exe";
    auto comapareTree = utils.createTree({
                                             "./" + DISTRO_DIR + "/InstallerTestOnlyC.exe",
                                             "./" + DISTRO_DIR + "/InstallerTestOnlyC.exe.md5",

                                         });

#endif

    runTestParams({"-bin", bin, "clear" ,
                   "qif", "qifFromSystem",
                   "-qifStyle", "quasar",
                   "-qifBanner", TestBinDir + "/../../res/CQtDeployer_banner_web.png",
                   "-qifLogo", TestBinDir + "/../../res/CQtDeployer defaultIcon_web.png"}, &comapareTree, true);

}

void deploytest::testQIFMulti() {
    TestUtils utils;
#ifdef Q_OS_UNIX
    QString target1 = TestBinDir + "TestCPPOnly";
    QString target2 = TestBinDir + "TestOnlyC";

    auto comapareTreeMulti = utils.createTree({
                                                  "./" + DISTRO_DIR + "/InstallerApplication.run",
                                                  "./" + DISTRO_DIR + "/InstallerApplication.run.md5",

                                              });

#else
    QString target1 = TestBinDir + "TestCPPOnly.exe";
    QString target2 = TestBinDir + "TestOnlyC.exe";

    auto comapareTreeMulti = utils.createTree({
                                                  "./" + DISTRO_DIR + "/InstallerApplication.exe",
                                                  "./" + DISTRO_DIR + "/InstallerApplication.exe.md5",

                                              });

#endif
    QString bin = target1;
    bin += "," + target2;

    auto packageString = "/package1/;" + QFileInfo(target1).absoluteFilePath() + ",/package2/;" + QFileInfo(target2).absoluteFilePath();
    runTestParams({"-bin", bin, "force-clear",
                   "-targetPackage", packageString,
                   "qif", "qifFromSystem"}, &comapareTreeMulti, true);
}

void deploytest::testQIFCustom() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestCPPOnly" + "," + TestBinDir + "TestOnlyC";

    QString qmake = TestQtDir + "bin/qmake";

    auto comapareTreeCustom = utils.createTree({
                                                   "./" + DISTRO_DIR + "/Installerorg.qtproject.ifw.example.stylesheet.run",
                                                   "./" + DISTRO_DIR + "/Installerorg.qtproject.ifw.example.stylesheet.run.md5",

                                               });
#else
    QString bin = TestBinDir + "TestCPPOnly.exe" + "," + TestBinDir + "TestOnlyC.exe";

    QString qmake = TestQtDir + "bin/qmake.exe";
    auto comapareTreeCustom = utils.createTree({
                                                   "./" + DISTRO_DIR + "/Installerorg.qtproject.ifw.example.stylesheet.exe",
                                                   "./" + DISTRO_DIR + "/Installerorg.qtproject.ifw.example.stylesheet.exe.md5",
                                               });

#endif

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-qif", TestBinDir + "/../../UnitTests/testRes/QIFCustomTemplate",
                   "-name", "org.qtproject.ifw.example.stylesheet",
                   "qifFromSystem"}, &comapareTreeCustom, true);

}

void deploytest::testZIP() {

    TestUtils utils;

    auto comapareTree = utils.createTree({
                                             "./" + DISTRO_DIR + "/TestOnlyC.zip",
                                             "./" + DISTRO_DIR + "/TestOnlyC.zip.md5",
                                         });



#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";
#else
    QString bin = TestBinDir + "TestOnlyC.exe";
#endif

    runTestParams({"-bin", bin, "clear" ,
                   "zip", "verbose"}, &comapareTree, true);

    // test clear for zip
    runTestParams({"clear", "verbose"}, nullptr, true);

}

void deploytest::testZIPMulti() {
    TestUtils utils;

    auto comapareTreeMulti = utils.createTree({
                                                  "./" + DISTRO_DIR + "/package1.zip",
                                                  "./" + DISTRO_DIR + "/package2.zip",
                                                  "./" + DISTRO_DIR + "/package1.zip.md5",
                                                  "./" + DISTRO_DIR + "/package2.zip.md5",

                                              });

#ifdef Q_OS_UNIX
    QString target1 = TestBinDir + "TestCPPOnly";
    QString target2 = TestBinDir + "TestOnlyC";

#else
    QString target1 = TestBinDir + "TestCPPOnly.exe";
    QString target2 = TestBinDir + "TestOnlyC.exe";
#endif
    QString bin = target1;
    bin += "," + target2;

    auto packageString = "/package1/;" + QFileInfo(target1).absoluteFilePath() + ",/package2/;" + QFileInfo(target2).absoluteFilePath();
    runTestParams({"-bin", bin, "force-clear",
                   "-targetPackage", packageString,
                   "zip"}, &comapareTreeMulti, true);
}

void deploytest::testDEB() {

#ifdef Q_OS_UNIX
    TestUtils utils;

    auto comapareTree = utils.createTree({
                                             "./" + DISTRO_DIR + "/TestOnlyC.deb",
                                             "./" + DISTRO_DIR + "/TestOnlyC.deb.md5",

                                         });

    QString bin = TestBinDir + "TestOnlyC";

    runTestParams({"-bin", bin, "clear" ,
                   "deb", "verbose"}, &comapareTree, true);

    // test clear for deb
    runTestParams({"clear", "verbose"}, nullptr, true);

#endif

}

void deploytest::testDEBMulti() {
#ifdef Q_OS_UNIX
    TestUtils utils;

    auto comapareTreeMulti = utils.createTree({
                                                  "./" + DISTRO_DIR + "/package1.deb",
                                                  "./" + DISTRO_DIR + "/package2.deb",
                                                  "./" + DISTRO_DIR + "/package1.deb.md5",
                                                  "./" + DISTRO_DIR + "/package2.deb.md5",

                                              });

    QString target1 = TestBinDir + "TestCPPOnly";

    QString target2 = TestBinDir + "TestOnlyC";

    QString bin = target1;
    bin += "," + target2;

    auto packageString = "/package1/;" + QFileInfo(target1).absoluteFilePath() + ",/package2/;" + QFileInfo(target2).absoluteFilePath();
    runTestParams({"-bin", bin, "force-clear",
                   "-targetPackage", packageString,
                   "deb"}, &comapareTreeMulti, true);
#endif
}

void deploytest::testDEBCustom() {
#ifdef Q_OS_UNIX

    TestUtils utils;

    QString bin = TestBinDir + "TestOnlyC";

    auto comapareTreeCustom = utils.createTree({
                                                   "./" + DISTRO_DIR + "/chrome.deb",
                                                   "./" + DISTRO_DIR + "/chrome.deb.md5",

                                               });

    runTestParams({"-bin", bin, "clear" ,
                   "-deb", TestBinDir + "/../../UnitTests/testRes/DEBCustomTemplate",
                   "-name", "chrome"},
                  &comapareTreeCustom, true);
#endif
}

void deploytest::testMultiPacking() {
    TestUtils utils;

#ifdef Q_OS_UNIX

    auto comapareTree = utils.createTree({
                                             "./" + DISTRO_DIR + "/TestOnlyC.zip",
                                             "./" + DISTRO_DIR + "/TestOnlyC.deb",
                                             "./" + DISTRO_DIR + "/InstallerTestOnlyC.run",
                                             "./" + DISTRO_DIR + "/InstallerTestOnlyC.run.md5",
                                             "./" + DISTRO_DIR + "/TestOnlyC.zip.md5",
                                             "./" + DISTRO_DIR + "/TestOnlyC.deb.md5",

                                         });

    QString bin = TestBinDir + "TestOnlyC";

    runTestParams({"-bin", bin, "clear" ,
                   "zip",
                   "qif", "qifFromSystem",
                   "deb",
                   "verbose"}, &comapareTree, true);

#else
    auto comapareTree = utils.createTree({
                                             "./" + DISTRO_DIR + "/TestOnlyC.zip",
                                             "./" + DISTRO_DIR + "/InstallerTestOnlyC.exe",
                                             "./" + DISTRO_DIR + "/TestOnlyC.zip.md5",
                                             "./" + DISTRO_DIR + "/InstallerTestOnlyC.exe.md5",
                                         });
    QString bin = TestBinDir + "TestOnlyC.exe";

    runTestParams({"-bin", bin, "clear" ,
                   "zip",
                   "qif", "qifFromSystem",
                   "verbose"}, &comapareTree, true);

#endif
}

void deploytest::testInit()
{

    TestUtils utils;

    runTestParams({"init"});

    QVERIFY(QFile(DEFAULT_COFIGURATION_FILE).remove());

    runTestParams({"-init", "multi"});


    QVERIFY(QFile(DEFAULT_COFIGURATION_FILE).remove());

    runTestParams({"-init", "single"});

    QVERIFY(QFile(DEFAULT_COFIGURATION_FILE).remove());

}

void deploytest::testDependencyMap() {
    DependencyMap dep1, dep2, dep3;

    QVERIFY(dep1.qtModules() == 0);
    QVERIFY(dep2.qtModules() == 0);
    QVERIFY(dep3.qtModules() == 0);

    QVERIFY(dep1.systemLibs().isEmpty());
    QVERIFY(dep2.systemLibs().isEmpty());
    QVERIFY(dep3.systemLibs().isEmpty());


    QVERIFY(dep1.neadedLibs().isEmpty());
    QVERIFY(dep2.neadedLibs().isEmpty());
    QVERIFY(dep3.neadedLibs().isEmpty());

    dep1.addModule(DeployCore::QtModule::QtGuiModule);

    QVERIFY(dep1.qtModules() == DeployCore::QtModule::QtGuiModule);
    dep1.addModule(DeployCore::QtModule::QtHelpModule);

    QVERIFY(dep1.qtModules() == (DeployCore::QtModule::QtGuiModule |
                                 DeployCore::QtModule::QtHelpModule));

    dep1.removeModule(DeployCore::QtModule::QtGuiModule);

    QVERIFY(dep1.qtModules() == DeployCore::QtModule::QtHelpModule);

    dep2.addModule(DeployCore::QtModule::QtGuiModule);

    dep1 += dep2;

    QVERIFY(dep1.qtModules() == (DeployCore::QtModule::QtGuiModule |
                                 DeployCore::QtModule::QtHelpModule));



}

void deploytest::testQmlScaner() {

    // qt5
    auto qmlRoot = QFileInfo(TestQtDir + "/qml").absoluteFilePath();
    QML *scaner = new QML(qmlRoot, QtMajorVersion::Qt5);
    auto imports = scaner->extractImportsFromFile(":/qmlFile.qml");

    scaner->scanQmlTree(qmlRoot);

    QSet<QString> results = {
        {qmlRoot + "/QtQuick.2/"},
        {qmlRoot + "/QtQuick/Controls.2/"},
        {qmlRoot + "/QtQuick/Controls.2/Material/"},
        {qmlRoot + "/QtQuick/Layouts/"},
    };

    QVERIFY(results.size() == imports.size());

    for (const auto &import: qAsConst(imports)) {
        auto path = scaner->getPathFromImport(import);
        QVERIFY(results.contains(path));
    }


    scaner->setQtVersion(QtMajorVersion::Qt6);

    // qt6

    results = {
        {qmlRoot + "/QtQuick"},
        {qmlRoot + "/QtQuick/Controls"},
        {qmlRoot + "/QtQuick/Controls/Material"},
        {qmlRoot + "/QtQuick/Layouts"},
    };

    imports = scaner->extractImportsFromFile(":/qmlFileQt6.qml");

    QVERIFY(results.size() == imports.size());

    for (const auto &import: qAsConst(imports)) {
        auto path = scaner->getPathFromImport(import);
        QVERIFY(results.contains(path));
    }

    imports = scaner->extractImportsFromFile(":/qmlFile.qml");

    QVERIFY(results.size() == imports.size());

    for (const auto & import: qAsConst(imports)) {
        auto path = scaner->getPathFromImport(import);
        QVERIFY(results.contains(path));
    }

    // qt5
    scaner->setQtVersion(QtMajorVersion::Qt5);

    results = {
        {qmlRoot + "/QtQuick.2/"},
        {qmlRoot + "/QtQuick/Window.2/"},
        {qmlRoot + "/QtQuick/Layouts/"},
        {qmlRoot + "/Qt/labs/folderlistmodel/"},
        {qmlRoot + "/QtQuick/VirtualKeyboard/Settings/"},
        {qmlRoot + "/QtQuick/VirtualKeyboard/Styles/"},
    };

    imports = scaner->extractImportsFromQmlModule(":/qmlDir");

    QVERIFY(results.size() == imports.size());

    for (const auto &import: qAsConst(imports)) {
        auto path = scaner->getPathFromImport(import);
        QVERIFY(results.contains(path));
    }

    // qt6
    scaner->setQtVersion(QtMajorVersion::Qt6);

    results = {
        {qmlRoot + "/QtQuick"},
        {qmlRoot + "/QtQuick/Window"},
        {qmlRoot + "/QtQuick/Layouts"},
        {qmlRoot + "/Qt/labs/folderlistmodel"},
        {qmlRoot + "/QtQuick/VirtualKeyboard/Settings"},
        {qmlRoot + "/QtQuick/VirtualKeyboard/Styles"},
    };

    imports = scaner->extractImportsFromQmlModule(":/qmlDir");

    QVERIFY(results.size() == imports.size());

    for (const auto &import: qAsConst(imports)) {
        auto path = scaner->getPathFromImport(import);
        QVERIFY(results.contains(path));
    }

}

void deploytest::testPrefix() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QFile f("./" + DISTRO_DIR + "/bin/TestOnlyC");
    auto comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/package/TestOnlyC.sh",
                    "./" + DISTRO_DIR + "/package/bin/TestOnlyC",
                    "./" + DISTRO_DIR + "/package/bin/qt.conf"
                });
    QString target1 = TestBinDir + "TestOnlyC";

#else
    QFile f("./" + DISTRO_DIR + "/TestOnlyC.exe");
    auto comapareTree = utils.createTree(
                {"./" + DISTRO_DIR + "/package/TestOnlyC.exe",
                 "./" + DISTRO_DIR + "/package/TestOnlyC.bat",
                 "./" + DISTRO_DIR + "/package/qt.conf"});
    QString target1 = TestBinDir + "TestOnlyC.exe";

#endif
    QString bin = target1;

    comapareTree = TestModule.replace(comapareTree, {{"package","prefix"}});

    runTestParams({"-bin", bin, "force-clear",
                   "-targetPackage", "/package/;TestOn",
                   "-prefix", "package;prefix"}, &comapareTree);
}

void deploytest::testallowEmptyPackages() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";
#else
    QString bin = TestBinDir + "TestOnlyC.exe";
#endif
    runTestParams({"-bin", bin, "force-clear",
                   "-prefix", "package;prefix"}, nullptr, false, false,
                  exitCodes::PrepareError);

    runTestParams({"-bin", bin, "force-clear",
                   "-prefix", "package;prefix",
                   "allowEmptyPackages"});
}

void deploytest::testEmptyPackages() {
    TestUtils utils;
    auto comapareTree = TestModule.onlyC();

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";
#else
    QString bin = TestBinDir + "TestOnlyC.exe";
#endif

    runTestParams({"-bin", bin, "force-clear",
                   "-prefix", "package;prefix"}, nullptr, false, false,
                  exitCodes::PrepareError);

    runTestParams({"-bin", bin, "force-clear",
                   "-targetPackage", "/package/;NONE",
                   "-libDir", TestQtDir + "bin",
                   "-prefix", "package;prefix",
                   "allowEmptyPackages"}, &comapareTree);
}

void deploytest::testRunScripts() {
    TestUtils utils;
    auto comapareTree = TestModule.onlyC();

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";

    QFile f(":/testResurces/testRes/customRunScript.sh");
    QVERIFY(f.open(QIODevice::ReadOnly));
    auto etalonData = f.readAll();
    f.close();

    runTestParams({"-bin", bin,
                   "force-clear",
                   "-libOut", "lib",
                   "-runScript", "TestOnlyC;:/testResurces/testRes/customRunScript.sh"}, &comapareTree);

    f.setFileName(DISTRO_DIR + "/TestOnlyC.sh");
    QVERIFY(f.open(QIODevice::ReadOnly));
    auto deployData = f.readAll();

    QVERIFY(deployData == etalonData);
#else
    QString bin = TestBinDir + "TestOnlyC.exe";

    QFile f(":/testResurces/testRes/customRunScript.sh");
    QVERIFY(f.open(QIODevice::ReadOnly));
    auto etalonData = f.readAll();
    f.close();

    runTestParams({"-bin", bin,
                   "force-clear",
                   "-libOut", "lib",
                   "-runScript", "TestOnlyC.exe;:/testResurces/testRes/customRunScript.sh"}, nullptr);

    f.setFileName(DISTRO_DIR + "/TestOnlyC.bat");
    QVERIFY(f.open(QIODevice::ReadOnly));
    auto deployData = f.readAll();

    QVERIFY(deployData == etalonData);
#endif

}

void deploytest::testOverridingDefaultTemplateDEB()
{
    TestUtils utils;


#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC" + "," + TestBinDir + "TestCPPOnly";
    QString qmake = TestQtDir + "bin/qmake";

    QDir tempalteDir("temaplate");
    if (!tempalteDir.isEmpty()) {
        tempalteDir.removeRecursively();
    };

    auto comapareTree = utils.createTree(
                {
                    "temaplate/defaultDEBTemplate/Test/DEBIAN/control",
                    "temaplate/defaultDEBTemplate/Test/DEBIAN/postinst",
                    "temaplate/defaultDEBTemplate/Test/DEBIAN/prerm"
                });
    QFile appScript("temaplate/defaultDEBTemplate/Test/DEBIAN/control");
    appScript.remove();

    runTestParams(
                {"force-clear",
                 "getDefaultTemplate",
                 "-name", "Test",
                 "deb",
                 "-targetDir", "temaplate",
                 "-targetPackage", "MyApp"
                }, &comapareTree);

    comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/Test.deb",
                    "./" + DISTRO_DIR + "/Test.deb.md5",

                });

    runTestParams(
                {"force-clear",
                 "-deb", "temaplate/defaultDEBTemplate",
                 "-targetPackage", "MyApp",
                 "-name", "Test",
                 "-qmake", qmake
                }, &comapareTree, false, false);


    QVERIFY(appScript.open(QIODevice::WriteOnly));
    QVERIFY(appScript.write(QByteArray{"ERROR"}));
    appScript.close();

    runTestParams(
                {"-bin", bin,
                 "force-clear",
                 "-deb", "temaplate/defaultDEBTemplate",
                 "-targetPackage", "MyApp",
                 "-name", "Test",
                 "-qmake", qmake
                }, nullptr, false, false, exitCodes::PackingError);
#endif

}

void deploytest::testOverridingDefaultTemplateQIF() {
    TestUtils utils;
    // Prepare bin and qmake values
#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC" + "," + TestBinDir + "TestCPPOnly";
    QString qmake = TestQtDir + "bin/qmake";

#else
    QString bin = TestBinDir + "TestOnlyC.exe" + "," + TestBinDir + "TestCPPOnly.exe";
    QString qmake = TestQtDir + "bin/qmake.exe";

#endif
    QDir tempalteDir("temaplate");
    if (!tempalteDir.isEmpty()) {
        tempalteDir.removeRecursively();
    };

    // QIF case. prepare default template.
    auto comapareTree = utils.createTree(
                {
                    "temaplate/defaultQIFWTemplate/config/config.xml",
                    "temaplate/defaultQIFWTemplate/config/controlScript.qs",
                    "temaplate/defaultQIFWTemplate/packages/MyApp/meta/installscript.qs",
                    "temaplate/defaultQIFWTemplate/packages/MyApp/meta/package.xml"
                });
    QFile appScript("temaplate/defaultQIFWTemplate/packages/MyApp/meta/installscript.qs");
    appScript.remove();

    // This command shold be deploy default template in the template folder
    runTestParams(
                {"-bin", bin,
                 "force-clear",
                 "getDefaultTemplate",
                 "-name", "Test",
                 "qif",
                 "-targetDir", "temaplate",
                 "-targetPackage", "MyApp"
                }, &comapareTree);

#ifdef Q_OS_UNIX
    comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/InstallerTest.run",
                    "./" + DISTRO_DIR + "/InstallerTest.run.md5",
                });

#else
    comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/InstallerTest.exe",
                    "./" + DISTRO_DIR + "/InstallerTest.exe.md5",
                });

#endif

    // check deploy application with custom template
    runTestParams(
                {"-bin", bin,
                 "force-clear",
                 "-qif", "temaplate/defaultQIFWTemplate",
                 "-targetPackage", "MyApp",
                 "-name", "Test",
                 "-qmake", qmake,
                 "qifFromSystem"
                }, &comapareTree, false, false);

    QVERIFY(appScript.open(QIODevice::WriteOnly));
    QVERIFY(appScript.write(QByteArray{"ERROR"}));
    appScript.close();

    // Shold be failde because we added error string into template files.
    runTestParams(
                {"-bin", bin,
                 "force-clear",
                 "-qif", "temaplate/defaultQIFWTemplate",
                 "-targetPackage", "MyApp",
                 "-name", "Test",
                 "-qmake", qmake,
                 "qifFromSystem"
                }, nullptr, false, false, exitCodes::PackingError);



}

void deploytest::testDeployGeneralFiles() {
    TestUtils utils;

    QString bin = TestBinDir + "/../../CMakeLists.txt";

    auto comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/bin/CMakeLists.txt",
                    "./" + DISTRO_DIR + "/bin/qt.conf",

                });

    runTestParams(
                {"-bin", bin,
                 "-binOut", "bin",
                 "force-clear"
                }, &comapareTree);
}

void deploytest::testTr() {
    TestUtils utils;
#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";

#else
    QString bin = TestBinDir + "TestOnlyC.exe";

#endif

    runTestParams({"-bin", bin, "clear" ,
                   "-tr", ":/testResurces/testRes/TestTr.qm",});

    QVERIFY(QFile::exists("./" + DISTRO_DIR + "/translations/TestTr.qm"));

    runTestParams({"-bin", bin, "clear" ,
                   "-tr", ":/testResurces/testRes/"});

    QVERIFY(QFile::exists("./" + DISTRO_DIR + "/translations/TestTr.qm"));
}

void deploytest::testVirtualKeyBoard() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "basic";
    QString qmake = TestQtDir + "bin/qmake";
#else
    QString bin = TestBinDir + "basic.exe";
    QString qmake = TestQtDir + "bin/qmake.exe";
#endif
    auto comapareTree = TestModule.qmlVirtualKeyBoadrLibs();

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-qmlDir", TestBinDir + "/../virtualkeyboard"}, &comapareTree);
}

void deploytest::testIcons() {
    TestUtils utils;

    auto initTargets = [](ConfigParser * deploy, const QStringList& params) {
        QVERIFY(QuasarAppUtils::Params::parseParams(params));

        auto bin = QuasarAppUtils::Params::getArg("bin").
                split(DeployCore::getSeparator(0), splitbehavior);


        QVERIFY(deploy->setTargets(bin));

        deploy->configureTargets();
    };

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";
    QStringList binMulti = {TestBinDir + "TestOnlyC" , TestBinDir + "TestCPPOnly"};

#else
    QString bin = TestBinDir + "TestOnlyC.exe";
    QStringList binMulti = {TestBinDir + "TestOnlyC.exe" , TestBinDir + "TestCPPOnly.exe"};

#endif

    FileManager file;
    DependenciesScanner scan;
    Packing pac(&file);
    PluginsParser _pluginParser;


    // Test default icons
    ConfigParser *deploy = new ConfigParser(&file, &_pluginParser, &scan, &pac);

    initTargets(deploy, {"-bin", bin,
                         "force-clear",
                });

    auto targetsMap = deploy->_config.getTargetsListByFilter("TestOnlyC");
    auto targetinfo = targetsMap.begin().value();
    QVERIFY(targetinfo->getName().contains("TestOnlyC"));
    auto icon = QFileInfo(targetinfo->getIcon());

#ifdef Q_OS_UNIX
    QVERIFY(icon.fileName() == "Icon.png");
#else
    QVERIFY(icon.fileName() == "Icon.ico");

#endif
    delete deploy;

    // Test multiple targets icons (sets for all targets on icon)
    deploy = new ConfigParser(&file, &_pluginParser, &scan, &pac);

    initTargets(deploy, {"-bin", binMulti.join(","),
                         "force-clear",
                         "-icon", ":/testResurces/testRes/TestIcon.png",
                });

    for (const auto &target : qAsConst(binMulti)) {
        QFileInfo tarInfo(target);

        targetsMap = deploy->_config.getTargetsListByFilter(tarInfo.baseName());
        targetinfo = targetsMap.begin().value();
        QVERIFY(targetinfo->getName().contains(tarInfo.baseName()));
        icon = QFileInfo(targetinfo->getIcon());

        QVERIFY(icon.fileName() == "TestIcon.png");
    }

    delete deploy;


    // Test multiple targets icons (sets only one icon)
    deploy = new ConfigParser(&file, &_pluginParser, &scan, &pac);

    initTargets(deploy, {"-bin", binMulti.join(","),
                         "force-clear",
                         "-icon", "TestOnlyC;:/testResurces/testRes/TestIcon.png",
                });


    targetsMap = deploy->_config.getTargetsListByFilter("TestOnlyC");
    targetinfo = targetsMap.begin().value();
    QVERIFY(targetinfo->getName().contains("TestOnlyC"));
    icon = QFileInfo(targetinfo->getIcon());

    QVERIFY(icon.fileName() == "TestIcon.png");


    delete deploy;

    // manual tests
#ifdef QT_DEBUG
#ifdef Q_OS_UNIX
    bin = TestBinDir + "TestOnlyC," + TestBinDir + "TestCPPOnly";

    auto comapareTree = utils.createTree({
                                             "./" + DISTRO_DIR + "/InstallerTest.run",
                                             "./" + DISTRO_DIR + "/InstallerTest.run.md5",
                                         });

#else
    bin = TestBinDir + "TestOnlyC.exe,"  + TestBinDir + "TestCPPOnly.exe";

    auto comapareTree = utils.createTree({
                                             "./" + DISTRO_DIR + "/InstallerTest.exe",
                                             "./" + DISTRO_DIR + "/InstallerTest.exe.md5",

                                         });

#endif

    runTestParams({"-bin", bin, "clear" ,
                   "qif", "qifFromSystem", "-name", "Test",
                   "-targetPackage", "testPackage;TestOnlyC;TestCPPOnly",
                   "-icon", "TestOnlyC;:/testResurces/testRes/TestIcon.png,TestCPPOnly;:/testResurces/testRes/TestTr.qm"}, &comapareTree, true);

#endif
}

void deploytest::testPathUtils() {

    // test getName
    QMap<QString, QString> cases = {
        {"",""},
        {"test","test"},
        {"t","t"},
        {"/","/"},
        {"/test","test"},
        {"/t","t"},
        {"/test/","test"},
        {"/t/","t"},

        {"/check/test","test"},
        {"/check/t","t"},
        {"/check/test/","test"},
        {"/check/t/","t"},

        {"C:\\","C:"},
        {"\\","/"},
        {"\\test","test"},
        {"\\t","t"},
        {"\\test\\","test"},
        {"\\t\\","t"},

    };

    for (auto it = cases.begin(); it != cases.end(); ++it) {
        if (PathUtils::getName(it.key()) != it.value())
            QVERIFY(false);
    }

    struct Result {
        QString result;
        QString newPath;
    };

    // test popItem
    QMap<QString, Result> popItemCases = {
        {"", {"", ""}},
        {"test", {"test", ""}},
        {"t", {"t", ""}},
        {"/", {"/", ""}},
        {"/test", {"test", "/"}},
        {"/t", {"t", "/"}},
        {"/test/", {"test", "/"}},
        {"/t/", {"t", "/"} },

        {"/check/test", {"test", "/check/"}},
        {"/check/t", {"t", "/check/"}},
        {"/check/test/", {"test", "/check/"}},
        {"/check/t/", {"t", "/check/"}},

        {"C:\\", {"C:", ""}},
        {"\\", {"/", ""}},
        {"\\test", {"test", "/"}},
        {"\\t", {"t", "/"}},
        {"\\test\\", {"test", "/"}},
        {"\\t\\", {"t", "/"}},

    };

    for (auto it = popItemCases.begin(); it != popItemCases.end(); ++it) {
        QString path = it.key();
        QString result = PathUtils::popItem(path);
        if (path != it.value().newPath || result != it.value().result)
            QVERIFY(false);
    }
}

void deploytest::testBinPrefix() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    auto comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/TestOnlyC.sh",
                    "./" + DISTRO_DIR + "/bin/TestOnlyC",
                    "./" + DISTRO_DIR + "/bin/qt.conf"
                });
    QString target = TestBinDir + "TestOnlyC";
    QString targetWithoutPrefix = "TestOnlyC";

#else
    auto comapareTree = utils.createTree(
                {"./" + DISTRO_DIR + "/TestOnlyC.exe",
                 "./" + DISTRO_DIR + "/TestOnlyC.bat",
                 "./" + DISTRO_DIR + "/qt.conf"});
    QString target = TestBinDir + "TestOnlyC.exe";
    QString targetWithoutPrefix = "TestOnlyC.exe";

#endif

    runTestParams({"-bin", target, "force-clear"}, &comapareTree);

    runTestParams({"-bin", targetWithoutPrefix,
                   "-binPrefix", TestBinDir,
                   "force-clear"}, &comapareTree);

}

void deploytest::testMd5() {
    // This test will check hashes

    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";
    auto outFile = "./" + DISTRO_DIR + "/InstallerTestOnlyC.run";

#else
    QString bin = TestBinDir + "TestOnlyC.exe";
    auto outFile = "./" + DISTRO_DIR + "/InstallerTestOnlyC.exe";

#endif

    auto comapareTreeqif = utils.createTree(
                {
                    outFile,
                    outFile + ".md5"
                });

    // Run deploy installer
    runTestParams({"-bin", bin,
                   "force-clear",
                   "qif",
                   "qifFromSystem"}, &comapareTreeqif);

    QFile file(outFile);
    QVERIFY(file.open(QIODevice::ReadOnly));
    auto binaryData = file.readAll();
    file.close();

    file.setFileName(outFile + ".md5");
    QVERIFY(file.open(QIODevice::ReadOnly));
    auto hash = file.readAll();
    file.close();

    // Compare hash sum of the installer with realy hash sum of the object.
    QVERIFY(QCryptographicHash::hash(binaryData, QCryptographicHash::Md5).toHex() == hash);


    comapareTreeqif = utils.createTree(
                {
                    outFile,
                });

    // Check noHashSum option. CQtDeployer must be skip calculating  a hash sum of an installer.
    runTestParams({"-bin", bin,
                   "force-clear",
                   "noHashSum",
                   "qif",
                   "qifFromSystem"}, &comapareTreeqif);
}

void deploytest::testDisableShortcuts() {

    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";
#else
    QString bin = TestBinDir + "TestOnlyC.exe";
#endif

    auto cb = [](const DeployConfig * config){
        const auto list = config->getTargetsListByFilter("TestOnlyC");
        // Check shrtcut option. all targets should be return false.
        for (auto target: list) {
            QVERIFY(!target->getShortCut());
        }
    };

    // Run deploy installer
    runTestParams({"-bin", bin, "clear",
                   "-disableShortCut", "TestOnlyC",
                   "qif", "qifFromSystem"},
                  nullptr,
                  false,
                  false,
                  exitCodes::Good,
                  cb);



}

void deploytest::testDisableRunScripts() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";
#else
    QString bin = TestBinDir + "TestOnlyC.exe";
#endif

    auto comapareTreeqif = TestModule.onlyC();
    comapareTreeqif -= utils.createTree({DISTRO_DIR + "/TestOnlyC.sh",
                                         DISTRO_DIR + "/TestOnlyC.bat"});
    // Run deploy installer
    runTestParams({"-bin", bin, "clear", "-libDir", TestQtDir + "/bin",
                   "-disableRunScript", "TestOnlyC"}, &comapareTreeqif);
}

void deploytest::testQifOut() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";
#else
    QString bin = TestBinDir + "TestOnlyC.exe";
#endif

#ifdef Q_OS_UNIX
    auto result = utils.createTree({{DISTRO_DIR + "/QIF_OUT.exe"}, {DISTRO_DIR + "/QIF_OUT.exe.md5"},
                                    {DISTRO_DIR + "/DEB_OUT.deb"}, {DISTRO_DIR + "/DEB_OUT.deb.md5"},
                                    {DISTRO_DIR + "/ZIP_OUT.zip"}, {DISTRO_DIR + "/ZIP_OUT.zip.md5"}});

    // Run deploy installer
    runTestParams({"-bin", bin, "clear",
                   "qif", "-qifOut", "QIF_OUT.exe",
                   "deb", "-debOut", "DEB_OUT.deb",
                   "zip", "-zipOut", "ZIP_OUT.zip"}, &result);
#else
    auto result = utils.createTree({{DISTRO_DIR + "/QIF_OUT.exe"}, {DISTRO_DIR + "/QIF_OUT.exe.md5"},
                                    {DISTRO_DIR + "/ZIP_OUT.zip"}, {DISTRO_DIR + "/ZIP_OUT.zip.md5"}});

    // Run deploy installer
    runTestParams({"-bin", bin, "clear",
                   "qif", "-qifOut", "QIF_OUT.exe",
                   "zip", "-zipOut", "ZIP_OUT.zip"}, &result);
#endif
}

void deploytest::testIgnoreEnvWithLibDir() {
#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";
#else
    QString bin = TestBinDir + "TestOnlyC.exe";
#endif

    QVERIFY(QDir().mkdir("libDirtest"));

    // Run deploy installer
    runTestParams({"-bin", bin, "clear",
                   "-targetDir", "./libDirtest",
                   "-libDir", "./libDirtest"}, nullptr, false, false,
                  exitCodes::PrepareError);

    QVERIFY(QDir().rmdir("libDirtest"));
}

void deploytest::testInstallDirsOptions() {
#ifdef QT_DEBUG
#ifdef Q_OS_UNIX
    QStringList binMulti = {TestBinDir + "TestOnlyC" , TestBinDir + "TestCPPOnly"};

#else
    QStringList binMulti = {TestBinDir + "TestOnlyC.exe" , TestBinDir + "TestCPPOnly.exe"};

#endif


    runTestParams({"-bin", binMulti.join(","), "clear",
                   "qif", "deb",
                   "-targetPackage", "pkg;TestCPPOnly",
                   "-installDirDeb", "pkg;/var",
                   "-installDirQIFW", "/opt"});

#endif
}

void deploytest::testQIFResources() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = {TestBinDir + "TestOnlyC"};

    auto result = utils.createTree({{DISTRO_DIR + "/InstallerTestOnlyC.run"},
                                    {DISTRO_DIR + "/InstallerTestOnlyC.run.md5"}});
#else
    QString bin = {TestBinDir + "TestOnlyC.exe"};

    auto result = utils.createTree({{DISTRO_DIR + "/InstallerTestOnlyC.exe"},
                                    {DISTRO_DIR + "/InstallerTestOnlyC.exe.md5"}});
#endif


    auto templateDir = TestBinDir + "/../../UnitTests/testRes/QIFCustomTemplate";
    runTestParams({
                      "-bin", bin,
                      "clear",
                      "qif",
                      "-qifConfig", templateDir + "/customconfig.xml",
                      "-qifPackages", templateDir + "/custompackages",
                      "-qifResources", templateDir + "customRes.qrc"
                  }, &result
                  );

}

void deploytest::testCustomPlatform() {
    TestUtils utils;

    auto compareTree = TestModule.onlyC();

#ifdef Q_OS_UNIX
    QString bin = {TestBinDir + "TestOnlyC"};
    QString platform = "linux_x86_64";

#else
    QString bin = {TestBinDir + "TestOnlyC.exe"};
    QString platform = "win_x86_64";

    compareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/TestOnlyC.exe",
                    "./" + DISTRO_DIR + "/TestOnlyC.bat",
                    "./" + DISTRO_DIR + "/qt.conf"
                }
                );

#endif

    runTestParams({
                      "-bin", bin,
                      "clear",
                      "-platform", platform,
                  }, &compareTree
                  );



    runTestParams({
                      "-bin", bin,
                      "clear",
                      "-platform", "GeneralFile",
                  }, nullptr, false, false, exitCodes::PrepareError
                  );
}

void deploytest::testQifArchiveFormat() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = {TestBinDir + "TestOnlyC"};

    auto result = utils.createTree({{DISTRO_DIR + "/InstallerTestOnlyC.run"},
                                    {DISTRO_DIR + "/InstallerTestOnlyC.run.md5"}});
#else
    QString bin = {TestBinDir + "TestOnlyC.exe"};

    auto result = utils.createTree({{DISTRO_DIR + "/InstallerTestOnlyC.exe"},
                                    {DISTRO_DIR + "/InstallerTestOnlyC.exe.md5"}});
#endif


    runTestParams({
                      "-bin", bin,
                      "qifFromSystem",
                      "clear",
                      "qif",
                      "-qifArchiveFormat", "zip"
                  }, &result
                  );
}

void deploytest::testQifBinaryCreator() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = {TestBinDir + "TestOnlyC"};
#else
    QString bin = {TestBinDir + "TestOnlyC.exe"};
#endif

    QuasarAppUtils::Params::parseParams({
                                            "-bin", bin,
                                            "qifFromSystem",
                                            "clear",
                                            "qif",
                                            "-binarycreator", "test testValue"
                                        });
    Deploy deploy;
    QVERIFY(deploy.prepare());
    FileManager fm;
    QIF qif(&fm);

    auto command = qif.runCmd();
    QVERIFY(command.size() == 1);
    QVERIFY(command.first().command == "test");
    QVERIFY(command.first().arguments.contains("testValue"));


}

void deploytest::customTest() {
    //runTestParams({"-confFile", "",
    //               "qifFromSystem"});
}

void deploytest::testQmlExtrct() {
    QmlCreator creator("./");
    auto imports = creator.getQmlImports();

    auto qmlFiles = creator.getCopyedQml();


    QML scaner("./", QtMajorVersion::Qt5);


    for (const auto &file : qAsConst(qmlFiles)) {


        auto fileImports = scaner.extractImportsFromFile(file);

        for (const auto &fil : imports.value(file)) {
            QVERIFY(fileImports.contains(fil, Qt::CaseInsensitive));
        }

    }
}

void deploytest::testDistroStruct() {
    DistroStruct distro;

    auto cases = QList<QPair<QString,QString>>{
        {"", "/"},
        {"/", "/"},
        {"/res","/../"},
        {"/res/","/../"},
        {"/res/type","/../../"},
        {"/res/type/","/../../"},
        {"res/type","../../"},
        {"res/type/","../../"},
        {"res//type/","../../"},
        {"res////type/","../../"},
        {"//res///type/////","/../../"},
        {"\\", "/"},
        {"\\res","/../"},
        {"\\res\\","/../"},
        {"\\res\\type","/../../"},
        {"\\res\\type\\","/../../"},
        {"res\\type","../../"},
        {"res\\type\\","../../"},
        {"res\\\\type\\","../../"},
        {"res\\\\\\\\type\\","../../"},
        {"\\\\res\\\\\\type\\\\\\\\\\","/../../"},
    };

    for (const auto &i: qAsConst(cases)) {
        if (distro.getRelativePath(i.first) != i.second)
            QVERIFY(false);
    }

    distro = DistroStruct();

    distro.setTrOutDir("/tr/");
    QVERIFY(distro.getTrOutDir() == "/tr/");


    distro.setTrOutDir("/tr");
    QVERIFY(distro.getTrOutDir() == "/tr/");

    distro.setTrOutDir("tr");
    QVERIFY(distro.getTrOutDir() == "/tr/");


}

void deploytest::testRelativeLink() {
    auto cases = QList<QList<QString>>{
    {"", "", "./"},
    {"/media", "/etc", "./../etc/"},
    {"/media///", "/etc///", "./../etc/"},
    {"/media/etc/usr", "/media/etc", "./../"},
    {"/media/etc", "/media/etc/usr", "./usr/"},

    {"C:/", "C:/", "./"},
    {"C:\\", "C:/", "./"},
    {"C:/", "C:\\", "./"},

    {"C:/media", "C:/etc", "./../etc/"},
    {"C:/media//\\", "C:/etc///", "./../etc/"},
    {"C:/media/etc/usr", "C:/media/etc", "./../"},
    {"C:/media\\etc", "C:/media/etc/usr", "./usr/"},
    {"C:/media/etc", "D:/media/etc/usr", "D:/media/etc/usr"},

};

    for (const auto &i: qAsConst(cases)) {
        if (PathUtils::getRelativeLink(i[0], i[1]) != i[2])
            QVERIFY(false);
    }

    for (int i = 1; i < cases.size() - 1; i++) {
        if (!PathUtils::isAbsalutPath(cases[i][0]))
            QVERIFY(false);
        if (PathUtils::isAbsalutPath(cases[i][2]))
            QVERIFY(false);

    }
}

void deploytest::testCheckQt() {

    Deploy *deployer = new Deploy();
    QuasarAppUtils::Params::parseParams({"-bin", TestBinDir, "clear",
                                         "noCheckRPATH", "noCheckPATH", "noQt"});
    QVERIFY(deployer->prepare());


    auto cases = QList<QPair<QString, QtMajorVersion>>{
        {TestQtDir + "/", QtMajorVersion::NoQt},
        {TestQtDir + "", QtMajorVersion::NoQt},
        {TestQtDir + "/bin/file1", QtMajorVersion::NoQt},
        {TestQtDir + "/lib/file12.so", QtMajorVersion::NoQt},
        {TestQtDir + "/resurces/file13.dll", QtMajorVersion::NoQt},
        {TestQtDir + "/libexec/f", QtMajorVersion::NoQt},
        {TestQtDir + "/mkspecs", QtMajorVersion::NoQt},
        {TestQtDir + "/qml", QtMajorVersion::NoQt},
        {TestQtDir + "/plugins", QtMajorVersion::NoQt},
        {TestQtDir + "/file", QtMajorVersion::NoQt},

        {TestQtDir + "\\", QtMajorVersion::NoQt},
        {TestQtDir + "", QtMajorVersion::NoQt},
        {TestQtDir + "\\bin\\file1", QtMajorVersion::NoQt},
        {TestQtDir + "\\lib\\file12", QtMajorVersion::NoQt},
        {TestQtDir + "\\resurces\\file13", QtMajorVersion::NoQt},
        {TestQtDir + "\\libexec\\f.so", QtMajorVersion::NoQt},
        {TestQtDir + "\\mkspecs.dll", QtMajorVersion::NoQt},
        {TestQtDir + "\\qml", QtMajorVersion::NoQt},
        {TestQtDir + "\\plugins", QtMajorVersion::NoQt},
        {TestQtDir + "\\file", QtMajorVersion::NoQt},

    };

    for (const auto &i: qAsConst(cases)) {
        QVERIFY(DeployCore::isQtLib(i.first) == i.second);
    }
    delete deployer;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestQMLWidgets";
    QString qmake = TestQtDir + "bin/qmake";

#else
    QString bin = TestBinDir + "TestQMLWidgets.exe";
    QString qmake = TestQtDir + "bin/qmake.exe";
#endif

    deployer = new Deploy();
    QuasarAppUtils::Params::parseParams({"-bin", bin, "clear" ,
                                         "-qmake", qmake,
                                         "-qmlDir", TestBinDir + "/../TestQMLWidgets"});
    QVERIFY(deployer->prepare());


    cases = QList<QPair<QString, QtMajorVersion>>{
        {TestQtDir + "/", QtMajorVersion::NoQt},
        {TestQtDir + "", QtMajorVersion::NoQt},
        {TestQtDir + "/bin/file1", QtMajorVersion::NoQt},
        {TestQtDir + "/lib/file12", QtMajorVersion::NoQt},
        {TestQtDir + "/lib/file12", QtMajorVersion::NoQt},

        {TestQtDir + "/mkspecs", QtMajorVersion::NoQt},
        {TestQtDir + "/qml", QtMajorVersion::NoQt},
        {TestQtDir + "/plugins", QtMajorVersion::NoQt},
        {TestQtDir + "/file", QtMajorVersion::NoQt},

        {TestQtDir + "\\", QtMajorVersion::NoQt},
        {TestQtDir + "", QtMajorVersion::NoQt},
        {TestQtDir + "\\lib\\file12", QtMajorVersion::NoQt},
        {TestQtDir + "\\libexec\\fQt", QtMajorVersion::NoQt},
        {TestQtDir + "\\mkspecs", QtMajorVersion::NoQt},
        {TestQtDir + "\\qml", QtMajorVersion::NoQt},
        {TestQtDir + "\\plugins", QtMajorVersion::NoQt},
        {TestQtDir + "\\file", QtMajorVersion::NoQt},

        {TestQtDir + "/bin/file1Qt4.so", QtMajorVersion::Qt4},
        {TestQtDir + "/resources/Qt4file13.so", QtMajorVersion::Qt4},
        {TestQtDir + "/libexec/Qt4f.dll", QtMajorVersion::Qt4},
        {TestQtDir + "\\bin\\Qt4file1.dll", QtMajorVersion::Qt4},
        {TestQtDir + "\\resources\\fileQt413.dll", QtMajorVersion::Qt4},

        {TestQtDir + "/bin/file1Qt5.so", QtMajorVersion::Qt5},
        {TestQtDir + "/resources/Qt5file13.so", QtMajorVersion::Qt5},
        {TestQtDir + "/libexec/Qt5f.dll", QtMajorVersion::Qt5},
        {TestQtDir + "\\bin\\Qt5file1.dll", QtMajorVersion::Qt5},
        {TestQtDir + "\\resources\\fileQt513.dll", QtMajorVersion::Qt5},

        {TestQtDir + "/bin/file1Qt6.so", QtMajorVersion::Qt6},
        {TestQtDir + "/resources/Qt6file13.so", QtMajorVersion::Qt6},
        {TestQtDir + "/libexec/Qt6f.dll", QtMajorVersion::Qt6},
        {TestQtDir + "\\bin\\Qt6file1.dll", QtMajorVersion::Qt6},
        {TestQtDir + "\\resources\\fileQt613.dll", QtMajorVersion::Qt6},

    };

    for (const auto &i: qAsConst(cases)) {
        auto dexription = QString("The isQtLib(%0) function should be return %1").arg(
                    i.first).arg(i.second);
        QVERIFY2(DeployCore::isQtLib(i.first) == i.second, dexription.toLatin1().data());
    }

    delete deployer;
}

void deploytest::testSetTargetDir() {

    FileManager file;
    DependenciesScanner scan;
    Packing pac(&file);
    PluginsParser _plugins;
    ConfigParser  dep(&file, &_plugins, &scan, &pac);

    dep.setTargetDir();

    QVERIFY(dep.config()->getTargetDir() == QFileInfo("./" + DISTRO_DIR + "").absoluteFilePath());
    dep.setTargetDir("./ff");
    QVERIFY(dep.config()->getTargetDir() == QFileInfo("./ff").absoluteFilePath());

    QStringList argv = {"-targetDir", "./" + DISTRO_DIR + "2"};

    QuasarAppUtils::Params::parseParams(argv);

    dep.setTargetDir();
    QVERIFY(dep.config()->getTargetDir() == QFileInfo("./" + DISTRO_DIR + "2").absoluteFilePath());
    dep.setTargetDir("./ff");
    QVERIFY(dep.config()->getTargetDir() == QFileInfo("./" + DISTRO_DIR + "2").absoluteFilePath());

}

void deploytest::testZip() {
    TestUtils utils;

    ZipCompresser zip;
    auto befor = utils.getTree("./test");

    QVERIFY(zip.compress("./test", "./arr.zip"));
    QVERIFY(QDir("./test").removeRecursively());
    QVERIFY(zip.extract("./arr.zip", "./test"));

    auto after = utils.getTree("./test");

    QVERIFY(utils.compareTree(befor, after).size() == 0);


}

void deploytest::runTestParams(QStringList list,
                               QSet<QString>* tree,
                               bool noWarnings, bool onlySize,
                               exitCodes exitCode,
                               const std::function<void (const DeployConfig *)> &cb) {

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

void deploytest::checkResults(const QSet<QString> &tree,
                              bool noWarnings,
                              bool onlySize) {
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

void deploytest::createTree(const QStringList &tree) {
    for (const auto& dir : tree) {
        QDir().mkpath(dir);
    }
}

void deploytest::costomScript() {
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

void deploytest::testOverwrite() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QFile f("./" + DISTRO_DIR + "/bin/TestOnlyC");
    auto comapareTree = utils.createTree(
                {"./" + DISTRO_DIR + "/bin/TestOnlyC",
                 "./" + DISTRO_DIR + "/bin/qt.conf",
                 "./" + DISTRO_DIR + "/TestOnlyC.sh"});
    QString bin = TestBinDir + "TestOnlyC";

#else
    QFile f("./" + DISTRO_DIR + "/TestOnlyC.exe");
    auto comapareTree = utils.createTree(
                {"./" + DISTRO_DIR + "/TestOnlyC.exe",
                 "./" + DISTRO_DIR + "/TestOnlyC.bat",
                 "./" + DISTRO_DIR + "/qt.conf"});
    QString bin = TestBinDir + "TestOnlyC.exe";

#endif

    runTestParams({"-bin", bin, "force-clear", "noOverwrite"}, &comapareTree);

    QVERIFY(f.open(QIODevice::ReadOnly));
    auto hashBefor = QCryptographicHash::hash(f.readAll(), QCryptographicHash::Md5);
    f.close();

    QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Append));
    f.write(QByteArray(10, '1'));
    f.close();

    QVERIFY(f.open(QIODevice::ReadOnly));
    auto hashAfter = QCryptographicHash::hash(f.readAll(), QCryptographicHash::Md5);
    f.close();

    QVERIFY(hashAfter != hashBefor);

    runTestParams({"-bin", bin, "noOverwrite"}, &comapareTree);

    QVERIFY(f.open(QIODevice::ReadOnly));
    hashAfter = QCryptographicHash::hash(f.readAll(), QCryptographicHash::Md5);
    f.close();

    QVERIFY(hashAfter != hashBefor);


    runTestParams({"-bin", bin}, &comapareTree);

    QVERIFY(f.open(QIODevice::ReadOnly));
    hashAfter = QCryptographicHash::hash(f.readAll(), QCryptographicHash::Md5);
    f.close();

    QVERIFY(hashAfter == hashBefor);

}

void deploytest::testOverwriteWithPacking() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC," + TestBinDir + "TestCPPOnly";

#else
    QString bin = TestBinDir + "TestOnlyC.exe," + TestBinDir + "TestCPPOnly.exe";

#endif
#ifdef Q_OS_UNIX
    auto comapareTreeqif = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/InstallerTest.run",
                    "./" + DISTRO_DIR + "/InstallerTest.run.md5",
                });
#else
    auto comapareTreeqif = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/InstallerTest.exe",
                    "./" + DISTRO_DIR + "/InstallerTest.exe.md5",

                });

#endif

    runTestParams({"-bin", bin,
                   "force-clear",
                   "noOverwrite",
                   "qif",
                   "qifFromSystem",
                   "-name", "Test"}, &comapareTreeqif);
}

void deploytest::testextraData() {
    TestUtils utils;


#ifdef Q_OS_UNIX
    auto comapareTree = utils.createTree(
                {"./" + DISTRO_DIR + "/build/TestOnlyC",
                 "./" + DISTRO_DIR + "/build/TestCPPOnly",
                 "./" + DISTRO_DIR + "/build/QtWidgetsProject",
                 "./" + DISTRO_DIR + "/build/TestQMLWidgets",
                 "./" + DISTRO_DIR + "/build/basic",
                 "./" + DISTRO_DIR + "/build/quicknanobrowser",
                 "./" + DISTRO_DIR + "/build/webui"});
#else
    auto comapareTree = utils.createTree(
                {"./" + DISTRO_DIR + "/build/TestOnlyC.exe",
                 "./" + DISTRO_DIR + "/build/TestCPPOnly.exe",
                 "./" + DISTRO_DIR + "/build/QtWidgetsProject.exe",
                 "./" + DISTRO_DIR + "/build/TestQMLWidgets.exe",
                 "./" + DISTRO_DIR + "/build/basic.exe"});
#endif


    runTestParams({"-extraData", TestBinDir, "clear",
                   "noCheckRPATH", "noCheckPATH", "noQt"}, &comapareTree);


    comapareTree = TestModule.replace(comapareTree, {
                                          {"DistributionKit/build",
                                           "DistributionKit/myExtraData/build"}});

    runTestParams({"-extraData", TestBinDir, "clear",
                   "noCheckRPATH", "noCheckPATH", "noQt",
                   "-extraDataOut", "myExtraData"}, &comapareTree);
}

void deploytest::testConfFile() {
    TestUtils utils;


    QFile::remove(TestBinDir + "/TestConf.json");
    QFile::remove(TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json");

#ifdef Q_OS_UNIX
    auto comapareTree = utils.createTree(
                {"./" + DISTRO_DIR + "/bin/TestOnlyC",
                 "./" + DISTRO_DIR + "/bin/qt.conf",
                 "./" + DISTRO_DIR + "/TestOnlyC.sh"});
#else
    auto comapareTree = utils.createTree(
                {"./" + DISTRO_DIR + "/TestOnlyC.exe",
                 "./" + DISTRO_DIR + "/TestOnlyC.bat",
                 "./" + DISTRO_DIR + "/qt.conf"});
#endif

#ifdef Q_OS_UNIX
    runTestParams({"-bin", TestBinDir + "TestOnlyC", "clear" , "noCheckRPATH", "noCheckPATH", "noQt",
                   "-confFile", TestBinDir + "/TestConf.json"}, &comapareTree);
#else
    runTestParams({"-bin", TestBinDir + "TestOnlyC.exe", "clear" , "noCheckRPATH", "noCheckPATH", "noQt",
                   "-confFile", TestBinDir + "/TestConf.json"}, &comapareTree);
#endif


    QVERIFY(QFile::exists(TestBinDir + "/TestConf.json"));
    QFile::remove(TestBinDir + "/TestConf.json");



#ifdef Q_OS_UNIX
    comapareTree += utils.createTree(
                {"./" + DISTRO_DIR + "/bin/TestCPPOnly",
                 "./" + DISTRO_DIR + "/TestCPPOnly.sh"});
#else
    comapareTree += utils.createTree(
                {"./" + DISTRO_DIR + "/TestCPPOnly.exe",
                 "./" + DISTRO_DIR + "/TestCPPOnly.bat"});
#endif

#ifdef Q_OS_UNIX
    runTestParams({"-bin", TestBinDir + "TestOnlyC," + TestBinDir + "TestCPPOnly",
                   "clear", "noCheckRPATH", "noCheckPATH", "noQt",
                   "-confFile", TestBinDir + "/TestConf.json"}, &comapareTree);
#else
    runTestParams({"-bin", TestBinDir + "TestOnlyC.exe," + TestBinDir + "TestCPPOnly.exe",
                   "clear" , "-libDir", "L:/never/absalut/path", "noCheckPATH", "noQt",
                   "-confFile", TestBinDir + "/TestConf.json"}, &comapareTree);
#endif

    QFile confFile(TestBinDir + "/TestConf.json");
    QVERIFY(confFile.open(QIODevice::ReadOnly));

    auto data = confFile.readAll();
    confFile.close();

    QJsonDocument doc;
    doc = doc.fromJson(data);
    QVERIFY(!doc.isNull());

#ifdef Q_OS_UNIX

    QVERIFY(data.contains("\"bin\": ["));
    QVERIFY(data.contains("./TestOnlyC"));
    QVERIFY(data.contains("./TestCPPOnly"));

    QVERIFY(data.contains("\"clear\": true"));

    data.insert(data.size() - 2, QString(",\"libDir\": \"/never/absalut/path/\"").toLatin1());

    QVERIFY(confFile.open(QIODevice::WriteOnly | QIODevice::Truncate));
    confFile.write(data);
    confFile.close();

    runTestParams({"-confFile", TestBinDir + "/TestConf.json"}, &comapareTree);
#else

    QVERIFY(data.contains("\"bin\": ["));
    QVERIFY(data.contains("./TestOnlyC.exe"));
    QVERIFY(data.contains("./TestCPPOnly.exe"));
    QVERIFY(data.contains("\"libDir\": \"L:/never/absalut/path\""));

    QVERIFY(data.contains("\"clear\": true"));

    runTestParams({"-confFile", TestBinDir + "/TestConf.json"}, &comapareTree);

#endif

    QVERIFY(QuasarAppUtils::Params::isEndable("clear"));
    QVERIFY(QuasarAppUtils::Params::isEndable("bin"));
    QVERIFY(QuasarAppUtils::Params::isEndable("libDir"));
#ifdef Q_OS_UNIX
    QVERIFY(QuasarAppUtils::Params::getArg("libDir") == "/never/absalut/path/");
#else
    QVERIFY(QuasarAppUtils::Params::getArg("libDir") == "L:/never/absalut/path");
#endif
    QFile::remove(TestBinDir + "/TestConf.json");


#ifdef Q_OS_UNIX
    runTestParams({"-bin", TestBinDir + "TestOnlyC," + TestBinDir + "TestCPPOnly",
                   "clear" , "noCheckRPATH", "noCheckPATH", "noQt",
                   "-confFile", TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json"}, &comapareTree);
#else
    runTestParams({"-bin", TestBinDir + "TestOnlyC.exe," + TestBinDir + "TestCPPOnly.exe",
                   "clear" , "noCheckPATH", "noQt",
                   "-confFile", TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json"}, &comapareTree);
#endif

    confFile.setFileName(TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json");
    QVERIFY(confFile.open(QIODevice::ReadOnly));

    data = confFile.readAll();
    confFile.close();

    doc = doc.fromJson(data);
    QVERIFY(!doc.isNull());

#ifdef Q_OS_UNIX

    QVERIFY(data.contains("\"bin\": ["));
    QVERIFY(data.contains("./../../../../../build/TestOnlyC"));
    QVERIFY(data.contains("./../../../../../build/TestCPPOnly"));

    QVERIFY(data.contains("\"clear\": true"));
    QString qmake = TestQtDir + "bin/qmake";

#else

    QVERIFY(data.contains("\"bin\": ["));
    QVERIFY(data.contains("./../../../../../build/TestOnlyC.exe"));
    QVERIFY(data.contains("./../../../../../build/TestCPPOnly.exe"));

    QVERIFY(data.contains("\"clear\": true"));
    QString qmake = TestQtDir + "bin/qmake.exe";

#endif
    runTestParams({"-confFile", TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json"}, &comapareTree);

    QVERIFY(QuasarAppUtils::Params::isEndable("clear"));
    QVERIFY(QuasarAppUtils::Params::isEndable("bin"));

    QFile::remove(TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json");

    // Test generar string in confFile
    comapareTree = TestModule.qtLibs();

    comapareTree = TestModule.ignoreFilter(comapareTree, "/plugins/p");
#ifdef Q_OS_UNIX
    comapareTree -= utils.createTree(
                {
                    "./" + DISTRO_DIR + "/lib/libQt5EglFSDeviceIntegration.so",
                });
    auto bin =  TestBinDir + "QtWidgetsProject";
#else
    comapareTree -= utils.createTree(
                {
                    "./" + DISTRO_DIR + "/Qt5DBus.dll"

                });
    auto bin =  TestBinDir + "QtWidgetsProject.exe";
#endif

    runTestParams({"-bin", bin,
                   "-qmake", qmake,
                   "clear",
                   "-ignore", "/plugins/p",
                   "-confFile", TestBinDir + "/TestConf.json"}, &comapareTree);


    runTestParams({"-confFile", TestBinDir + "/TestConf.json"}, &comapareTree);
    QFile::remove(TestBinDir + "/TestConf.json");

#ifdef Q_OS_UNIX
    QFile f("./" + DISTRO_DIR + "/bin/TestOnlyC");
    comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/package/TestOnlyC.sh",
                    "./" + DISTRO_DIR + "/package/bin/TestOnlyC",
                    "./" + DISTRO_DIR + "/package/bin/qt.conf"
                });
    QString target1 = TestBinDir + "TestOnlyC";

#else
    QFile f("./" + DISTRO_DIR + "/TestOnlyC.exe");
    comapareTree = utils.createTree(
                {"./" + DISTRO_DIR + "/package/TestOnlyC.exe",
                 "./" + DISTRO_DIR + "/package/TestOnlyC.bat",
                 "./" + DISTRO_DIR + "/package/qt.conf"});
    QString target1 = TestBinDir + "TestOnlyC.exe";

#endif
    bin = target1;

    runTestParams({"-bin", bin, "force-clear",
                   "-targetPackage", "package;TestOn",
                   "-confFile", TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json"}, &comapareTree);

    runTestParams({"-confFile", TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json"},
                  &comapareTree);
    QFile::remove(TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json");

    auto file = "testCase.json";
#ifdef Q_OS_UNIX
    bin =  TestBinDir + "QtWidgetsProject," + TestBinDir + "TestOnlyC";
#else
    bin =  TestBinDir + "QtWidgetsProject.exe," + TestBinDir + "TestOnlyC.exe";
#endif
    QVERIFY(utils.deployFile(":/testResurces/testRes/testMultiPackageConfig.json", file,
                             {{"$BIN_DIR", bin.toLatin1()}}));

    comapareTree = TestModule.onlyC(DISTRO_DIR + "/Dstro1") +
            TestModule.qtLibs(DISTRO_DIR + "/Dstro2");


    runTestParams({"-confFile", file},
                  &comapareTree);

    QFile::remove(TestBinDir + "/TestConf.json");
    QFile::remove(TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json");

}

void deploytest::testPackages() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QFile f("./" + DISTRO_DIR + "/bin/TestOnlyC");
    auto comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/package/TestOnlyC.sh",
                    "./" + DISTRO_DIR + "/package/bin/TestOnlyC",
                    "./" + DISTRO_DIR + "/package/bin/qt.conf"
                });
    QString target1 = TestBinDir + "TestOnlyC";

#else
    QFile f("./" + DISTRO_DIR + "/TestOnlyC.exe");
    auto comapareTree = utils.createTree(
                {"./" + DISTRO_DIR + "/package/TestOnlyC.exe",
                 "./" + DISTRO_DIR + "/package/TestOnlyC.bat",
                 "./" + DISTRO_DIR + "/package/qt.conf"});
    QString target1 = TestBinDir + "TestOnlyC.exe";

#endif
    QString bin = target1;

    runTestParams({"-bin", bin, "force-clear",
                   "-targetPackage", "/package/;TestOn"}, &comapareTree);

    runTestParams({"-bin", bin, "force-clear",
                   "-targetPackage", "/package/;" + QFileInfo(target1).absoluteFilePath()}, &comapareTree);

#ifdef Q_OS_UNIX
    QString target2 = TestBinDir + "TestQMLWidgets";
    QString target3 = TestBinDir + "QtWidgetsProject";

#else
    QString target2 = TestBinDir + "TestQMLWidgets.exe";
    QString target3 = TestBinDir + "QtWidgetsProject.exe";

#endif
    bin += "," + target2;
    bin += "," + target3;

    auto packageString = "package1;" + QFileInfo(target1).absoluteFilePath() + ",package2/ZzZ;" + QFileInfo(target2).absoluteFilePath();

    comapareTree = TestModule.testDistroLibs(DISTRO_DIR);

    runTestParams({"-bin", bin, "force-clear",
                   "-binOut", "/lol",
                   "-libOut", "/lolLib",
                   "-trOut", "/lolTr",
                   "-pluginOut", "/p",
                   "-qmlOut", "package2/ZzZ;/q/and/q,/q",
                   "-qmlDir", "package2/ZzZ;" + TestBinDir + "/../TestQMLWidgets",
                   "-targetPackage", packageString}, &comapareTree);


#ifdef Q_OS_UNIX

    // test a wrapers source
    QFile wraper("./" + DISTRO_DIR + "/package2/ZzZ/TestQMLWidgets.sh");

    QVERIFY(wraper.open(QIODevice::ReadOnly));
    auto data = wraper.readAll();
    wraper.close();

    wraper.setFileName(":/testResurces/testRes/TestQMLWidgets.sh");
    QVERIFY(wraper.open(QIODevice::ReadOnly));
    QVERIFY(wraper.readAll() == data);
    wraper.close();
#endif
}

void deploytest::testQt() {
    TestUtils utils;


#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "QtWidgetsProject";
    QString qmake = TestQtDir + "bin/qmake";

#else
    QString bin = TestBinDir + "QtWidgetsProject.exe";
    QString qmake = TestQtDir + "bin/qmake.exe";

#endif


    auto comapareTree = TestModule.qtLibs();

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake}, &comapareTree);

    // test auto detection of detection qmake from PATH
    runTestParams({"-bin", bin, "clear", "noCheckRPATH"}, &comapareTree);

#ifdef Q_OS_UNIX
    // test auto detection of detection qmake from RPATH
    runTestParams({"-bin", bin, "clear", "noCheckPATH"}, &comapareTree);
#endif


    comapareTree = TestModule.qtWithoutTr();


    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake, "noTranslations"}, &comapareTree);


    comapareTree = TestModule.qmlLibs();

#ifdef Q_OS_UNIX
    bin = TestBinDir + "TestQMLWidgets";
#else
    bin = TestBinDir + "TestQMLWidgets.exe";

#endif

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-qmlDir", TestBinDir + "/../TestQMLWidgets"}, &comapareTree);


#ifdef Q_OS_UNIX

    runTestParams({"-bin", bin, "clear" ,
                   "-qmlDir", TestBinDir + "/../TestQMLWidgets"}, &comapareTree);

#endif

}

void deploytest::testClear() {
    TestUtils utils;


    auto compareTree = QSet<QString>{};

    runTestParams({"clear"}, &compareTree);

}

void deploytest::testIgnore() {
    TestUtils utils;


#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "QtWidgetsProject";
    QString qmake = TestQtDir + "bin/qmake";
    auto comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/QtWidgetsProject.sh",
                    "./" + DISTRO_DIR + "/bin/qt.conf",
                    "./" + DISTRO_DIR + "/bin/QtWidgetsProject",
                });

#else
    QString bin = TestBinDir + "QtWidgetsProject.exe";
    QString qmake = TestQtDir + "bin/qmake.exe";

    auto comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/qt.conf",
                    "./" + DISTRO_DIR + "/QtWidgetsProject.exe",
                    "./" + DISTRO_DIR + "/QtWidgetsProject.bat",
                #if defined(Q_OS_WIN64)
                    "./" + DISTRO_DIR + "/libgcc_s_seh-1.dll",
                #else
                    "./" + DISTRO_DIR + "/libgcc_s_dw2-1.dll",
                #endif
                    "./" + DISTRO_DIR + "/libstdc++-6.dll",
                    "./" + DISTRO_DIR + "/libwinpthread-1.dll"

                });

#endif


    if (!TestQtDir.contains("Qt5")) {

#ifdef Q_OS_UNIX
        bin = TestBinDir + "QtWidgetsProject";
        qmake = TestQtDir + "bin/qmake";

#else
        bin = TestBinDir + "QtWidgetsProject.exe";
        qmake = TestQtDir + "bin/qmake.exe";

#endif
    }


    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-ignore", "Qt5"}, &comapareTree);



#ifdef Q_OS_UNIX
    auto removeTree = utils.createTree({
                                           "./" + DISTRO_DIR + "/plugins/virtualkeyboard/libqtvirtualkeyboard_hangul.so",
                                           "./" + DISTRO_DIR + "/plugins/virtualkeyboard/libqtvirtualkeyboard_openwnn.so",
                                           "./" + DISTRO_DIR + "/plugins/virtualkeyboard/libqtvirtualkeyboard_pinyin.so",
                                           "./" + DISTRO_DIR + "/plugins/virtualkeyboard/libqtvirtualkeyboard_tcime.so",
                                           "./" + DISTRO_DIR + "/plugins/virtualkeyboard/libqtvirtualkeyboard_thai.so",
                                           "./" + DISTRO_DIR + "/plugins/platforminputcontexts/libqtvirtualkeyboardplugin.so",
                                           "./" + DISTRO_DIR + "/lib/libQt5VirtualKeyboard.so",

                                       });
#else
    auto removeTree = utils.createTree({
                                           "./" + DISTRO_DIR + "/Qt5VirtualKeyboard.dll",
                                           "./" + DISTRO_DIR + "/plugins/platforminputcontexts/qtvirtualkeyboardplugin.dll",
                                           "./" + DISTRO_DIR + "/plugins/virtualkeyboard/qtvirtualkeyboard_hangul.dll",
                                           "./" + DISTRO_DIR + "/plugins/virtualkeyboard/qtvirtualkeyboard_openwnn.dll",
                                           "./" + DISTRO_DIR + "/plugins/virtualkeyboard/qtvirtualkeyboard_pinyin.dll",
                                           "./" + DISTRO_DIR + "/plugins/virtualkeyboard/qtvirtualkeyboard_tcime.dll",
                                           "./" + DISTRO_DIR + "/plugins/virtualkeyboard/qtvirtualkeyboard_thai.dll"
                                       });

#endif

    comapareTree = TestModule.qtLibs() - removeTree;

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-ignore", "VirtualKeyboard"}, &comapareTree);

}

void deploytest::testIgnoreEnv() {



    Envirement env;
    QDir("./testTree").removeRecursively();

    QStringList ignoreTree = {
        "./testTree/test",
        "./testTree/",
        "./testTree/test1/1",
        "./testTree/test2/1/",
    };

    QStringList testTree = {
        "./testTree/test/z",
        "./testTree/z",
        "./testTree/test1/1z",
        "./testTree/test2/1/z",
    };

    createTree(ignoreTree);
    createTree(testTree);

    env.setIgnoreEnvList(ignoreTree);
    env.addEnv(ignoreTree);

    // must be empty becouse all pathes is ignored
    QVERIFY(env.size() == 0);

    env.addEnv(testTree);

    // must be equals 4 becouse all pathes is not ignored
    QVERIFY(env.size() == 4);

    // try add dublicate
    env.addEnv(testTree);

    // must be equals 4 becouse all dublicates must be ignored
    QVERIFY(env.size() == 4);

    QVERIFY(QDir("./testTree").removeRecursively());
}

void deploytest::testLibDir() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";
    QString extraPath = "/usr/lib,/lib";

    auto comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/TestOnlyC.sh",
                    "./" + DISTRO_DIR + "/bin/qt.conf",
                    "./" + DISTRO_DIR + "/bin/TestOnlyC"
                });

#else
    QString bin = TestBinDir + "TestOnlyC.exe";
    QString extraPath = TestQtDir;

    auto comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/qt.conf",
                    "./" + DISTRO_DIR + "/TestOnlyC.exe",
                    "./" + DISTRO_DIR + "/TestOnlyC.bat",

                });

#endif




    runTestParams({"-bin", bin, "clear" ,
                   "-libDir", extraPath,
                  }, &comapareTree);


#ifdef Q_OS_UNIX
    comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/TestOnlyC.sh",
                    "./" + DISTRO_DIR + "/bin/qt.conf",
                    "./" + DISTRO_DIR + "/bin/TestOnlyC",
                    "./" + DISTRO_DIR + "/lib/libstdc++.so",
                    "./" + DISTRO_DIR + "/lib/libgcc_s.so",
                    "./" + DISTRO_DIR + "/lib/ld-linux-x86-64.so",
                    "./" + DISTRO_DIR + "/lib/libc.so",
                    "./" + DISTRO_DIR + "/lib/libm.so",
                });

    auto comapareTreeExtraLib = utils.createTree(
                {
                    "./" + DISTRO_DIR + "2/TestOnlyC.sh",
                    "./" + DISTRO_DIR + "2/bin/qt.conf",
                    "./" + DISTRO_DIR + "2/bin/TestOnlyC",
                    "./" + DISTRO_DIR + "2/lib/libstdc++.so",
                    "./" + DISTRO_DIR + "2/lib/libgcc_s.so"
                });

#else
    comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/qt.conf",
                    "./" + DISTRO_DIR + "/TestOnlyC.exe",
                    "./" + DISTRO_DIR + "/TestOnlyC.bat",
                #if defined(Q_OS_WIN64)
                    "./" + DISTRO_DIR + "/libgcc_s_seh-1.dll",
                #else
                    "./" + DISTRO_DIR + "/libgcc_s_dw2-1.dll",
                #endif
                    "./" + DISTRO_DIR + "/libwinpthread-1.dll",
                    "./" + DISTRO_DIR + "/libstdc++-6.dll",

                });

    auto comapareTreeExtraLib = utils.createTree(
                {
                    "./" + DISTRO_DIR + "2/qt.conf",
                    "./" + DISTRO_DIR + "2/TestOnlyC.exe",
                    "./" + DISTRO_DIR + "2/TestOnlyC.bat",
                #if defined(Q_OS_WIN64)
                    "./" + DISTRO_DIR + "2/libgcc_s_seh-1.dll",
                #else
                    "./" + DISTRO_DIR + "2/libgcc_s_dw2-1.dll",
                #endif
                    "./" + DISTRO_DIR + "2/libstdc++-6.dll",

                });

#endif


    runTestParams({"-bin", bin, "clear" ,
                   "-libDir", extraPath,
                   "-recursiveDepth", "5",
                   "noCheckRPATH, noCheckPATH", "noQt"}, &comapareTree, true);

    runTestParams({"-bin", bin, "clear" ,
                   "-targetDir", "./" + DISTRO_DIR + "2",
                   "-extraLibs", "stdc,gcc",
                   "noCheckRPATH, noCheckPATH", "noQt"}, &comapareTreeExtraLib, true);

    //task #258
    //https://github.com/QuasarApp/CQtDeployer/issues/258


#ifdef Q_OS_UNIX
    comapareTreeExtraLib = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/TestOnlyC.sh",
                    "./" + DISTRO_DIR + "/bin/qt.conf",
                    "./" + DISTRO_DIR + "/bin/TestOnlyC",
                    "./" + DISTRO_DIR + "/lib/libstdc++.so",
                    "./" + DISTRO_DIR + "/lib/libgcc_s.so"
                });
    extraPath = "./" + DISTRO_DIR + "2/lib";


#else

    comapareTreeExtraLib = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/qt.conf",
                    "./" + DISTRO_DIR + "/TestOnlyC.exe",
                    "./" + DISTRO_DIR + "/TestOnlyC.bat",
                #if defined(Q_OS_WIN64)
                    "./" + DISTRO_DIR + "/libgcc_s_seh-1.dll",
                #else
                    "./" + DISTRO_DIR + "/libgcc_s_dw2-1.dll",
                #endif
                    "./" + DISTRO_DIR + "/libstdc++-6.dll",

                });
    extraPath = "./" + DISTRO_DIR + "2";

#endif
    runTestParams({"-bin", bin, "clear" ,
                   "-libDir", extraPath,
                   "noCheckRPATH, noCheckPATH", "noQt"}, &comapareTreeExtraLib, true);

    QDir(extraPath).removeRecursively();

}

void deploytest::testExtraPlugins() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "QtWidgetsProject";
    QString qmake = TestQtDir + "bin/qmake";

    auto pluginTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/plugins/sqldrivers/libqsqlodbc.so",
                    "./" + DISTRO_DIR + "/plugins/sqldrivers/libqsqlpsql.so",
                    "./" + DISTRO_DIR + "/plugins/sqldrivers/libqsqlite.so",
                    "./" + DISTRO_DIR + "/lib/libQt5Sql.so",
                    "./" + DISTRO_DIR + "/lib/libpq.so",
                    "./" + DISTRO_DIR + "/lib/libcrypto.so",
                    "./" + DISTRO_DIR + "/lib/libssl.so",

                });
#else
    QString bin = TestBinDir + "QtWidgetsProject.exe";
    QString qmake = TestQtDir + "bin/qmake.exe";

    auto pluginTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/plugins/sqldrivers/qsqlmysql.dll",
                    "./" + DISTRO_DIR + "/plugins/sqldrivers/qsqlodbc.dll",
                    "./" + DISTRO_DIR + "/plugins/sqldrivers/qsqlite.dll",
                    "./" + DISTRO_DIR + "/plugins/sqldrivers/qsqlpsql.dll",
                    "./" + DISTRO_DIR + "/Qt5Sql.dll",
                    "./" + DISTRO_DIR + "/libpq.dll",

                });
#endif

    auto comapareTree = TestModule.qtLibs();

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                  }, &comapareTree);


    comapareTree = comapareTree + pluginTree;


    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-enablePlugins", "sqldrivers"}, &comapareTree);

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-extraPlugin", TestQtDir + "/plugins/sqldrivers"}, &comapareTree);

    comapareTree -= pluginTree;
    comapareTree -= utils.createTree(
                {
                    "./" + DISTRO_DIR + "/plugins/platforms/libqxcb.so",
                    "./" + DISTRO_DIR + "/lib/libxcb-xinerama.so.0",
                    "./" + DISTRO_DIR + "/plugins/xcbglintegrations/libqxcb-egl-integration.so",
                    "./" + DISTRO_DIR + "/plugins/xcbglintegrations/libqxcb-glx-integration.so",
                    "./" + DISTRO_DIR + "/lib/libQt5XcbQpa.so",

                });

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-disablePlugins", "qxcb,xcbglintegrations"}, &comapareTree);
}

void deploytest::testTargetDir() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";
    auto comapareTree = utils.createTree(
                {"./" + DISTRO_DIR + "Z/bin/TestOnlyC",
                 "./" + DISTRO_DIR + "Z/bin/qt.conf",
                 "./" + DISTRO_DIR + "Z/TestOnlyC.sh"});
#else
    QString bin = TestBinDir + "TestOnlyC.exe";

    auto comapareTree = utils.createTree(
                {"./" + DISTRO_DIR + "Z/TestOnlyC.exe",
                 "./" + DISTRO_DIR + "Z/TestOnlyC.bat",
                 "./" + DISTRO_DIR + "Z/qt.conf"});
#endif

    runTestParams({"-bin", bin, "clear" ,
                   "-targetDir", "./" + DISTRO_DIR + "Z"
                  }, &comapareTree);
}

void deploytest::testSystemLib() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";
    QString qmake = TestQtDir + "bin/qmake";

    auto comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/TestOnlyC.sh",
                    "./" + DISTRO_DIR + "/bin/qt.conf",
                    "./" + DISTRO_DIR + "/bin/TestOnlyC",
                    "./" + DISTRO_DIR + "/lib/systemLibs/libgcc_s.so",
                    "./" + DISTRO_DIR + "/lib/systemLibs/libstdc++.so"
                });

#else
    QString bin = TestBinDir + "TestOnlyC.exe";
    QString qmake = TestQtDir + "bin/qmake.exe";

    auto comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/TestOnlyC.exe",
                    "./" + DISTRO_DIR + "/TestOnlyC.bat",
                #if defined(Q_OS_WIN64)
                    "./" + DISTRO_DIR + "/systemLibs/libgcc_s_seh-1.dll",
                #else
                    "./" + DISTRO_DIR + "/systemLibs/libgcc_s_dw2-1.dll",
                #endif
                    "./" + DISTRO_DIR + "/systemLibs/libstdc++-6.dll",
                    "./" + DISTRO_DIR + "/systemLibs/libwinpthread-1.dll",
                    "./" + DISTRO_DIR + "/systemLibs/msvcrt.dll",
                    "./" + DISTRO_DIR + "/qt.conf"
                });

#endif

    runTestParams({"-bin", bin, "clear" ,
                   "deploySystem",
                   "-qmake", qmake,
                  }, &comapareTree);

#ifdef Q_OS_WIN
    bin = TestBinDir + "QtWidgetsProject.exe";

    comapareTree += TestModule.qtLibs();

    comapareTree -= utils.createTree(
                {
                    "./" + DISTRO_DIR + "/TestOnlyC.exe",
                    "./" + DISTRO_DIR + "/TestOnlyC.bat",
                #if defined(Q_OS_WIN64)
                    "./" + DISTRO_DIR + "/systemLibs/libgcc_s_seh-1.dll",
                #else
                    "./" + DISTRO_DIR + "/systemLibs/libgcc_s_dw2-1.dll",
                #endif
                    "./" + DISTRO_DIR + "/systemLibs/libstdc++-6.dll",
                    "./" + DISTRO_DIR + "/systemLibs/libwinpthread-1.dll",

                });

    comapareTree += utils.createTree(
                {
                    "./" + DISTRO_DIR + "/systemLibs/msvcrt.dll",
                    "./" + DISTRO_DIR + "/qt.conf",
                    "./" + DISTRO_DIR + "/systemLibs/mpr.dll",
                    "./" + DISTRO_DIR + "/systemLibs/profapi.dll",
                    "./" + DISTRO_DIR + "/systemLibs/rpcrt4.dll",
                    "./" + DISTRO_DIR + "/systemLibs/shell32.dll",
                    "./" + DISTRO_DIR + "/systemLibs/userenv.dll",
                    "./" + DISTRO_DIR + "/systemLibs/uxtheme.dll",
                    "./" + DISTRO_DIR + "/systemLibs/version.dll",
                    "./" + DISTRO_DIR + "/systemLibs/ucrtbase.dll",
                    "./" + DISTRO_DIR + "/systemLibs/oleaut32.dll",
                    "./" + DISTRO_DIR + "/systemLibs/bcryptprimitives.dll",
                    "./" + DISTRO_DIR + "/systemLibs/msvcp_win.dll",
                    "./" + DISTRO_DIR + "/systemLibs/wtsapi32.dll",
                    "./" + DISTRO_DIR + "/systemLibs/combase.dll",
                #if !defined(Q_OS_WIN64)
                    "./" + DISTRO_DIR + "/systemLibs/sspicli.dll",
                    "./" + DISTRO_DIR + "/systemLibs/cryptbase.dll",
                #endif

                });


#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    comapareTree += utils.createTree(
                {
                    "./" + DISTRO_DIR + "/systemLibs/d3d11.dll",
                    "./" + DISTRO_DIR + "/systemLibs/dxgi.dll",
                    "./" + DISTRO_DIR + "/systemLibs/win32u.dll",
                });
#endif


    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "deploySystem"
                  }, &comapareTree);


#endif
}

void deploytest::testOutDirs() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestQMLWidgets";
    QString qmake = TestQtDir + "bin/qmake";

#else
    QString bin = TestBinDir + "TestQMLWidgets.exe";
    QString qmake = TestQtDir + "bin/qmake.exe";


#endif

    auto comapareTree = TestModule.testOutLibs();

    runTestParams({"-bin", bin, "clear" ,
                   "-binOut", "/lol",
                   "-libOut", "/lolLib",
                   "-trOut", "/lolTr",
                   "-pluginOut", "/p",
                   "-qmlOut", "/q",
                   "-qmake", qmake,
                   "-qmlDir", TestBinDir + "/../TestQMLWidgets"
                  }, &comapareTree);

    QFile file;

    file.setFileName("./" + DISTRO_DIR + "/lol/qt.conf");

    QVERIFY(file.open(QIODevice::ReadOnly));

    auto runScript = file.readAll();
    file.close();

    QVERIFY(runScript.contains("Prefix= ./../"));
    QVERIFY(runScript.contains("Libraries= ./lolLib/"));
    QVERIFY(runScript.contains("Plugins= ./p/"));
    QVERIFY(runScript.contains("Imports= ./q/"));
    QVERIFY(runScript.contains("Translations= ./lolTr/"));
    QVERIFY(runScript.contains("Qml2Imports= ./q/"));

#ifdef Q_OS_WIN


    runTestParams({"-bin", bin, "clear" ,
                   "-binOut", "/lol",
                   "-libOut", "/lolLib",
                   "-trOut", "/lolTr",
                   "-pluginOut", "/p",
                   "-qmlOut", "/q",
                   "-qmake", qmake,
                   "-qmlDir", TestBinDir + "/../TestQMLWidgets"
                  }, &comapareTree);



    file.setFileName( "./" + DISTRO_DIR + "/TestQMLWidgets.bat");

    QVERIFY(file.open(QIODevice::ReadOnly));

    runScript = file.readAll();
    file.close();

    qDebug() << "runScript =" << runScript;

    QVERIFY(runScript.contains("SET BASE_DIR=%~dp0"));
    QVERIFY(runScript.contains("SET PATH=%BASE_DIR%\\lol\\;%BASE_DIR%\\lolLib\\;%PATH%"));
    QVERIFY(runScript.contains("start \"TestQMLWidgets\" /B \"%BASE_DIR%\\lol\\TestQMLWidgets.exe\" %*"));

    runTestParams({"-bin", TestBinDir + "TestOnlyC.exe", "clear",
                  }, nullptr);

    file.setFileName( "./" + DISTRO_DIR + "/TestOnlyC.bat");

    QVERIFY(file.open(QIODevice::ReadOnly));

    runScript = file.readAll();
    file.close();

    qDebug() << "runScript =" << runScript;

    QVERIFY(runScript.contains("call \"%BASE_DIR%\\TestOnlyC.exe\" %*"));

#endif

}

QTEST_APPLESS_MAIN(deploytest)

#include "tst_deploytest.moc"
