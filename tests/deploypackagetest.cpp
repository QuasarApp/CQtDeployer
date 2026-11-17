//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#include "deploypackagetest.h"
#include "configparser.h"
#include "deployconfig.h"
#include "distromodule.h"
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
 *  pipeline under test (the initDistroStruct method works with the
 *  DeployConfig and the QuasarAppUtils::Params objects only), but the
 *  ConfigParser constructor checks that all the pointers are not null.
 */
ConfigParser *createParser() {
    static FileManager fileManager;
    static PluginsParser pluginsParser;
    static DependenciesScanner scanner;
    static Packing packing(&fileManager);

    return new ConfigParser(&fileManager, &pluginsParser, &scanner, &packing);
}

} // namespace

void DeployPackageTest::addDefaultPackages(ConfigParser &parser) {
    const QStringList names{QStringLiteral("win32Package"),
                            QStringLiteral("win64Package"),
                            QStringLiteral("win64GccPackage"),
                            QStringLiteral("linuxPackage")};
    for (const auto &name : names) {
        parser._config.packagesEdit().insert(name, DistroModule{name});
    }
}

void DeployPackageTest::initTestCase() {
    // The QuasarAppUtils::Params object is a process-wide singleton, so
    // the tests must not leak the options between the slots.
    QuasarAppUtils::Params::clearParsedData();
}

void DeployPackageTest::cleanupTestCase() {
    QuasarAppUtils::Params::clearParsedData();
}

void DeployPackageTest::packageSelectorExact() {
    QuasarAppUtils::Params::clearParsedData();
    auto *parser = createParser();

    addDefaultPackages(*parser);

    // The plain selector of the package option must equal the package
    // name exactly (the comparison is case-insensitive, issue #311):
    // the 'WIN32PACKAGE' selector selects only the 'win32Package'
    // package.
    QuasarAppUtils::Params::setEnable("name", true);
    QuasarAppUtils::Params::setArg("name", "WIN32PACKAGE;Win 32 Name,win64GccPackage;Gcc Name");
    QVERIFY(parser->initDistroStruct());

    QCOMPARE(parser->_config.packagesEdit().value("win32Package", DistroModule{"win32Package"}).name(),
             QString("Win 32 Name"));
    QCOMPARE(parser->_config.packagesEdit().value("win64GccPackage", DistroModule{"win64GccPackage"}).name(),
             QString("Gcc Name"));

    // The packages without a selector are unchanged.
    QVERIFY(parser->_config.packagesEdit().value("win64Package", DistroModule{"win64Package"}).name().isEmpty());
    QVERIFY(parser->_config.packagesEdit().value("linuxPackage", DistroModule{"linuxPackage"}).name().isEmpty());

    delete parser;
    QuasarAppUtils::Params::clearParsedData();
}

void DeployPackageTest::packageSelectorWildcard() {
    QuasarAppUtils::Params::clearParsedData();
    auto *parser = createParser();

    addDefaultPackages(*parser);

    // The '*' glob of the package option selects all the packages that
    // match the pattern and the other packages stay unchanged (issue
    // #311).
    QuasarAppUtils::Params::setEnable("description", true);
    QuasarAppUtils::Params::setArg("description", "win*Package;Windows Description");
    QVERIFY(parser->initDistroStruct());

    QCOMPARE(parser->_config.packagesEdit().value("win32Package", DistroModule{"win32Package"}).description(),
             QString("Windows Description"));
    QCOMPARE(parser->_config.packagesEdit().value("win64Package", DistroModule{"win64Package"}).description(),
             QString("Windows Description"));
    QCOMPARE(parser->_config.packagesEdit().value("win64GccPackage", DistroModule{"win64GccPackage"}).description(),
             QString("Windows Description"));

    // The 'linuxPackage' package does not match the 'win*Package'
    // pattern.
    QVERIFY(parser->_config.packagesEdit().value("linuxPackage", DistroModule{"linuxPackage"}).description().isEmpty());

    delete parser;
    QuasarAppUtils::Params::clearParsedData();
}

void DeployPackageTest::packageSelectorNoMatch() {
    QuasarAppUtils::Params::clearParsedData();
    auto *parser = createParser();

    addDefaultPackages(*parser);

    // A plain selector without a match selects nothing: the old
    // 'contains' substring match of the selector is gone (issue #311),
    // the package is not created (the allowEmptyPackages option is not
    // enabled) and the option is failed.
    QuasarAppUtils::Params::setEnable("name", true);
    QuasarAppUtils::Params::setArg("name", "win;Win Name");
    QVERIFY(!parser->initDistroStruct());

    QVERIFY(!parser->_config.packagesEdit().contains("win"));

    delete parser;
    QuasarAppUtils::Params::clearParsedData();
}

void DeployPackageTest::packageSelectorGlobNoMatch() {
    QuasarAppUtils::Params::clearParsedData();
    auto *parser = createParser();

    addDefaultPackages(*parser);

    // A glob pattern without a match selects nothing: the pattern cannot
    // name the package to create, so the option is failed (issue #311).
    QuasarAppUtils::Params::setEnable("name", true);
    QuasarAppUtils::Params::setArg("name", "nope*;Win Name");
    QVERIFY(!parser->initDistroStruct());

    delete parser;
    QuasarAppUtils::Params::clearParsedData();
}

void DeployPackageTest::packageAllowEmpty() {
    QuasarAppUtils::Params::clearParsedData();
    auto *parser = createParser();

    // A plain selector without a match keeps the old behavior: the
    // package is created on the fly when the allowEmptyPackages option
    // is enabled (issue #311).
    QuasarAppUtils::Params::setEnable("allowEmptyPackages", true);
    QuasarAppUtils::Params::setEnable("name", true);
    QuasarAppUtils::Params::setArg("name", "newPackage;New Name");
    QVERIFY(parser->initDistroStruct());

    QVERIFY(parser->_config.packagesEdit().contains("newPackage"));
    QCOMPARE(parser->_config.packagesEdit().value("newPackage", DistroModule{"newPackage"}).name(),
             QString("New Name"));

    delete parser;
    QuasarAppUtils::Params::clearParsedData();
}

void DeployPackageTest::packageDefaultChooser() {
    QuasarAppUtils::Params::clearParsedData();
    auto *parser = createParser();

    // A value without the package selector (the single level value) is
    // set to the default package (issue #311).
    QuasarAppUtils::Params::setEnable("name", true);
    QuasarAppUtils::Params::setArg("name", "Default Name");
    QVERIFY(parser->initDistroStruct());

    QCOMPARE(parser->_config.getDefaultPackage(), QString("Application"));
    QVERIFY(parser->_config.packagesEdit().contains("Application"));
    QCOMPARE(parser->_config.packagesEdit().value("Application", DistroModule{"Application"}).name(),
             QString("Default Name"));

    delete parser;
    QuasarAppUtils::Params::clearParsedData();
}

// Standard stand-alone QTest entry point, see the Qt test tutorial:
// https://doc.qt.io/qt-6/qttestlib-tutorial1-example.html
QTEST_GUILESS_MAIN(DeployPackageTest)
