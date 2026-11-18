//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#ifndef DEPLOYPACKAGETEST_H
#define DEPLOYPACKAGETEST_H

#include <QtTest>

class ConfigParser;

/**
 * @brief The DeployPackageTest class
 *
 * Fast unit tests for the package options of the cqtdeployer tool (issue
 * #311, the review comment of the PR #928 "But we must support it for
 * all choosers"). The target selector of the issue #311 was supported
 * for the target choosers only (the targets are the executables of the
 * -bin option), so the same exact/glob selector semantics are supported
 * now for the package name choosers of all the package options (the
 * -name, -description, -deployVersion, -releaseDate, -publisher,
 * -homePage, -prefix, -extraData, -tr, -binOut, -qmlOut, -libOut,
 * -trOut, -pluginOut, -recOut, -extraDataOut, -installDirDeb, -debOut,
 * -zipOut, -appimageOut, -dmgOut, -qmlDir, -extraPlugin, -enablePlugins
 * and the -disablePlugins options): every option with a chooser uses
 * the single DeployConfig::getPackagesListByFilter() entry point. The
 * tests run the real option pipeline of the ConfigParser (the
 * initDistroStruct method, the friend access of the DeployPackageTest
 * class is declared in the configparser.h) with the console parameters
 * set in the QuasarAppUtils::Params object and verify that the
 * -name and the -description options (the representatives of the
 * package options pipeline) use the package selector: a selector that
 * does not contain the '*' character must equal the package name
 * exactly (case-insensitive) and a selector that contains the '*'
 * character is a glob pattern where every '*' matches any sequence of
 * characters.
 */
class DeployPackageTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void packageSelectorExact();
    void packageSelectorWildcard();
    void packageSelectorNoMatch();
    void packageSelectorGlobNoMatch();
    void packageAllowEmpty();
    void packageDefaultChooser();

private:
    /**
     * @brief addDefaultPackages - fills the config of the @a parser with
     *  the win32Package, win64Package, win64GccPackage and the
     *  linuxPackage packages (the static member function of the friend
     *  class has the access to the private members of the ConfigParser,
     *  the friend access is declared in the configparser.h).
     */
    static void addDefaultPackages(ConfigParser &parser);
};

#endif // DEPLOYPACKAGETEST_H