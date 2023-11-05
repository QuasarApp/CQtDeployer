//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "checkqttest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void CheckQtTest::test() {

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

    for (const auto &i: std::as_const(cases)) {
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

    for (const auto &i: std::as_const(cases)) {
        auto dexription = QString("The isQtLib(%0) function should be return %1").arg(
                    i.first).arg(i.second);
        QVERIFY2(DeployCore::isQtLib(i.first) == i.second, dexription.toLatin1().data());
    }

    delete deployer;
}
