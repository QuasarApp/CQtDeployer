#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <QSet>

class TestUtils
{
public:
    TestUtils();
    QSet<QString> getTree( const QString& path);
    QSet<QString> createTree( const QStringList& tree);

    /**
     * @brief compareTree - compare 2 tree
     * @param leftTree
     * @param rightTree
     * @return hash with pathes and indexes( 1 - missing left and -1 - missing right)
     */
    QHash<QString, int> compareTree(const QSet<QString>& leftTree, const QSet<QString>& rightTree);

};

#endif // TESTUTILS_H
