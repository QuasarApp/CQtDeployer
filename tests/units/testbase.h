#ifndef TESTBASE_H
#define TESTBASE_H

#include "deploy.h"
#include "test.h"
#include <QString>
#include <deployconfig.h>


class TestBase: public Test
{
public:
    TestBase();

    void deploytest();
    int generateLib(const QString &paath);
    void deleteLib(const QString &paath);
    void initTestCase();

    void cleanupTestCase();

    void checkResults(const QSet<QString> &tree,
                      bool noWarnings,
                      bool onlySize);

    void costomScript();

    void runTestParams(QStringList list,
                       QSet<QString> *tree = nullptr,
                       bool noWarnings = false,
                       bool onlySize = false,
                       exitCodes exitCode = exitCodes::Good,
                       const std::function<void (const DeployConfig *)> &cb = {});


    static const QString TestBinDir;
    static const QString TestQtDir;

private:
    QSet<QString> filesTree;

};

#endif // TESTBASE_H
