//
// Copyright (C) 2026 QuasarApp.
// Distributed under the GPLv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//
// Fast unit tests (issue #861).
//
// This executable is a plain QTest runner. Unlike the integration
// tests (CQtDeployer-tests) it does not deploy any sample application
// and does not require a full Qt installation, qmake or Qt Installer
// Framework. All tests are fast, self-contained and are executed by
// CTest (see CMakeLists.txt in this folder).
//

#include <QCoreApplication>
#include <QtTest>

#include "settings.h"
#include "deployconfigtest.h"
#include "distromoduletest.h"
#include "distrostructtest.h"
#include "envirementtest.h"
#include "filemanagertest.h"
#include "ignoreruletest.h"
#include "libinfotest.h"
#include "pathutilstest.h"
#include "petest.h"
#include "targetinfotest.h"
#include "zipcompressertest.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("CQtDeployer-unittests");
    QCoreApplication::setOrganizationName("QuasarApp");

    // Initialize the Settings service, it is needed by the
    // FileManager::saveDeploymendFiles/loadDeployemendFiles tests.
    (void)QuasarAppUtils::Settings::autoInstance();

    int result = 0;

    {
        PathUtilsTest test;
        result |= QTest::qExec(&test, argc, argv);
    }
    {
        EnvirementTest test;
        result |= QTest::qExec(&test, argc, argv);
    }
    {
        IgnoreRuleTest test;
        result |= QTest::qExec(&test, argc, argv);
    }
    {
        LibInfoTest test;
        result |= QTest::qExec(&test, argc, argv);
    }
    {
        DeployConfigTest test;
        result |= QTest::qExec(&test, argc, argv);
    }
    {
        DistroModuleTest test;
        result |= QTest::qExec(&test, argc, argv);
    }
    {
        DistroStructTest test;
        result |= QTest::qExec(&test, argc, argv);
    }
    {
        TargetInfoTest test;
        result |= QTest::qExec(&test, argc, argv);
    }
    {
        FileManagerTest test;
        result |= QTest::qExec(&test, argc, argv);
    }
    {
        ZipCompresserTest test;
        result |= QTest::qExec(&test, argc, argv);
    }
    {
        PeTest test;
        result |= QTest::qExec(&test, argc, argv);
    }

    return result;
}
