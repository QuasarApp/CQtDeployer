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

    // Each QTest::qExec() call re-parses the command line arguments.
    // Every call that sees "-o <file>" (re)opens and truncates that
    // file, so if all classes share a single report file, only the
    // last class survives and every failure of the previous classes
    // is lost (see issue #861). Run every class with its own report
    // file "<file>.<ClassName>" so no failure can hide.
    auto runClass = [&argc, argv](const char *className, QObject *obj) -> int {
        QStringList args;
        for (int i = 0; i < argc; ++i) {
            const QString a = QString::fromLocal8Bit(argv[i]);
            args.append(a);
            if (a == QLatin1String("-o") && i + 1 < argc) {
                args.append(QString::fromLocal8Bit(argv[++i]) + QLatin1String(".") + className);
            }
        }

        QVector<QByteArray> a;
        a.reserve(args.size());
        for (const QString &s : std::as_const(args)) {
            a.append(s.toLocal8Bit());
        }

        QVector<char *> argv2;
        argv2.reserve(a.size());
        for (QByteArray &s : a) {
            argv2.append(s.data());
        }
        return QTest::qExec(obj, args.size(), argv2.data());
    };

    int result = 0;

    {
        PathUtilsTest test;
        result |= runClass("PathUtilsTest", &test);
    }
    {
        EnvirementTest test;
        result |= runClass("EnvirementTest", &test);
    }
    {
        IgnoreRuleTest test;
        result |= runClass("IgnoreRuleTest", &test);
    }
    {
        LibInfoTest test;
        result |= runClass("LibInfoTest", &test);
    }
    {
        DeployConfigTest test;
        result |= runClass("DeployConfigTest", &test);
    }
    {
        DistroModuleTest test;
        result |= runClass("DistroModuleTest", &test);
    }
    {
        DistroStructTest test;
        result |= runClass("DistroStructTest", &test);
    }
    {
        TargetInfoTest test;
        result |= runClass("TargetInfoTest", &test);
    }
    {
        FileManagerTest test;
        result |= runClass("FileManagerTest", &test);
    }
    {
        ZipCompresserTest test;
        result |= runClass("ZipCompresserTest", &test);
    }
    {
        PeTest test;
        result |= runClass("PeTest", &test);
    }

    return result;
}
