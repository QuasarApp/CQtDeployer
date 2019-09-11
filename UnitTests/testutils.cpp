#include "testutils.h"

#include <QDir>
#include <QFileInfo>

TestUtils::TestUtils()
{

}

QSet<QString> TestUtils::getTree(const QString &path) {
    QFileInfo info(path);

    if (!info.exists()) {
        return {};
    }

    auto result = QSet<QString>{};

    if (!info.isDir()) {
        result.insert(info.absoluteFilePath());
        return result;
    }

    QDir dir(info.absoluteFilePath());
    auto list = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
    for (auto &i: list) {
        result.unite(getTree(i.absoluteFilePath()));
    }

    return result;
}

QSet<QString> TestUtils::createTree(const QStringList &tree) {
    QSet<QString> res;
    for (auto &i : tree) {
        res.insert(QFileInfo(i).absoluteFilePath());
    }

    return res;
}

QHash<QString, int> TestUtils::compareTree(const QSet<QString> &leftTree, const QSet<QString> &rightTree) {
    QHash<QString, int> result;

    auto valuel = leftTree - rightTree;
    auto valuer = rightTree - leftTree;

    for(auto &i :valuel) {
        result.insert(i, 1);
    }

    for(auto &i :valuer) {
        result.insert(i, -1);
    }

    return result;
}

