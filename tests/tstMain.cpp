//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#include <QtTest>
#include "qttest.h"

#ifdef Q_OS_LINUX
#include "extradependstest.h"

#include "allowemptypackagestest.h"
#include "binprefixtest.h"
#include "checkqttest.h"
#include "cleartest.h"
#include "confifiletest.h"
#include "customplatformtest.h"
#include "customscripttest.h"
#include "customtest.h"
#include "debcustomtest.h"
#include "debmultitest.h"
#include "debtest.h"
#include "dependencymaptest.h"
#include "deploygeneralfilestest.h"
#include "deploytargettest.h"
#include "disablerunscripttest.h"
#include "disableshortcutstest.h"
#include "distrostructtest.h"
#include "emptypackagestest.h"
#include "extractlibtest.h"
#include "extradatatest.h"
#include "extrapluginstest.h"
#include "iconstest.h"
#include "ignoreenvtest.h"
#include "ignoreenvwithlibdirtest.h"
#include "ignoretest.h"
#include "inittest.h"
#include "installdiroptionstest.h"
#include "libdirstest.h"
#include "md5test.h"
#include "multipackingtest.h"
#include "outdirtest.h"
#include "overridingtemplatedebtest.h"
#include "overwritetest.h"
#include "overwritewithpackingtest.h"
#include "packagestest.h"
#include "pathutilstest.h"
#include "prefixtest.h"
#include "qifwachiveformattest.h"
#include "qifwbinarycreatortest.h"
#include "qifwouttest.h"
#include "qifwresourcestest.h"
#include "QIFWtest.h"
#include "qmlextracttest.h"
#include "qmlscanertest.h"
#include "releativelinktest.h"
#include "runscriptstest.h"
#include "settargetdirtest.h"
#include "striptest.h"
#include "targetdirtest.h"
#include "trtest.h"
#include "virtualkeybordtest.h"
#include "webenginetest.h"
#include "ziparrchivetest.h"
#include "zipmultitest.h"
#include "ziptest.h"
#include "systemlibtest.h"
#include "qmlfiledialogtest.h"
#endif

#ifdef Q_OS_WIN
#include "MSVCtest.h"
#endif

// Use This macros for initialize your own test classes.
// Check exampletests
#define TestCase(name, testClass) \
    void name() { \
        initTest(new testClass); \
    }

/**
 * @brief The tstMain class - this is main test class
 */
class tstMain : public QObject
{
    Q_OBJECT


public:
    tstMain();

    ~tstMain();

private slots:
    // BEGIN TESTS CASES

    // General tests for all platforms
        TestCase(qttest, QtTest )

    // main tests works on linux only
#ifdef Q_OS_LINUX
        TestCase(extraDependsTest, ExtraDependsTest)
        TestCase(allowemptypackagestest, AllowEmptyPackagesTest )
        TestCase(binprefixtest,  BinPrefixTest)
        TestCase(checkqttest,  CheckQtTest)
        TestCase(cleartest, ClearTest )
        TestCase(confifiletest,  ConfFileTest)
        TestCase(customplatformtest,  CustomPlatformTest)
        TestCase(customscripttest,  CustomScriptTest)
        TestCase(customtest,  CustomTest)
        TestCase(debcustomtest,  DEBCustomTest)
        TestCase(debmultitest,  DEBMultiTest)
        TestCase(debtest,  DEBTest)
        TestCase(dependencymaptest,  DependencyMapTest)
        TestCase(deploygeneralfilestest,  DeployGeneralFilesTest)
        TestCase(deploytargettest,  DeployTargetTest)
        TestCase(disablerunscripttest, DisableRunScriptTest )
        TestCase(disableshortcutstest,  DisableShortcutsTest)
        TestCase(distrostructtest, DistroStructTest )
        TestCase(emptypackagestest,  EmptyPackagesTest)
        TestCase(extractlibtest,  ExtractLibTest)
        TestCase(extradatatest, ExtraDataTest )
        TestCase(extrapluginstest,  ExtraPluginTest)
        TestCase(iconstest, IconsTest )
        TestCase(ignoreenvtest, IgnoreEnvTest )
        TestCase(ignoreenvwithlibdirtest, IgnoreEnvWithLibDirTest )
        TestCase(ignoretest, IgnoreTest )
        TestCase(inittest, InitTest )
        TestCase(installdiroptionstest, InstallDirsOptionsTest )
        TestCase(libdirstest, LibDirTest )
        TestCase(md5test, MD5Test )
        TestCase(multipackingtest, MultiPackingTest )
        TestCase(outdirtest, OutDirTest )
        TestCase(overridingtemplatedebtest, OverridingDefaultTemplateDEBTest )
        TestCase(overridingtemplateqifwtest, OverridingDefaultTemplateDEBTest )
        TestCase(overwritetest, OverwiriteTest )
        TestCase(overwritewithpackingtest, OverwiriteWithPackingTest )
        TestCase(packagestest, PacakgesTest )
        TestCase(pathutilstest, PathUtilsTest )
        TestCase(prefixtest, PrefixTest )
        TestCase(qifwachiveformattest, QIFWArchiveFormatTest )
        TestCase(qifwbinarycreatortest, QIFWBinaryCreatorTest )
        TestCase(qifwouttest, QIFWOutTest )
        TestCase(qifwresourcestest, QIFWResourcesTest )
        TestCase(QIFWtest, QIFWTest )
        TestCase(qmlextracttest, QMLExtractTest )
        TestCase(qmlscanertest, QmlScanerTest )
        TestCase(releativelinktest, ReleativeLinkTest )
        TestCase(runscriptstest, RunScriptsTest )
        TestCase(settargetdirtest, SetTargetDirTest )
        TestCase(striptest, StripTest )
        TestCase(targetdirtest, TargetDirTest )
        TestCase(trtest, TrTest )
        TestCase(virtualkeybordtest, VirtualKeyBoardTest )
        TestCase(webenginetest, WebEngineTest )
        TestCase(ziparrchivetest, ZIPArchiveTest )
        TestCase(zipmultitest, ZIPMultiTest )
        TestCase(ziptest, ZIPTest )
        TestCase(qmlfiledialogtest, QmlFileDialogTest)
        TestCase(systemLibTest, SystemLibTest)


#endif

//     platform tests
#ifdef Q_OS_WIN
        TestCase(MSVCtest, MSVCTest )
#endif
    // END TEST CASES

private:
    void initTestCase();
    void cleanupTestCase();


    /**
     * @brief initTest This method prepare @a test for run in the QApplication loop.
     * @param test are input test case class.
     */
    void initTest(Test* test);

    QCoreApplication *_app = nullptr;
};

void tstMain::initTestCase() {
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

void tstMain::cleanupTestCase() {
    QDir qt("./test");
    qt.removeRecursively();
    auto originalPath = qgetenv("CQT_TEST_ORIGINAL_PATH");
    if (!originalPath.isEmpty()) {
        qputenv("PATH", originalPath);
    }
}

/**
 * @brief tstMain::tstMain
 * init all availabel units for testsing
 */
tstMain::tstMain() {
    auto originalPath = qgetenv("CQT_TEST_ORIGINAL_PATH");
    if (originalPath.isEmpty()) {
        qputenv("CQT_TEST_ORIGINAL_PATH", qgetenv("PATH"));
    }

    qputenv("QTEST_FUNCTION_TIMEOUT", "1800000");

    QString cqtTestPath = QString(QT_BASE_DIR) + "/bin/" +
                          DeployCore::getEnvSeparator() +
                          qgetenv("CQT_TEST_ORIGINAL_PATH");
    cqtTestPath = cqtTestPath +
                  DeployCore::getEnvSeparator() +
                  QT_BASE_DIR + "/../../Tools/QtInstallerFramework/4.0/bin/";
    cqtTestPath = cqtTestPath +
                  DeployCore::getEnvSeparator() +
                  QT_BASE_DIR + "/../../Tools/QtInstallerFramework/4.1/bin/";
    cqtTestPath = cqtTestPath +
                  DeployCore::getEnvSeparator() +
                  QT_BASE_DIR + "/../../Tools/QtInstallerFramework/4.2/bin/";
    cqtTestPath = cqtTestPath +
                  DeployCore::getEnvSeparator() +
                  QT_BASE_DIR + "/../../Tools/QtInstallerFramework/4.3/bin/";
    cqtTestPath = cqtTestPath +
                  DeployCore::getEnvSeparator() +
                  QT_BASE_DIR + "/../../Tools/QtInstallerFramework/4.4/bin/";
    cqtTestPath = cqtTestPath +
                  DeployCore::getEnvSeparator() +
                  QT_BASE_DIR + "/../../Tools/QtInstallerFramework/4.5/bin/";
    cqtTestPath = cqtTestPath +
                  DeployCore::getEnvSeparator() +
                  QT_BASE_DIR + "/../../Tools/QtInstallerFramework/4.6/bin/";
    qputenv("PATH", cqtTestPath.toLatin1().data());
    TestUtils utils;

    auto &filesTree = *FilesTreeService::autoInstance();
    filesTree += utils.getFilesSet(QT_BASE_DIR);
    // init xample unit test
    int argc =0;
    char * argv[] = {nullptr};

    _app = new QCoreApplication(argc, argv);
    QCoreApplication::setApplicationName("testCQtDeployer");
    QCoreApplication::setOrganizationName("QuasarApp");

    auto path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    QDir(path).removeRecursively();

    initTestCase();
}

tstMain::~tstMain() {
    _app->exit(0);
    delete _app;
    cleanupTestCase();
}

void tstMain::initTest(Test *test) {

    QTimer::singleShot(0, this, [this, test]() {
        try {
            test->test();
        } catch (std::exception *e) {
            QVERIFY2(false , e->what());
            delete e;
        }

        _app->exit(0);
    });

    _app->exec();

    delete test;
}

QTEST_APPLESS_MAIN(tstMain)

#include "tstMain.moc"
