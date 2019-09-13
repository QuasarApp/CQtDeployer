/*
 * Copyright (C) 2018-2019 QuasarApp.
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
#include <cqt.h>
#include <QCryptographicHash>

#include <QMap>
#include <QByteArray>
#include <QDir>
#include <thread>
#include "libcreator.h"
#include "qmlcreator.h"
#include "testutils.h"
// add necessary includes here

const QString TestBinDir = TEST_BIN_DIR;
const QString TestQtDir = QT_BASE_DIR;

class deploytest : public QObject
{
    Q_OBJECT

private:
    QSet<QString> qtFilesTree;


    bool runProcess(const QString& DistroPath,
                    const QString& filename,
                    const QString &qt = "");
    QStringList getFilesFromDir(const QString& dir);


    void runTestParams(const QStringList &list, QSet<QString> *tree = nullptr);

public:
    deploytest();
    /**
     * @brief generateLib
     * @param paath
     * @return size of lib
     */
    int generateLib(const QString& paath);
    void deleteLib(const QString& paath);

//    bool mainTestOnlyC();
//    bool mainTestQMake();
//    bool mainTestQML();
//    bool testEnvIgnore();


    ~deploytest();

private slots:
    void initTestCase();
    void cleanupTestCase();

    // old tests (not valid)
    void testDeployTarget();
    void testStrip();
    void testExtractLib();

    void testQmlExtrct();
    void testSetTargetDir();

//    void mainTests();

    // end old tests

    // tested flags : help, version
    void testHelp();
    // tested flags clear noOvervrite
    void testOverwrite();

    // tested flags binDir
    void testBinDir();

    void testConfFile();

    void testQt();

    void testMSVC();

};

bool deploytest::runProcess(const QString &DistroPath,
                            const QString &filename,
                            const QString& qt) {

    QProcess p;

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    if (qt.size()) {
        auto val = env.value("LD_LIBRARY_PATH","").remove(qt);
        env.insert("LD_LIBRARY_PATH", val);

        val = env.value("PATH","").remove(qt);
        env.insert("PATH", val);

        env.insert("QTDIR", "");
    } else {
        env.clear();
        env.insert("QTDIR", "");

    }
    p.setProcessEnvironment(env);

#ifdef Q_OS_UNIX
    p.setProgram(DistroPath + "/" + filename + ".sh");
#else
    p.setProgram(DistroPath + "/" + filename + ".exe");
#endif

    p.start();

    if (!p.waitForFinished(10000)) {
        return false;
    }

    QString str = p.readAll();
    if (p.exitCode()) {
        qCritical() << p.errorString();
    }

    if (p.exitCode()) {
        qWarning() << "exitCode == " <<  p.exitCode();
    }

    if (str.contains("failed to load component", Qt::CaseInsensitive)
            || str.contains("is not installed", Qt::CaseInsensitive) ||
            str.contains("error", Qt::CaseInsensitive)) {
        return false;
    }

    return p.exitCode() == 0;
}

QStringList deploytest::getFilesFromDir(const QString &path) {
        QDir dir(path);

        QStringList res;

        auto list = dir.entryInfoList(QDir::Dirs| QDir::Files| QDir::NoDotAndDotDot);

        for (auto &&subDir: list) {

            if (subDir.isFile()) {
                res.push_back(subDir.fileName());
            } else {
                res.append(getFilesFromDir(subDir.absoluteFilePath()));
            }
        }

        return res;
}

//bool deploytest::testEnvIgnore()
//{
//#ifdef WITH_ALL_TESTS

//    QFileInfo QtDir = QFileInfo(QT_BASE_DIR);

//    if (!QtDir.isDir()) {
//        return false;
//    }

//    int argc = 9;
//    std::string qmakePath = (QtDir.absoluteFilePath() + "bin/qmake.exe").toStdString();
//#ifdef Q_OS_WIN
//    std::string qtPath = (QtDir.absoluteFilePath() + "bin/").toStdString();
//#else
//    std::string qtPath = (QtDir.absoluteFilePath() + "lib/").toStdString();
//#endif
//    const char *qmake = qmakePath.c_str();
//    const char *qt = qtPath.c_str();
//#ifdef Q_OS_WIN

//    const char * argv[] = {"./",
//                           "-bin", "./../../../tests/build/QtWidgetsProject.exe",
//                           "-qmake", qmake,
//                           "-ignoreEnv", qt,
//                           "-targetDir", "./Distro"};
//#else
//    const char * argv[] = {"./",
//                           "-bin", "./../../../tests/build/QtWidgetsProject",
//                           "-qmake", qmake,
//                           "-ignoreEnv", qt,

//                           "-targetDir", "./Distro"};
//#endif
//    if (!QuasarAppUtils::Params::parseParams(argc, argv)) {
//        return false;
//    }

//    Deploy deploy;

//    if (deploy.run()) {
//        return false;
//    }

//    if (!QFileInfo("./Distro").isDir()) {
//        return false;
//    }
//#ifdef Q_OS_WIN
//    QDir info("./Distro");

//#else
//    QDir info("./Distro/lib");
//#endif

//    for (auto &i :info.entryInfoList()) {
//        if (i.fileName().contains("Qt")) {
//            return false;
//        }

//    }
//    if (!info.removeRecursively()) {
//        return false;
//    }

//    return true;
//#else
//    return false;
//#endif
//}

deploytest::deploytest() {
    TestUtils utils;

    auto tempTree = utils.getTree(TestQtDir);

    for (const QString &i: tempTree) {
        qtFilesTree.insert(QFileInfo(i).fileName());
    }

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
    CQT *deploy = new CQT(&file);

    QStringList targets;
    targets << "./test/bins/execTarget.exe";
    QVERIFY(deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new CQT(&file);
    targets << "./test/bins/execTarget";
    QVERIFY(deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new CQT(&file);
    targets << "./test/bins/execTarget.exe" << "./test/bins/execTarget";
    QVERIFY(deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new CQT(&file);
    targets << "./test/bns/execTarget.exe";
    QVERIFY(!deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new CQT(&file);
    targets << "./test/bins/";
    QVERIFY(deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new CQT(&file);
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

    for (auto i: libList) {
        sizeBeforList.push_back(generateLib(i));
    }

    QList<qint64> sizeAfterList;

    deploy = new FileManager();
    QVERIFY(deploy->strip("./test/binTargetDir"));

    for(auto i: libList) {
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

    for (auto &&lib : libs) {
        QVERIFY(scaner.fillLibInfo(info, lib));
        QVERIFY(info.getName() == QFileInfo(lib).fileName());
        QVERIFY(info.getPath() == QFileInfo(lib).absolutePath());
        QVERIFY(info.fullPath() == QFileInfo(lib).absoluteFilePath());
        QVERIFY(info.getPlatform() == platforms.value(lib));

        for (auto &dep : deb.value(lib)) {
            bool test = info.getDependncies().contains(dep.toUpper());
            QVERIFY(test);
        }

    }

}

void deploytest::testMSVC() {
    QString testPath = "./Qt/5.11.2/msvc2017_64/bin/";
    QString testqmakepath = testPath +"qmake";

    QDir d;
    QDir oldDir("./Qt");
    oldDir.removeRecursively();
    QVERIFY(d.mkpath(testPath));

    QFile file(testqmakepath);

    QVERIFY(file.open(QIODevice::ReadWrite | QIODevice::Truncate));

    QVERIFY(file.write("test"));

    file.close();


    auto msvc = DeployCore::getMSVC(testqmakepath);

    QVERIFY(msvc & MSVCVersion::MSVC_17);
    QVERIFY(msvc & MSVCVersion::MSVC_x64);

    QVERIFY(file.remove());
    QDir dir("./Qt");
    dir.removeRecursively();


}

void deploytest::testQmlExtrct() {
    QmlCreator creator("./");
    auto imports = creator.getQmlImports();

    auto qmlFiles = creator.getCopyedQml();


    QML scaner("./");


    for (auto &&file : qmlFiles) {


        auto fileImports = scaner.extractImportsFromFile(file);

        for (auto &fil : imports.value(file)) {
            QVERIFY(fileImports.contains(fil, Qt::CaseInsensitive));
        }

    }
}

void deploytest::testSetTargetDir() {

    FileManager file;
    CQT  dep(&file);

    dep.setTargetDir();

    QVERIFY(dep.config()->targetDir == QFileInfo("./Distro").absoluteFilePath());
    dep.setTargetDir("./ff");
    QVERIFY(dep.config()->targetDir == QFileInfo("./ff").absoluteFilePath());

    int argc = 3;
    const char * argv[] = {"", "-targetDir", "./Distro2"};

    QuasarAppUtils::Params::parseParams(argc, argv);

    dep.setTargetDir();
    QVERIFY(dep.config()->targetDir == QFileInfo("./Distro2").absoluteFilePath());
    dep.setTargetDir("./ff");
    QVERIFY(dep.config()->targetDir == QFileInfo("./Distro2").absoluteFilePath());

}

void deploytest::runTestParams(const QStringList &list, QSet<QString>* tree) {

    QuasarAppUtils::Params::parseParams(list);

    Deploy deploy;
    QVERIFY(deploy.run() == 0);

    if (tree) {
        TestUtils utils;

        QVERIFY(DeployCore::_config);
        QVERIFY(!DeployCore::_config->targetDir.isEmpty());

        auto resultTree = utils.getTree(DeployCore::_config->targetDir);

        auto comapre = utils.compareTree(resultTree, *tree);

        if (comapre.size() != 0) {

            bool bug = false;

            for (auto i = comapre.begin(); i != comapre.end(); ++i) {

                if (i.value() == 1) {
                    qCritical() << "added unnecessary file : " + i.key();
                    bug = true;
                } else if (qtFilesTree.contains(QFileInfo(i.key()).fileName())) {
                    qCritical() << "Missing file : " + i.key();
                    bug = true;
                } else {
                    qWarning() << "File : " + i.key() + " not exits in qt Dir";
                }
            }

            if (!bug) {
                return;
            }

            QJsonObject obj;
            for (auto i : resultTree) {
                obj[i];
            }

            QJsonDocument doc(obj);

            QFile lasttree("./LastTree.json");
            lasttree.open(QIODevice::WriteOnly| QIODevice::Truncate);

            lasttree.write(doc.toJson());
            lasttree.close();

            QVERIFY2(false, "runTestParams fail");


        }

    }

}

void deploytest::testHelp() {
    runTestParams({"h"});
    runTestParams({"v"});
    auto help = DeployCore::help();

    for (auto &key: DeployCore::helpKeys()) {
        QVERIFY(help.contains(key));
    }

}

void deploytest::testOverwrite() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    auto comapareTree = utils.createTree(
                {"./Distro/bin/TestOnlyC",
                 "./Distro/TestOnlyC.sh"});
#else
    auto comapareTree = utils.createTree(
                {"./Distro/TestOnlyC.exe",
                 "./Distro/qt.conf"});
#endif

    runTestParams({"-bin", TestBinDir + "TestOnlyC", "force-clear", "noOverwrite"}, &comapareTree);


    QFile f("./Distro/bin/TestOnlyC");

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

    runTestParams({"-bin", TestBinDir + "TestOnlyC", "noOverwrite"}, &comapareTree);

    QVERIFY(f.open(QIODevice::ReadOnly));
    hashAfter = QCryptographicHash::hash(f.readAll(), QCryptographicHash::Md5);
    f.close();

    QVERIFY(hashAfter != hashBefor);


    runTestParams({"-bin", TestBinDir + "TestOnlyC"}, &comapareTree);

    QVERIFY(f.open(QIODevice::ReadOnly));
    hashAfter = QCryptographicHash::hash(f.readAll(), QCryptographicHash::Md5);
    f.close();

    QVERIFY(hashAfter == hashBefor);

}

void deploytest::testBinDir() {
    TestUtils utils;


#ifdef Q_OS_UNIX
    auto comapareTree = utils.createTree(
                {"./Distro/bin/TestOnlyC",
                 "./Distro/bin/QtWidgetsProject",
                 "./Distro/bin/TestQMLWidgets",
                 "./Distro/TestOnlyC.sh",
                 "./Distro/QtWidgetsProject.sh",
                 "./Distro/TestQMLWidgets.sh"});
#else
    auto comapareTree = utils.createTree(
                {"./Distro/TestOnlyC.exe",
                 "./Distro/QtWidgetsProject.exe",
                 "./Distro/TestQMLWidgets.exe",
                 "./Distro/qt.conf"});
#endif



    runTestParams({"-binDir", TestBinDir, "clear"}, &comapareTree);
}

void deploytest::testConfFile() {
    TestUtils utils;

    QFile::remove(TestBinDir + "/TestConf.json");

#ifdef Q_OS_UNIX
    auto comapareTree = utils.createTree(
                {"./Distro/bin/TestOnlyC",
                 "./Distro/bin/QtWidgetsProject",
                 "./Distro/bin/TestQMLWidgets",
                 "./Distro/TestOnlyC.sh",
                 "./Distro/QtWidgetsProject.sh",
                 "./Distro/TestQMLWidgets.sh"});
#else
    auto comapareTree = utils.createTree(
                {"./Distro/TestOnlyC.exe",
                 "./Distro/QtWidgetsProject.exe",
                 "./Distro/TestQMLWidgets.exe",
                 "./Distro/qt.conf"});
#endif


    runTestParams({"-bin", TestBinDir, "clear" ,
                   "-confFile", TestBinDir + "/TestConf.json"}, &comapareTree);


    QVERIFY(QFile::exists(TestBinDir + "/TestConf.json"));
    QFile::remove(TestBinDir + "/TestConf.json");

#ifdef Q_OS_UNIX
    runTestParams({"-bin", TestBinDir + "TestOnlyC," + TestBinDir + "QtWidgetsProject," + TestBinDir + "TestQMLWidgets",
                   "clear" ,
                   "-confFile", TestBinDir + "/TestConf.json"}, &comapareTree);
#else
    runTestParams({"-bin", TestBinDir + "TestOnlyC.exe," + TestBinDir + "QtWidgetsProject.exe," + TestBinDir + "TestQMLWidgets.exe",
                   "clear" ,
                   "-confFile", TestBinDir + "/TestConf.json"}, &comapareTree);
#endif

    QFile confFile(TestBinDir + "/TestConf.json");
    QVERIFY(confFile.open(QIODevice::ReadOnly));

    auto data = confFile.readAll();
    confFile.close();

    QJsonDocument doc;
    doc = doc.fromJson(data);
    QVERIFY(!doc.isNull());

    QVERIFY(data.contains("\"bin\": ["));
    QVERIFY(data.contains("build/TestOnlyC"));
    QVERIFY(data.contains("build/QtWidgetsProject"));
    QVERIFY(data.contains("build/TestQMLWidgets"));

    QVERIFY(data.contains("\"clear\": true"));

#ifdef Q_OS_UNIX
    runTestParams({"-confFile", TestBinDir + "/TestConf.json"}, &comapareTree);
#else
    runTestParams({"-confFile", TestBinDir + "/TestConf.json"}, &comapareTree);

#endif

    QVERIFY(QuasarAppUtils::Params::isEndable("clear"));
    QVERIFY(QuasarAppUtils::Params::isEndable("bin"));

    QFile::remove(TestBinDir + "/TestConf.json");
}

void deploytest::testQt() {
    TestUtils utils;


#ifdef Q_OS_UNIX
    auto comapareTree = utils.createTree(
    {
        "./Distro/TestQMLWidgets.sh",
        "./Distro/bin/TestQMLWidgets",
        "./Distro/lib/libQt5Core.so.5",
        "./Distro/lib/libQt5DBus.so.5",
        "./Distro/lib/libQt5EglFSDeviceIntegration.so.5",
        "./Distro/lib/libQt5Gui.so.5",
        "./Distro/lib/libQt5Network.so.5",
        "./Distro/lib/libQt5Qml.so.5",
        "./Distro/lib/libQt5Quick.so.5",
        "./Distro/lib/libQt5Svg.so.5",
        "./Distro/lib/libQt5VirtualKeyboard.so.5",
        "./Distro/lib/libQt5WaylandClient.so.5",
        "./Distro/lib/libQt5WaylandCompositor.so.5",
        "./Distro/lib/libQt5WebSockets.so.5",
        "./Distro/lib/libQt5Widgets.so.5",
        "./Distro/lib/libQt5XcbQpa.so.5",
        "./Distro/lib/libicudata.so.56",
        "./Distro/lib/libicui18n.so.56",
        "./Distro/lib/libicuuc.so.56",
        "./Distro/plugins/bearer/libqconnmanbearer.so",
        "./Distro/plugins/bearer/libqgenericbearer.so",
        "./Distro/plugins/bearer/libqnmbearer.so",
        "./Distro/plugins/iconengines/libqsvgicon.so",
        "./Distro/plugins/imageformats/libqgif.so",
        "./Distro/plugins/imageformats/libqicns.so",
        "./Distro/plugins/imageformats/libqico.so",
        "./Distro/plugins/imageformats/libqjpeg.so",
        "./Distro/plugins/imageformats/libqsvg.so",
        "./Distro/plugins/imageformats/libqtga.so",
        "./Distro/plugins/imageformats/libqtiff.so",
        "./Distro/plugins/imageformats/libqwbmp.so",
        "./Distro/plugins/imageformats/libqwebp.so",
        "./Distro/plugins/platforminputcontexts/libcomposeplatforminputcontextplugin.so",
        "./Distro/plugins/platforminputcontexts/libibusplatforminputcontextplugin.so",
        "./Distro/plugins/platforminputcontexts/libqtvirtualkeyboardplugin.so",
        "./Distro/plugins/platforms/libqeglfs.so",
        "./Distro/plugins/platforms/libqlinuxfb.so",
        "./Distro/plugins/platforms/libqminimal.so",
        "./Distro/plugins/platforms/libqminimalegl.so",
        "./Distro/plugins/platforms/libqoffscreen.so",
        "./Distro/plugins/platforms/libqvnc.so",
        "./Distro/plugins/platforms/libqwayland-egl.so",
        "./Distro/plugins/platforms/libqwayland-generic.so",
        "./Distro/plugins/platforms/libqwayland-xcomposite-egl.so",
        "./Distro/plugins/platforms/libqwayland-xcomposite-glx.so",
        "./Distro/plugins/platforms/libqwebgl.so",
        "./Distro/plugins/platforms/libqxcb.so",
        "./Distro/plugins/virtualkeyboard/libqtvirtualkeyboard_hangul.so",
        "./Distro/plugins/virtualkeyboard/libqtvirtualkeyboard_openwnn.so",
        "./Distro/plugins/virtualkeyboard/libqtvirtualkeyboard_pinyin.so",
        "./Distro/plugins/virtualkeyboard/libqtvirtualkeyboard_tcime.so",
        "./Distro/plugins/virtualkeyboard/libqtvirtualkeyboard_thai.so",
        "./Distro/plugins/wayland-decoration-client/libbradient.so",
        "./Distro/plugins/wayland-graphics-integration-client/libdmabuf-server.so",
        "./Distro/plugins/wayland-graphics-integration-client/libdrm-egl-server.so",
        "./Distro/plugins/wayland-graphics-integration-client/libqt-plugin-wayland-egl.so",
        "./Distro/plugins/wayland-graphics-integration-client/libshm-emulation-server.so",
        "./Distro/plugins/wayland-graphics-integration-client/libxcomposite-egl.so",
        "./Distro/plugins/wayland-graphics-integration-client/libxcomposite-glx.so",
        "./Distro/plugins/wayland-graphics-integration-server/libdmabuf-server.so",
        "./Distro/plugins/wayland-graphics-integration-server/libdrm-egl-server.so",
        "./Distro/plugins/wayland-graphics-integration-server/liblinux-dmabuf-unstable-v1.so",
        "./Distro/plugins/wayland-graphics-integration-server/libqt-plugin-wayland-egl.so",
        "./Distro/plugins/wayland-graphics-integration-server/libshm-emulation-server.so",
        "./Distro/plugins/wayland-graphics-integration-server/libwayland-eglstream-controller.so",
        "./Distro/plugins/wayland-graphics-integration-server/libxcomposite-egl.so",
        "./Distro/plugins/wayland-graphics-integration-server/libxcomposite-glx.so",
        "./Distro/plugins/wayland-shell-integration/libfullscreen-shell-v1.so",
        "./Distro/plugins/wayland-shell-integration/libivi-shell.so",
        "./Distro/plugins/wayland-shell-integration/libwl-shell.so",
        "./Distro/plugins/wayland-shell-integration/libxdg-shell-v5.so",
        "./Distro/plugins/wayland-shell-integration/libxdg-shell-v6.so",
        "./Distro/plugins/wayland-shell-integration/libxdg-shell.so",
        "./Distro/plugins/xcbglintegrations/libqxcb-egl-integration.so",
        "./Distro/plugins/xcbglintegrations/libqxcb-glx-integration.so",
        "./Distro/translations/qtbase_ar.qm",
        "./Distro/translations/qtbase_bg.qm",
        "./Distro/translations/qtbase_ca.qm",
        "./Distro/translations/qtbase_cs.qm",
        "./Distro/translations/qtbase_da.qm",
        "./Distro/translations/qtbase_de.qm",
        "./Distro/translations/qtbase_en.qm",
        "./Distro/translations/qtbase_es.qm",
        "./Distro/translations/qtbase_fi.qm",
        "./Distro/translations/qtbase_fr.qm",
        "./Distro/translations/qtbase_gd.qm",
        "./Distro/translations/qtbase_he.qm",
        "./Distro/translations/qtbase_hu.qm",
        "./Distro/translations/qtbase_it.qm",
        "./Distro/translations/qtbase_ja.qm",
        "./Distro/translations/qtbase_ko.qm",
        "./Distro/translations/qtbase_lv.qm",
        "./Distro/translations/qtbase_pl.qm",
        "./Distro/translations/qtbase_ru.qm",
        "./Distro/translations/qtbase_sk.qm",
        "./Distro/translations/qtbase_uk.qm",
        "./Distro/translations/qtbase_zh_TW.qm",
        "./Distro/translations/qtdeclarative_bg.qm",
        "./Distro/translations/qtdeclarative_da.qm",
        "./Distro/translations/qtdeclarative_de.qm",
        "./Distro/translations/qtdeclarative_en.qm",
        "./Distro/translations/qtdeclarative_es.qm",
        "./Distro/translations/qtdeclarative_fi.qm",
        "./Distro/translations/qtdeclarative_fr.qm",
        "./Distro/translations/qtdeclarative_hu.qm",
        "./Distro/translations/qtdeclarative_ja.qm",
        "./Distro/translations/qtdeclarative_ko.qm",
        "./Distro/translations/qtdeclarative_lv.qm",
        "./Distro/translations/qtdeclarative_pl.qm",
        "./Distro/translations/qtdeclarative_ru.qm",
        "./Distro/translations/qtdeclarative_sk.qm",
        "./Distro/translations/qtdeclarative_uk.qm"
    });
    QString bin = TestBinDir + "TestQMLWidgets";
    QString qmake = TestQtDir + "bin/qmake";

#else
    auto comapareTree = utils.createTree(
                {"./Distro/TestQMLWidgets.exe",
                 "./Distro/qt.conf"});
    QString bin = TestBinDir + "TestQMLWidgets.exe";
    QString qmake = TestQtDir + "bin/qmake.exe";

#endif



    runTestParams({"-bin", bin, "clear" ,
                  "-qmake", qmake,
                  "-qnlDir", TestBinDir + "/../TestQMLWidgets"}, &comapareTree);

#ifdef Q_OS_UNIX
    comapareTree = utils.createTree(
    {
        "./Distro/QtWidgetsProject.sh",
        "./Distro/bin/QtWidgetsProject",
        "./Distro/lib/libQt5Core.so",
        "./Distro/lib/libQt5DBus.so",
        "./Distro/lib/libQt5EglFSDeviceIntegration.so",
        "./Distro/lib/libQt5Gui.so",
        "./Distro/lib/libQt5Network.so",
        "./Distro/lib/libQt5Qml.so",
        "./Distro/lib/libQt5Quick.so",
        "./Distro/lib/libQt5Svg.so",
        "./Distro/lib/libQt5VirtualKeyboard.so",
        "./Distro/lib/libQt5WaylandClient.so",
        "./Distro/lib/libQt5WaylandCompositor.so",
        "./Distro/lib/libQt5WebSockets.so",
        "./Distro/lib/libQt5Widgets.so",
        "./Distro/lib/libQt5XcbQpa.so",
        "./Distro/lib/libicudata.so",
        "./Distro/lib/libicui18n.so",
        "./Distro/lib/libicuuc.so",
        "./Distro/plugins/iconengines/libqsvgicon.so",
        "./Distro/plugins/imageformats/libqgif.so",
        "./Distro/plugins/imageformats/libqicns.so",
        "./Distro/plugins/imageformats/libqico.so",
        "./Distro/plugins/imageformats/libqjpeg.so",
        "./Distro/plugins/imageformats/libqsvg.so",
        "./Distro/plugins/imageformats/libqtga.so",
        "./Distro/plugins/imageformats/libqtiff.so",
        "./Distro/plugins/imageformats/libqwbmp.so",
        "./Distro/plugins/imageformats/libqwebp.so",
        "./Distro/plugins/platforminputcontexts/libcomposeplatforminputcontextplugin.so",
        "./Distro/plugins/platforminputcontexts/libibusplatforminputcontextplugin.so",
        "./Distro/plugins/platforminputcontexts/libqtvirtualkeyboardplugin.so",
        "./Distro/plugins/platforms/libqeglfs.so",
        "./Distro/plugins/platforms/libqlinuxfb.so",
        "./Distro/plugins/platforms/libqminimal.so",
        "./Distro/plugins/platforms/libqminimalegl.so",
        "./Distro/plugins/platforms/libqoffscreen.so",
        "./Distro/plugins/platforms/libqvnc.so",
        "./Distro/plugins/platforms/libqwayland-egl.so",
        "./Distro/plugins/platforms/libqwayland-generic.so",
        "./Distro/plugins/platforms/libqwayland-xcomposite-egl.so",
        "./Distro/plugins/platforms/libqwayland-xcomposite-glx.so",
        "./Distro/plugins/platforms/libqwebgl.so",
        "./Distro/plugins/platforms/libqxcb.so",
        "./Distro/plugins/virtualkeyboard/libqtvirtualkeyboard_hangul.so",
        "./Distro/plugins/virtualkeyboard/libqtvirtualkeyboard_openwnn.so",
        "./Distro/plugins/virtualkeyboard/libqtvirtualkeyboard_pinyin.so",
        "./Distro/plugins/virtualkeyboard/libqtvirtualkeyboard_tcime.so",
        "./Distro/plugins/virtualkeyboard/libqtvirtualkeyboard_thai.so",
        "./Distro/plugins/wayland-decoration-client/libbradient.so",
        "./Distro/plugins/wayland-graphics-integration-client/libdmabuf-server.so",
        "./Distro/plugins/wayland-graphics-integration-client/libdrm-egl-server.so",
        "./Distro/plugins/wayland-graphics-integration-client/libqt-plugin-wayland-egl.so",
        "./Distro/plugins/wayland-graphics-integration-client/libshm-emulation-server.so",
        "./Distro/plugins/wayland-graphics-integration-client/libxcomposite-egl.so",
        "./Distro/plugins/wayland-graphics-integration-client/libxcomposite-glx.so",
        "./Distro/plugins/wayland-graphics-integration-server/libdmabuf-server.so",
        "./Distro/plugins/wayland-graphics-integration-server/libdrm-egl-server.so",
        "./Distro/plugins/wayland-graphics-integration-server/liblinux-dmabuf-unstable-v1.so",
        "./Distro/plugins/wayland-graphics-integration-server/libqt-plugin-wayland-egl.so",
        "./Distro/plugins/wayland-graphics-integration-server/libshm-emulation-server.so",
        "./Distro/plugins/wayland-graphics-integration-server/libwayland-eglstream-controller.so",
        "./Distro/plugins/wayland-graphics-integration-server/libxcomposite-egl.so",
        "./Distro/plugins/wayland-graphics-integration-server/libxcomposite-glx.so",
        "./Distro/plugins/wayland-shell-integration/libfullscreen-shell-v1.so",
        "./Distro/plugins/wayland-shell-integration/libivi-shell.so",
        "./Distro/plugins/wayland-shell-integration/libwl-shell.so",
        "./Distro/plugins/wayland-shell-integration/libxdg-shell-v5.so",
        "./Distro/plugins/wayland-shell-integration/libxdg-shell-v6.so",
        "./Distro/plugins/wayland-shell-integration/libxdg-shell.so",
        "./Distro/plugins/xcbglintegrations/libqxcb-egl-integration.so",
        "./Distro/plugins/xcbglintegrations/libqxcb-glx-integration.so",
        "./Distro/translations/qtbase_ar.qm",
        "./Distro/translations/qtbase_bg.qm",
        "./Distro/translations/qtbase_ca.qm",
        "./Distro/translations/qtbase_cs.qm",
        "./Distro/translations/qtbase_da.qm",
        "./Distro/translations/qtbase_de.qm",
        "./Distro/translations/qtbase_en.qm",
        "./Distro/translations/qtbase_es.qm",
        "./Distro/translations/qtbase_fi.qm",
        "./Distro/translations/qtbase_fr.qm",
        "./Distro/translations/qtbase_gd.qm",
        "./Distro/translations/qtbase_he.qm",
        "./Distro/translations/qtbase_hu.qm",
        "./Distro/translations/qtbase_it.qm",
        "./Distro/translations/qtbase_ja.qm",
        "./Distro/translations/qtbase_ko.qm",
        "./Distro/translations/qtbase_lv.qm",
        "./Distro/translations/qtbase_pl.qm",
        "./Distro/translations/qtbase_ru.qm",
        "./Distro/translations/qtbase_sk.qm",
        "./Distro/translations/qtbase_uk.qm",
        "./Distro/translations/qtbase_zh_TW.qm",
        "./Distro/translations/qtdeclarative_bg.qm",
        "./Distro/translations/qtdeclarative_da.qm",
        "./Distro/translations/qtdeclarative_de.qm",
        "./Distro/translations/qtdeclarative_en.qm",
        "./Distro/translations/qtdeclarative_es.qm",
        "./Distro/translations/qtdeclarative_fi.qm",
        "./Distro/translations/qtdeclarative_fr.qm",
        "./Distro/translations/qtdeclarative_hu.qm",
        "./Distro/translations/qtdeclarative_ja.qm",
        "./Distro/translations/qtdeclarative_ko.qm",
        "./Distro/translations/qtdeclarative_lv.qm",
        "./Distro/translations/qtdeclarative_pl.qm",
        "./Distro/translations/qtdeclarative_ru.qm",
        "./Distro/translations/qtdeclarative_sk.qm",
        "./Distro/translations/qtdeclarative_uk.qm"
    });
    bin = TestBinDir + "QtWidgetsProject";

#else
    auto comapareTree = utils.createTree(
                {"./Distro/TestQMLWidgets.exe",
                 "./Distro/qt.conf"});
    QString bin = TestBinDir + "QtWidgetsProject.exe";

#endif

    runTestParams({"-bin", bin, "clear" ,
                  "-qmake", qmake}, &comapareTree);


#ifdef Q_OS_UNIX
    comapareTree = utils.createTree(
    {
        "./Distro/QtWidgetsProject.sh",
        "./Distro/bin/QtWidgetsProject",
        "./Distro/lib/libQt5Core.so",
        "./Distro/lib/libQt5DBus.so",
        "./Distro/lib/libQt5EglFSDeviceIntegration.so",
        "./Distro/lib/libQt5Gui.so",
        "./Distro/lib/libQt5Network.so",
        "./Distro/lib/libQt5Qml.so",
        "./Distro/lib/libQt5Quick.so",
        "./Distro/lib/libQt5Svg.so",
        "./Distro/lib/libQt5VirtualKeyboard.so",
        "./Distro/lib/libQt5WaylandClient.so",
        "./Distro/lib/libQt5WaylandCompositor.so",
        "./Distro/lib/libQt5WebSockets.so",
        "./Distro/lib/libQt5Widgets.so",
        "./Distro/lib/libQt5XcbQpa.so",
        "./Distro/lib/libicudata.so",
        "./Distro/lib/libicui18n.so",
        "./Distro/lib/libicuuc.so",
        "./Distro/plugins/iconengines/libqsvgicon.so",
        "./Distro/plugins/imageformats/libqgif.so",
        "./Distro/plugins/imageformats/libqicns.so",
        "./Distro/plugins/imageformats/libqico.so",
        "./Distro/plugins/imageformats/libqjpeg.so",
        "./Distro/plugins/imageformats/libqsvg.so",
        "./Distro/plugins/imageformats/libqtga.so",
        "./Distro/plugins/imageformats/libqtiff.so",
        "./Distro/plugins/imageformats/libqwbmp.so",
        "./Distro/plugins/imageformats/libqwebp.so",
        "./Distro/plugins/platforminputcontexts/libcomposeplatforminputcontextplugin.so",
        "./Distro/plugins/platforminputcontexts/libibusplatforminputcontextplugin.so",
        "./Distro/plugins/platforminputcontexts/libqtvirtualkeyboardplugin.so",
        "./Distro/plugins/platforms/libqeglfs.so",
        "./Distro/plugins/platforms/libqlinuxfb.so",
        "./Distro/plugins/platforms/libqminimal.so",
        "./Distro/plugins/platforms/libqminimalegl.so",
        "./Distro/plugins/platforms/libqoffscreen.so",
        "./Distro/plugins/platforms/libqvnc.so",
        "./Distro/plugins/platforms/libqwayland-egl.so",
        "./Distro/plugins/platforms/libqwayland-generic.so",
        "./Distro/plugins/platforms/libqwayland-xcomposite-egl.so",
        "./Distro/plugins/platforms/libqwayland-xcomposite-glx.so",
        "./Distro/plugins/platforms/libqwebgl.so",
        "./Distro/plugins/platforms/libqxcb.so",
        "./Distro/plugins/virtualkeyboard/libqtvirtualkeyboard_hangul.so",
        "./Distro/plugins/virtualkeyboard/libqtvirtualkeyboard_openwnn.so",
        "./Distro/plugins/virtualkeyboard/libqtvirtualkeyboard_pinyin.so",
        "./Distro/plugins/virtualkeyboard/libqtvirtualkeyboard_tcime.so",
        "./Distro/plugins/virtualkeyboard/libqtvirtualkeyboard_thai.so",
        "./Distro/plugins/wayland-decoration-client/libbradient.so",
        "./Distro/plugins/wayland-graphics-integration-client/libdmabuf-server.so",
        "./Distro/plugins/wayland-graphics-integration-client/libdrm-egl-server.so",
        "./Distro/plugins/wayland-graphics-integration-client/libqt-plugin-wayland-egl.so",
        "./Distro/plugins/wayland-graphics-integration-client/libshm-emulation-server.so",
        "./Distro/plugins/wayland-graphics-integration-client/libxcomposite-egl.so",
        "./Distro/plugins/wayland-graphics-integration-client/libxcomposite-glx.so",
        "./Distro/plugins/wayland-graphics-integration-server/libdmabuf-server.so",
        "./Distro/plugins/wayland-graphics-integration-server/libdrm-egl-server.so",
        "./Distro/plugins/wayland-graphics-integration-server/liblinux-dmabuf-unstable-v1.so",
        "./Distro/plugins/wayland-graphics-integration-server/libqt-plugin-wayland-egl.so",
        "./Distro/plugins/wayland-graphics-integration-server/libshm-emulation-server.so",
        "./Distro/plugins/wayland-graphics-integration-server/libwayland-eglstream-controller.so",
        "./Distro/plugins/wayland-graphics-integration-server/libxcomposite-egl.so",
        "./Distro/plugins/wayland-graphics-integration-server/libxcomposite-glx.so",
        "./Distro/plugins/wayland-shell-integration/libfullscreen-shell-v1.so",
        "./Distro/plugins/wayland-shell-integration/libivi-shell.so",
        "./Distro/plugins/wayland-shell-integration/libwl-shell.so",
        "./Distro/plugins/wayland-shell-integration/libxdg-shell-v5.so",
        "./Distro/plugins/wayland-shell-integration/libxdg-shell-v6.so",
        "./Distro/plugins/wayland-shell-integration/libxdg-shell.so",
        "./Distro/plugins/xcbglintegrations/libqxcb-egl-integration.so",
        "./Distro/plugins/xcbglintegrations/libqxcb-glx-integration.so",
    });
    bin = TestBinDir + "QtWidgetsProject";

#else
    auto comapareTree = utils.createTree(
                {"./Distro/TestQMLWidgets.exe",
                 "./Distro/qt.conf"});
    QString bin = TestBinDir + "QtWidgetsProject.exe";

#endif

    runTestParams({"-bin", bin, "clear" ,
                  "-qmake", qmake, "noTranslations"}, &comapareTree);

}

//bool deploytest::mainTestOnlyC() {
//#ifdef WITH_ALL_TESTS
//    int argc = 5;
//#ifdef Q_OS_WIN

//    QFileInfo QtDir = QFileInfo(QT_BASE_DIR);

//    if (!QtDir.isDir()) {
//        return false;
//    }
//    argc += 2;

//    std::string path = (QtDir.absoluteFilePath() + "bin/").toStdString();
//    const char *string = path.c_str();

//    const char * argv[] = {"./",
//                           "-bin", "./../../../tests/build/TestOnlyC.exe",
//                           "-targetDir", "./Distro",
//                           "-libDir", string};
//#else
//    const char * argv[] = {"./",
//                           "-bin", "./../../../tests/build/TestOnlyC",
//                           "-targetDir", "./Distro"};
//#endif

//    if (!QuasarAppUtils::Params::parseParams(argc, argv)) {
//        return false;
//    }

//    Deploy deploy;

//    if (deploy.run()) {
//        return false;
//    }

//    if (!QFileInfo("./Distro").isDir()) {
//        return false;
//    }

//    QDir info("./Distro");

//    bool run = runProcess("./Distro", "TestOnlyC");
//    if (!info.removeRecursively()) {
//        return false;
//    }

//    return run;
//#else
//    return false;

//#endif
//}

//bool deploytest::mainTestQMake() {
//#ifdef WITH_ALL_TESTS

//    QFileInfo QtDir = QFileInfo(QT_BASE_DIR);

//    if (!QtDir.isDir()) {
//        return false;
//    }

//    int argc = 7;
//#ifdef Q_OS_WIN

//    std::string qmakePath = (QtDir.absoluteFilePath() + "bin/qmake.exe").toStdString();
//    const char *qmake = qmakePath.c_str();
//    const char * argv[] = {"./",
//                           "-bin", "./../../../tests/build/QtWidgetsProject.exe",
//                           "-qmake", qmake,
//                           "-targetDir", "./Distro"};
//#else
//    const char * argv[] = {"./",
//                           "-bin", "./../../../tests/build/QtWidgetsProject",
//                           "-qmake", (QtDir.absoluteFilePath() + "/bin/qmake").toLatin1(),
//                           "-targetDir", "./Distro"};
//#endif
//    if (!QuasarAppUtils::Params::parseParams(argc, argv)) {
//        return false;
//    }

//    Deploy deploy;

//    if (deploy.run()) {
//        return false;
//    }

//    QDir info("./Distro");

//    if (!QFileInfo("./Distro").isDir()) {
//        return false;
//    }

//    bool run = runProcess("./Distro", "QtWidgetsProject", QtDir.absoluteFilePath());

//    if (!info.removeRecursively()) {
//        return false;
//    }

//    return run;
//#else
//    return false;
//#endif



//}

//bool deploytest::mainTestQML() {

//#ifdef WITH_ALL_TESTS

//    QFileInfo QtDir = QFileInfo(QT_BASE_DIR);

//    if (!QtDir.isDir()) {
//        return false;
//    }

//    int argc = 9;
//#ifdef Q_OS_WIN
//    std::string qmakePath = (QtDir.absoluteFilePath() + "bin/qmake.exe").toStdString();
//    const char *qmake = qmakePath.c_str();

//    const char * argv[] = {"./",
//                           "-bin", "./../../../tests/build/TestQMLWidgets.exe",
//                           "-qmlDir", "./../../../tests/TestQMLWidgets",
//                           "-qmake", qmake,
//                           "-targetDir", "./Distro"};
//#else
//    const char * argv[] = {"./",
//                           "-bin", "./../../../tests/build/TestQMLWidgets",
//                           "-qmlDir", "./../../../tests/TestQMLWidgets",
//                           "-qmake", (QtDir.absoluteFilePath() + "/bin/qmake").toLatin1(),
//                           "-targetDir", "./Distro"};
//#endif


//    if (!QuasarAppUtils::Params::parseParams(argc, argv)) {
//        return false;
//    }

//    Deploy deploy;

//    if (deploy.run()) {
//        return false;
//    }

//    QDir info("./Distro");

//    bool run = runProcess("./Distro", "TestQMLWidgets", QtDir.absoluteFilePath());

//    if (!info.removeRecursively()) {
//        return false;
//    }

//    if (!run ) {
//        return false;
//    }

//    if (!QFileInfo(QtDir).isDir()) {
//        return false;
//    }

//    argc = 10;
//#ifdef Q_OS_WIN

//    const char * argv2[] = {"./",
//                           "-bin", "./../../../tests/build/TestQMLWidgets.exe",
//                           "-qmlDir", "./../../../tests/TestQMLWidgets",
//                           "-qmake", qmake,
//                           "-targetDir", "./Distro", "qmlExtern"};
//#else
//    const char * argv2[] = {"./",
//                           "-bin", "./../../../tests/build/TestQMLWidgets",
//                           "-qmlDir", "./../../../tests/TestQMLWidgets",
//                           "-qmake", (QtDir.absoluteFilePath() + "/bin/qmake").toLatin1(),
//                           "-targetDir", "./Distro", "qmlExtern"};
//#endif


//    if (!QuasarAppUtils::Params::parseParams(argc, argv2)) {
//        return false;
//    }

//    Deploy deploy2;

//    if (deploy.run()) {
//        return false;
//    }
//    run = runProcess("./Distro", "TestQMLWidgets", QtDir.absoluteFilePath());

//    if (!info.removeRecursively()) {
//        return false;
//    }

//    return run;
//#else
//    return false;

//#endif
//}

QTEST_APPLESS_MAIN(deploytest)

#include "tst_deploytest.moc"
