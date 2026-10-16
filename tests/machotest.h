//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#ifndef MACHOTEST_H
#define MACHOTEST_H

#include <QtTest>

/**
 * @brief The MachoTest class
 *
 * Fast unit tests for the symbol-level part of the LIEF based Mach-O
 * parsing (issue #553 + PR #863 symbol matching follow-up).
 *
 * petest.cpp already covers the basic Mach-O fixture (platform + the
 * LC_LOAD_DYLIB dependencies). This suite adds a richer fixture that also
 * carries a symbol table (LC_SYMTAB) and a dyld export trie
 * (LC_DYLD_EXPORTS_TRIE) so the symbol extraction added for the
 * symbol-based dependency matching can be verified:
 *  - imported symbols: undefined (N_UNDF) nlist entries are reported by
 *    LIEF's imported_symbols() and end up in LibInfo::getRequiredSymbols();
 *  - exported symbols: defined (N_SECT) nlist entries that also appear in
 *    the export trie are reported by LIEF's exported_symbols() and end up
 *    in LibInfo::getProvidedSymbols().
 *
 * The fixture is generated at runtime, so no binary fixture is checked in.
 * The exact nlist n_type values (0x01 for imported, 0x0f for exported) are
 * the ones a real dylib uses and were verified by parsing a genuine macOS
 * dylib with the same LIEF build.
 */
class MachoTest : public QObject
{
    Q_OBJECT

private slots:
    void parseSymbols();
    void parseImports();
    void parseExports();
    void parseInvalidFile();
};

#endif // MACHOTEST_H