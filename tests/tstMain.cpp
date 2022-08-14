//#
//# Copyright (C) 2020-2022 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#include <QtTest>
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
#include "MSVCtest.h"
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
#include "qttest.h"
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

// Use This macros for initialize your own test classes.
// Check exampletests
#define TestCase(name, testClass) \
    void name() { \
        initTest(new testClass()); \
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
        TestCase(MSVCtest, MSVCTest )
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
        TestCase(qttest, QtTest )
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
        TestCase(systemLibTest, SystemLibTest)
    // END TEST CASES

private:

    /**
     * @brief initTest This method prepare @a test for run in the QApplication loop.
     * @param test are input test case class.
     */
    void initTest(Test* test);

    QCoreApplication *_app = nullptr;
};

/**
 * @brief tstMain::tstMain
 * init all availabel units for testsing
 */
tstMain::tstMain() {

    // init xample unit test
    int argc =0;
    char * argv[] = {nullptr};

    _app = new QCoreApplication(argc, argv);
    QCoreApplication::setApplicationName("testCQtDeployer");
    QCoreApplication::setOrganizationName("QuasarApp");

    auto path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    QDir(path).removeRecursively();

}

tstMain::~tstMain() {
    _app->exit(0);
    delete _app;
}

void tstMain::initTest(Test *test) {
    QTimer::singleShot(0, this, [this, test]() {
        test->test();
        delete test;
        _app->exit(0);
    });

    _app->exec();
}

QTEST_APPLESS_MAIN(tstMain)

#include "tstMain.moc"