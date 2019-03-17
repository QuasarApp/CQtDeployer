#include "pe.h"

#include <QFile>

bool PE::is32bit(const QString &file) {
    QFile f(file);

    if (!f.open(QIODevice::ReadOnly)) {
        return false;
    }

    if (!f.seek(INDEX_PE_MAGIC)) {
        f.close();
        return false;
    }

    unsigned short PE = 0x0;
    f.read(reinterpret_cast<char*>(&PE), sizeof (unsigned short));

    if (PE != PE_MAGIC) {
        f.close();
        return false;
    }

    f.close();
    return true;
}

bool PE::dependecies(QStringList &lisr, const QString &file)
{

}

PE::PE()
{

}
