/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include <QtTest>
#include <QCryptographicHash>
#include <QStorageInfo>


#include <QMap>
#include <QByteArray>
#include <QDir>
#include <thread>

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
