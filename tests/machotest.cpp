//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#include "machotest.h"
#include "libinfo.h"
#include "macho_type.h"

#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QFileInfo>

namespace {

struct MachoBytes {
    QByteArray data;

    void w8(quint8 v) { data.append(static_cast<char>(v)); }
    void w16(quint16 v) { w8(v & 0xff); w8((v >> 8) & 0xff); }
    void w32(quint32 v) {
        for (int i = 0; i < 4; ++i)
            w8((v >> (8 * i)) & 0xff);
    }
    void w64(quint64 v) {
        for (int i = 0; i < 8; ++i)
            w8((v >> (8 * i)) & 0xff);
    }
    void padTo(int pos) {
        while (data.size() < pos)
            data.append('\0');
        if (qEnvironmentVariableIsSet("MACHO_DEBUG"))
            qInfo().noquote() << "padTo(" << pos << ") -> size" << data.size()
                             << (data.size() > pos ? "DRIFT!" : "ok");
    }
};

// Mach-O 64-bit (little endian) constants
static const quint32 MH_MAGIC_64 = 0xFEEDFACF;
static const quint32 CPU_TYPE_X86_64 = 0x01000007; // 7 | ABI64
static const quint32 MH_DYLIB = 6;

// Load command types
static const quint32 LC_SEGMENT_64 = 0x19;
static const quint32 LC_LOAD_DYLIB = 0x0C;
static const quint32 LC_SYMTAB = 0x02;
static const quint32 LC_DYLD_EXPORTS_TRIE = 0x80000033;

// nlist n_type values (verified against a real dylib parsed with this LIEF):
//   0x01 = external undefined symbol  -> LIEF is_external() == true  (import)
//   0x0f = external section symbol    -> LIEF is_external() == false (export)
static const quint8 N_UNDF_EXT = 0x01;
static const quint8 N_SECT_EXT = 0x0f;

// String table layout: "" @0, "bar_import\0" @1, "foo_export\0" @12
static const int sBarImport = 1;
static const int sFooExport = 12;
static const int strtabSize = 23;

// Export trie size: root(1 term + 1 nb_children + 11 suffix + 1 child_off)
//                   + node(1 term + 1 flags + 2 address + 1 nb_children)
static const int trieSize = 19;

/**
 * @brief buildTestMachO Generates a minimal valid Mach-O 64-bit (X86_64)
 * dylib (MH_DYLIB) with the load commands needed for the symbol-level
 * parsing:
 *
 *   LC_SEGMENT_64 (__TEXT)   covers the whole file so the export trie offset
 *                            resolves to a readable segment
 *   LC_LOAD_DYLIB            dependency "libbar.dylib"
 *   LC_SYMTAB                two nlist_64 entries:
 *                              "bar_import" N_UNDF (imported)
 *                              "foo_export" N_SECT (defined)
 *   LC_DYLD_EXPORTS_TRIE     a two-node trie exporting "foo_export", so LIEF
 *                            attaches ExportInfo to the "foo_export" symbol
 *                            (this is what makes it show up in
 *                            exported_symbols()).
 */
QByteArray buildTestMachO()
{
    static const int headerSize = 32;
    static const int fileSize = 0x160; // 352 bytes (multiple of 16, >= all data)

    // Load command offsets
    static const int segOff = 0x20;
    static const int dylibOff = 0x68;
    static const int symtabOff = 0x8D;
    static const int trieCmdOff = 0xA5;
    static const int cmdEnd = 0xB5;

    // Data offsets
    static const int nlistOff = 0x100;
    static const int strtabOff = 0x120;
    static const int trieOff = 0x140;

    const int sizeofcmds = 72 + 37 + 24 + 16; // segment + dylib + symtab + trie

    MachoBytes b;
    // ===== mach_header_64 (32 bytes) =====
    b.w32(MH_MAGIC_64);     // magic
    b.w32(CPU_TYPE_X86_64); // cputype (X86_64)
    b.w32(3);               // cpusubtype (CPU_SUBTYPE_X86_64_ANY)
    b.w32(MH_DYLIB);        // filetype (MH_DYLIB)
    b.w32(4);               // ncmds
    b.w32(sizeofcmds);      // sizeofcmds
    b.w32(0);               // flags
    b.w32(0);               // reserved
    Q_ASSERT(b.data.size() == headerSize);
    b.padTo(segOff);

    // ===== LC_SEGMENT_64 (72 bytes) =====
    b.w32(LC_SEGMENT_64);   // cmd
    b.w32(72);              // cmdsize
    b.data.append("__TEXT");
    b.data.append(QByteArray(10, '\0')); // segname is a 16-byte fixed field
    b.w64(0);               // vmaddr
    b.w64(fileSize);        // vmsize
    b.w64(0);               // fileoff
    b.w64(fileSize);        // filesize
    b.w32(7);               // maxprot (VM_PROT_READ|WRITE|EXEC)
    b.w32(5);               // initprot (VM_PROT_READ|EXEC)
    b.w32(0);               // nsects
    b.w32(0);               // flags
    Q_ASSERT(b.data.size() == dylibOff);

    // ===== LC_LOAD_DYLIB (37 bytes) =====
    b.w32(LC_LOAD_DYLIB);   // cmd
    b.w32(37);              // cmdsize = 24 (header) + 13 (name)
    b.w32(24);              // name offset (relative to the command start)
    b.w32(0);               // timestamp
    b.w32(0x00010000);      // current_version (1.0.0)
    b.w32(0x00010000);      // compatibility_version (1.0.0)
    b.data.append("libbar.dylib");
    b.data.append('\0');    // 13 bytes
    Q_ASSERT(b.data.size() == symtabOff);

    // ===== LC_SYMTAB (24 bytes) =====
    b.w32(LC_SYMTAB);       // cmd
    b.w32(24);              // cmdsize
    b.w32(nlistOff);        // symoff
    b.w32(2);               // nsyms
    b.w32(strtabOff);       // stroff
    b.w32(strtabSize);      // strsize
    Q_ASSERT(b.data.size() == trieCmdOff);

    // ===== LC_DYLD_EXPORTS_TRIE (16 bytes) =====
    b.w32(LC_DYLD_EXPORTS_TRIE); // cmd
    b.w32(16);              // cmdsize
    b.w32(trieOff);         // dataoff
    b.w32(trieSize);        // datasize
    Q_ASSERT(b.data.size() == cmdEnd);

    // ===== nlist_64 array (2 x 16 = 32 bytes) at 0x100 =====
    b.padTo(nlistOff);
    // [0] "bar_import" — undefined external symbol (imported)
    b.w32(sBarImport);      // n_strx
    b.w8(N_UNDF_EXT);       // n_type: external undefined -> is_external() == true
    b.w8(0);                // n_sect
    b.w16(0);               // n_desc
    b.w64(0);               // n_value
    // [1] "foo_export" — defined external symbol in a section (exported)
    b.w32(sFooExport);      // n_strx
    b.w8(N_SECT_EXT);       // n_type: external section -> is_external() == false
    b.w8(1);                // n_sect
    b.w16(0);               // n_desc
    b.w64(0x1000);          // n_value
    Q_ASSERT(b.data.size() == strtabOff);

    // ===== string table (23 bytes) at 0x120 =====
    b.padTo(strtabOff);
    b.w8(0);                                // "" at offset 0
    b.data.append("bar_import");
    b.data.append('\0');                    // offset 1 (11 bytes)
    b.data.append("foo_export");
    b.data.append('\0');                    // offset 12 (11 bytes)
    Q_ASSERT(b.data.size() == strtabOff + strtabSize);

    // ===== dyld export trie (19 bytes) at 0x140 =====
    // root node (no terminal): one child "foo_export" pointing to node 1
    b.padTo(trieOff);
    b.w8(0);                  // terminal_size (root)
    b.w8(1);                  // nb_children (root)
    b.data.append("foo_export");
    b.data.append('\0');      // child suffix (11 bytes)
    b.w8(14);                 // child node offset (absolute in the trie; node1
                              // follows the 14-byte root node)
    // node 1: terminal export of "foo_export"
    b.w8(3);                  // terminal_size (1 flags byte + 2 address bytes)
    b.w8(0);                  // flags (ULEB128) — no reexport / stub
    b.w8(0x80);               // address (ULEB128) low byte, continuation set
    b.w8(0x20);               // address (ULEB128) high byte  -> 0x1000
    b.w8(0);                  // nb_children (node 1)
    Q_ASSERT(b.data.size() == trieOff + trieSize);

    // Pad to the declared segment / file size
    b.padTo(fileSize);
    Q_ASSERT(b.data.size() == fileSize);
    return b.data;
}

// Writes the generated Mach-O fixture into @p dir and returns its path.
QString writeFixture(const QTemporaryDir &dir, const QString &fileName)
{
    const QByteArray macho = buildTestMachO();
    const QString path = dir.filePath(fileName);
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly))
        return {};
    if (f.write(macho) != macho.size())
        return {};
    f.close();
    return path;
}

} // namespace

void MachoTest::parseSymbols()
{
    QTemporaryDir tmp;
    QVERIFY(tmp.isValid());

    const QString path = writeFixture(tmp, "libtest.dylib");
    QVERIFY(!path.isEmpty());

    MachO machoParser;
    LibInfo info;
    QVERIFY(machoParser.getLibInfo(path, info));

    QCOMPARE(info.getPlatform(), Mac_X86_64);
    QCOMPARE(info.getName(), QStringLiteral("libtest.dylib"));
    // The LC_LOAD_DYLIB dependency is reported
    QVERIFY(info.getDependncies().contains(QStringLiteral("libbar.dylib")));

    // The undefined nlist entry is the only required (imported) symbol
    QCOMPARE(info.getRequiredSymbols(),
             (QSet<QString>{QStringLiteral("bar_import")}));
    // The defined nlist entry that has an export-trie entry is the only
    // provided (exported) symbol
    QCOMPARE(info.getProvidedSymbols(),
             (QSet<QString>{QStringLiteral("foo_export")}));
    // Mach-O has no per-symbol versions
    QVERIFY(info.getProvidedVersions().isEmpty());
}

void MachoTest::parseImports()
{
    QTemporaryDir tmp;
    QVERIFY(tmp.isValid());

    const QString path = writeFixture(tmp, "libtest.dylib");
    QVERIFY(!path.isEmpty());

    MachO machoParser;
    LibInfo info;
    QVERIFY(machoParser.getLibInfo(path, info));

    // The undefined symbol is required
    QVERIFY(info.getRequiredSymbols().contains(QStringLiteral("bar_import")));
    // It must not appear as an export
    QVERIFY(!info.getProvidedSymbols().contains(QStringLiteral("bar_import")));
}

void MachoTest::parseExports()
{
    QTemporaryDir tmp;
    QVERIFY(tmp.isValid());

    const QString path = writeFixture(tmp, "libtest.dylib");
    QVERIFY(!path.isEmpty());

    MachO machoParser;
    LibInfo info;
    QVERIFY(machoParser.getLibInfo(path, info));

    // The defined symbol with an export-trie entry is provided
    QVERIFY(info.getProvidedSymbols().contains(QStringLiteral("foo_export")));
    // It must not appear as an import
    QVERIFY(!info.getRequiredSymbols().contains(QStringLiteral("foo_export")));
}

void MachoTest::parseInvalidFile()
{
    QTemporaryDir tmp;
    QVERIFY(tmp.isValid());

    const QString path = tmp.filePath("garbage.dylib");
    {
        QFile file(path);
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write(QByteArray(256, 'x'));
    }

    MachO machoParser;
    LibInfo info;
    QVERIFY(!machoParser.getLibInfo(path, info));
}

// Standard stand-alone QTest entry point, see the Qt test tutorial:
// https://doc.qt.io/qt-6/qttestlib-tutorial1-example.html
QTEST_GUILESS_MAIN(MachoTest)