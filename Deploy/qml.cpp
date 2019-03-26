#include "qml.h"

#include <QDir>
#include <QFile>

QStringList QML::extractImportsFromFile(const QString &filepath) {
    QStringList imports;
    QFile F(filepath);
    if (!F.open(QIODevice::ReadOnly)) return QStringList();

    QString content = F.readAll();
    content.remove(QRegExp("\\{(.*)\\}"));
    content.remove(QRegExp("/\\*(.*)\\*/"));

    for (const QString &line : content.split("\n"))
        for (QString &word : line.split(";", QString::SkipEmptyParts))
        {
            word = word.simplified();
            if (word.startsWith("//")) continue;
            if (!word.startsWith("import")) continue;

            QStringList list = word.split(" ", QString::SkipEmptyParts);
            if (list.count() != 3)
            {
                if (list.count() == 5)
                {
                    if (list[3] != "as") continue;
                }
                else
                    continue;
            }

            imports << (list[2][0] + "#" + list[1].replace(".", "/"));
        }

    return imports;
}

bool QML::extractImportsFromDir(const QString &path, qmlDep &dep, bool recursive) {
    QDir dir(path);
    auto infoList = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);

    for (auto &&info: infoList) {

        if (info.isFile()) {
            extractImportsFromFile(info.absoluteFilePath());
        }

    }

    return true;
}

QString QML::getPathFromImport(const QString &import) {

}

QML::QML(const QString &qmlRoot) {
    _qmlRoot = qmlRoot;
}
