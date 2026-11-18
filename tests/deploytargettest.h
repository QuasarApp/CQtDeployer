//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#ifndef DEPLOYTARGETTEST_H
#define DEPLOYTARGETTEST_H

#include <QtTest>

class ConfigParser;
class TargetInfo;

/**
 * @brief The DeployTargetTest class
 *
 * Fast unit tests for the target options of the cqtdeployer tool (issue
 * #311, the review comment of the PR #928 "lets use this selector for
 * all our argumets where is posible"). The tests run the real option
 * pipeline of the ConfigParser (the initPackages and the configureTargets
 * methods, the friend access of the DeployTargetTest class is declared in
 * the configparser.h) with the console parameters set in the
 * QuasarAppUtils::Params object and verify that every argument of the tool
 * that selects a target (-targetPackage, -icon, -runScript, -extraDepends,
 * -disableShortCut and the -disableRunScript options) uses the single
 * DeployConfig::getTargetsListByFilter() selector: a selector that does not
 * contain the '*' character must equal the target name or the full path
 * exactly (case-insensitive) and a selector that contains the '*' character
 * is a glob pattern where every '*' matches any sequence of characters.
 */
class DeployTargetTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void targetPackageSelector();
    void targetPackagePlainSelector();
    void iconSelector();
    void runScriptSelector();
    void extraDependsSelector();
    void disableShortCutSelector();
    void disableRunScriptSelector();

private:
    /**
     * @brief addDefaultTargets - fills the config of the @a parser with
     *  the win32, win64, win64_gcc and the linux targets (the keys of the
     *  targets map are the full paths of the target executables). The
     *  static member function of the friend class has the access to the
     *  private members of the ConfigParser (the friend access is declared
     *  in the configparser.h).
     */
    static void addDefaultTargets(ConfigParser &parser);

    /**
     * @brief targetOf - returns the target with the @a name of the
     *  /opt/apps folder from the config of the @a parser.
     */
    static TargetInfo targetOf(ConfigParser &parser, const QString &name);
};

#endif // DEPLOYTARGETTEST_H
