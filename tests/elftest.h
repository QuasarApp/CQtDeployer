//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#ifndef ELFTEST_H
#define ELFTEST_H

#include <QtTest>

class ElfTest : public QObject
{
    Q_OBJECT

private slots:
    void parseVersionedSymbols();
    void parseUnversionedExports();
    void parseInvalidFile();
};

#endif // ELFTEST_H
