//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#include "deploytargettest.h"
#include "configparser.h"
#include "deployconfig.h"
#include "targetinfo.h"
#include "filemanager.h"
#include "pluginsparser.h"
#include "dependenciesscanner.h"
#include "packing.h"

#include <quasarapp.h>
#include <QFileInfo>

namespace {

/**
 * @brief createParser - creates a ConfigParser with the real dependencies
 *  of the Deploy library. The dependencies are not used by the option
 *  pipeline under test (the initPackages and the configureTargets methods
 *  work with the DeployConfig and the QuasarAppUtils::Params objects only),
 *  but the ConfigParser constructor checks that all the pointers are not
 *  null.
 */
ConfigParser *createParser() {
    static FileManager fileManager;
    static PluginsParser pluginsParser;
    static DependenciesScanner scanner;
    static Packing packing(&fileManager);

    return new ConfigParser(&fileManager, &pluginsParser, &scanner, &packing);
}

TargetInfo makeTarget(const QString &path, const QString &name) {
    TargetInfo target;
    target.setPath(path);
    target.setName(name);
    target.setPlatform(Unix_x86_64);
    return target;
}

} // namespace

void DeployTargetTest::addDefaultTargets(ConfigParser &parser) {
    const QStringList names{QStringLiteral("win32"),
                            QStringLiteral("win64"),
                            QStringLiteral("win64_gcc"),
                            QStringLiteral("linux")};
    for (const auto &name : names) {
        auto target = makeTarget(QStringLiteral("/opt/apps"), name);
        parser._config.targetsEdit().insert(target.fullPath(), target);
    }
}

TargetInfo DeployTargetTest::targetOf(ConfigParser &parser, const QString &name) {
    return parser._config.targets().value(QStringLiteral("/opt/apps/") + name);
}

void DeployTargetTest::initTestCase() {
    // The QuasarAppUtils::Params object is a process-wide singleton, so
    // the tests must not leak the options between the slots.
    QuasarAppUtils::Params::clearParsedData();
}

void DeployTargetTest::cleanupTestCase() {
    QuasarAppUtils::Params::clearParsedData();
}

void DeployTargetTest::targetPackageSelector() {
    QuasarAppUtils::Params::clearParsedData();
    auto *parser = createParser();

    addDefaultTargets(*parser);

    // The '*' glob of the targetPackage option selects all the targets
    // that match the pattern and the plain 'linux' selector selects only
    // the exact target (issue #311).
    QuasarAppUtils::Params::setEnable("targetPackage", true);
    QuasarAppUtils::Params::setArg("targetPackage", "winPackage;win*,linuxPackage;linux");
    QVERIFY(parser->initPackages());

    QVERIFY(parser->_config.packagesEdit().contains("winPackage"));
    QVERIFY(parser->_config.packagesEdit().contains("linuxPackage"));
    QCOMPARE(targetOf(*parser, "win32").getPackage(), QString("winPackage"));
    QCOMPARE(targetOf(*parser, "win64").getPackage(), QString("winPackage"));
    QCOMPARE(targetOf(*parser, "win64_gcc").getPackage(), QString("winPackage"));
    QCOMPARE(targetOf(*parser, "linux").getPackage(), QString("linuxPackage"));

    delete parser;
    QuasarAppUtils::Params::clearParsedData();
}

void DeployTargetTest::targetPackagePlainSelector() {
    QuasarAppUtils::Params::clearParsedData();
    auto *parser = createParser();

    addDefaultTargets(*parser);

    // The old 'contains' substring match of the selector is gone (issue
    // #311): the plain 'win' selector must equal the target name exactly,
    // so it selects nothing (the "no matches were found for this pattern"
    // warning is printed and the package is not created) and all the
    // targets stay in the default package.
    QuasarAppUtils::Params::setEnable("targetPackage", true);
    QuasarAppUtils::Params::setArg("targetPackage", "pkg;win");
    QVERIFY(parser->initPackages());

    QVERIFY(!parser->_config.packagesEdit().contains("pkg"));
    QCOMPARE(targetOf(*parser, "win32").getPackage(), QString("Application"));
    QCOMPARE(targetOf(*parser, "win64").getPackage(), QString("Application"));
    QCOMPARE(targetOf(*parser, "win64_gcc").getPackage(), QString("Application"));
    QCOMPARE(targetOf(*parser, "linux").getPackage(), QString("Application"));

    delete parser;
    QuasarAppUtils::Params::clearParsedData();
}

void DeployTargetTest::iconSelector() {
    QuasarAppUtils::Params::clearParsedData();
    auto *parser = createParser();

    addDefaultTargets(*parser);

    // The '*' glob of the icon option is case-insensitive: all the win*
    // targets get the icon and the linux target keeps the default icon.
    QuasarAppUtils::Params::setArg("icon", "WIN*;icon.png");
    QVERIFY(parser->configureTargets());

    QCOMPARE(targetOf(*parser, "win32").getIcon(), QString("icon.png"));
    QCOMPARE(targetOf(*parser, "win64").getIcon(), QString("icon.png"));
    QCOMPARE(targetOf(*parser, "win64_gcc").getIcon(), QString("icon.png"));
    QCOMPARE(targetOf(*parser, "linux").getIcon(),
             QString(":/shared/Distributions/Templates/Icon.png"));

    delete parser;
    QuasarAppUtils::Params::clearParsedData();

    // The plain selector selects only the exact target and the old
    // 'contains' substring match ('win') selects nothing.
    parser = createParser();
    addDefaultTargets(*parser);
    QuasarAppUtils::Params::setArg("icon", "linux;linux.png");
    QVERIFY(parser->configureTargets());

    QCOMPARE(targetOf(*parser, "linux").getIcon(), QString("linux.png"));
    QCOMPARE(targetOf(*parser, "win32").getIcon(),
             QString(":/shared/Distributions/Templates/Icon.png"));
    QCOMPARE(targetOf(*parser, "win64").getIcon(),
             QString(":/shared/Distributions/Templates/Icon.png"));

    delete parser;
    QuasarAppUtils::Params::clearParsedData();
}

void DeployTargetTest::runScriptSelector() {
    QuasarAppUtils::Params::clearParsedData();
    auto *parser = createParser();

    addDefaultTargets(*parser);

    // The '*' glob of the runScript option selects all the win* targets,
    // the linux target keeps the default run script.
    QuasarAppUtils::Params::setArg("runScript", "win*;custom.sh");
    QVERIFY(parser->configureTargets());

    QCOMPARE(targetOf(*parser, "win32").getRunScript(), QString("custom.sh"));
    QCOMPARE(targetOf(*parser, "win64").getRunScript(), QString("custom.sh"));
    QCOMPARE(targetOf(*parser, "win64_gcc").getRunScript(), QString("custom.sh"));
    QCOMPARE(targetOf(*parser, "linux").getRunScript(),
             QString(":/Scripts/ScriptsTemplates/linux.sh"));

    delete parser;
    QuasarAppUtils::Params::clearParsedData();

    // The plain selector selects only the exact target.
    parser = createParser();
    addDefaultTargets(*parser);
    QuasarAppUtils::Params::setArg("runScript", "linux;linux.sh");
    QVERIFY(parser->configureTargets());

    QCOMPARE(targetOf(*parser, "linux").getRunScript(), QString("linux.sh"));
    QCOMPARE(targetOf(*parser, "win32").getRunScript(),
             QString(":/Scripts/ScriptsTemplates/linux.sh"));

    delete parser;
    QuasarAppUtils::Params::clearParsedData();
}


void DeployTargetTest::extraDependsSelector() {
    QuasarAppUtils::Params::clearParsedData();
    auto *parser = createParser();

    addDefaultTargets(*parser);

    // The '*' glob of the extraDepends option adds the dependency to all
    // the win* targets only.
    QuasarAppUtils::Params::setArg("extraDepends", "win*;libssl.so");
    QVERIFY(parser->configureTargets());

    QVERIFY(targetOf(*parser, "win32").getDependncies().contains("libssl.so"));
    QVERIFY(targetOf(*parser, "win64").getDependncies().contains("libssl.so"));
    QVERIFY(targetOf(*parser, "win64_gcc").getDependncies().contains("libssl.so"));
    QVERIFY(!targetOf(*parser, "linux").getDependncies().contains("libssl.so"));

    delete parser;
    QuasarAppUtils::Params::clearParsedData();

    // A value without the target selector (no ';' separator) is set for
    // all the targets and the plain selector selects only the exact target.
    parser = createParser();
    addDefaultTargets(*parser);
    QuasarAppUtils::Params::setArg("extraDepends", "libz.so,linux;libpng.so");
    QVERIFY(parser->configureTargets());

    QVERIFY(targetOf(*parser, "win32").getDependncies().contains("libz.so"));
    QVERIFY(targetOf(*parser, "win64").getDependncies().contains("libz.so"));
    QVERIFY(targetOf(*parser, "linux").getDependncies().contains("libz.so"));
    QVERIFY(targetOf(*parser, "linux").getDependncies().contains("libpng.so"));
    QVERIFY(!targetOf(*parser, "win32").getDependncies().contains("libpng.so"));

    delete parser;
    QuasarAppUtils::Params::clearParsedData();
}

void DeployTargetTest::disableShortCutSelector() {
    QuasarAppUtils::Params::clearParsedData();
    auto *parser = createParser();

    addDefaultTargets(*parser);

    // The '*' glob of the disableShortCut option disables the shortcut
    // generation for all the win* targets only.
    QuasarAppUtils::Params::setArg("disableShortCut", "win*");
    QVERIFY(parser->configureTargets());

    QCOMPARE(targetOf(*parser, "win32").getShortCut(), false);
    QCOMPARE(targetOf(*parser, "win64").getShortCut(), false);
    QCOMPARE(targetOf(*parser, "win64_gcc").getShortCut(), false);
    QCOMPARE(targetOf(*parser, "linux").getShortCut(), true);

    delete parser;
    QuasarAppUtils::Params::clearParsedData();

    // The plain selector selects only the exact target and the old
    // 'contains' substring match ('win') selects nothing.
    parser = createParser();
    addDefaultTargets(*parser);
    QuasarAppUtils::Params::setArg("disableShortCut", "linux");
    QVERIFY(parser->configureTargets());

    QCOMPARE(targetOf(*parser, "linux").getShortCut(), false);
    QCOMPARE(targetOf(*parser, "win32").getShortCut(), true);
    QCOMPARE(targetOf(*parser, "win64").getShortCut(), true);

    delete parser;
    QuasarAppUtils::Params::clearParsedData();
}


void DeployTargetTest::disableRunScriptSelector() {
    QuasarAppUtils::Params::clearParsedData();
    auto *parser = createParser();

    addDefaultTargets(*parser);

    // The '*' glob of the disableRunScript option disables the run script
    // generation for all the win* targets only.
    QuasarAppUtils::Params::setArg("disableRunScript", "win*");
    QVERIFY(parser->configureTargets());

    QCOMPARE(targetOf(*parser, "win32").fEnableRunScript(), false);
    QCOMPARE(targetOf(*parser, "win64").fEnableRunScript(), false);
    QCOMPARE(targetOf(*parser, "win64_gcc").fEnableRunScript(), false);
    QCOMPARE(targetOf(*parser, "linux").fEnableRunScript(), true);

    delete parser;
    QuasarAppUtils::Params::clearParsedData();

    // The plain selector selects only the exact target.
    parser = createParser();
    addDefaultTargets(*parser);
    QuasarAppUtils::Params::setArg("disableRunScript", "linux");
    QVERIFY(parser->configureTargets());

    QCOMPARE(targetOf(*parser, "linux").fEnableRunScript(), false);
    QCOMPARE(targetOf(*parser, "win32").fEnableRunScript(), true);
    QCOMPARE(targetOf(*parser, "win64").fEnableRunScript(), true);

    delete parser;
    QuasarAppUtils::Params::clearParsedData();
}

QTEST_GUILESS_MAIN(DeployTargetTest)

