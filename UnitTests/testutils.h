/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <QSet>

class TestUtils
{
public:
    TestUtils();
    QSet<QString> getTree(const QString& path, int limit = -1, int depch = 0);
    QSet<QString> createTree( const QStringList& tree);

    /**
     * @brief compareTree - compare 2 tree
     * @param leftTree
     * @param rightTree
     * @return hash with pathes and indexes( 1 - missing left and -1 - missing right)
     */
    QHash<QString, int> compareTree(const QSet<QString>& leftTree, const QSet<QString>& rightTree);

private:
    QString getFilePath(const QString &i);
};

#endif // TESTUTILS_H
