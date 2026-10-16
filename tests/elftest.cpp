//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#include "elftest.h"
#include "elf_type.h"
#include "libinfo.h"

#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QFileInfo>
#include <QTextStream>

namespace {

struct ElfBytes {
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
        if (qEnvironmentVariableIsSet("ELF_DEBUG"))
            qInfo().noquote() << "padTo(" << pos << ") -> size" << data.size()
                             << (data.size() > pos ? "DRIFT!" : "ok");
    }
};

// ELF64 section types
static const quint32 SHT_DYNAMIC = 6;
static const quint32 SHT_DYNSYM = 11;
static const quint32 SHT_STRTAB = 3;
static const quint32 SHT_GNU_VERDEF = 0x6ffffffdu;
static const quint32 SHT_GNU_VERNEED = 0x6ffffffeu;
static const quint32 SHT_GNU_VERSYM = 0x6fffffffu;

// Dynamic table tags
static const qint64 DT_NULL = 0;
static const qint64 DT_NEEDED = 1;
static const qint64 DT_STRTAB = 5;
static const qint64 DT_SYMTAB = 6;
static const qint64 DT_STRSZ = 10;
static const qint64 DT_SYMENT = 11;
static const qint64 DT_VERDEF = 0x6ffffffcLL;
static const qint64 DT_VERDEFNUM = 0x6ffffffdLL;
static const qint64 DT_VERNEED = 0x6ffffffeLL;
static const qint64 DT_VERNEEDNUM = 0x6fffffffLL;
static const qint64 DT_VERSYM = 0x6ffffff0LL;

// .dynstr offsets: "" "libfoo.so" "libc.so.6" "qDebug" "memcpy" "myexport"
//                  "other" "GLIBC_2.2.5" "GLIBC_2.14" "MYLIB_1.0"
static const int sLibFoo = 1;
static const int sLibc = 11;
static const int sQDebug = 21;
static const int sMemcpy = 28;
static const int sMyExport = 35;
static const int sOther = 44;
static const int sGlibc225 = 50;
static const int sGlibc214 = 62;
static const int sMyLib = 73;
static const int dynStrSize = 83;

// .shstrtab offsets: "" ".dynsym" ".dynstr" ".dynamic" ".gnu.version"
//                    ".gnu.version_r" ".gnu.version_d" ".shstrtab"
static const int nDynSym = 1;
static const int nDynStr = 9;
static const int nDynamic = 17;
static const int nGnuVersion = 26;
static const int nGnuVersionR = 39;
static const int nGnuVersionD = 54;
static const int nShstrtab = 69;
static const int shstrtabSize = 79;
/**
 * @brief buildTestElf Generates a minimal valid ELF64 x86-64 shared object
 * (ET_DYN) with a .dynsym table, a dynamic table and the GNU symbol
 * version tables (.gnu.version, .gnu.version_r, .gnu.version_d).
 *
 * Dynamic symbols:
 *   [0] <null>
 *   [1] qDebug   imported, versioned GLIBC_2.14 (via .gnu.version_r)
 *   [2] memcpy   imported, versioned GLIBC_2.2.5 (via .gnu.version_r)
 *   [3] myexport exported, versioned MYLIB_1.0 (via .gnu.version_d)
 *   [4] other    exported, unversioned
 *
 * DT_NEEDED = "libfoo.so"
 */
QByteArray buildTestElf()
{
    static const int phoff = 0x40;
    static const int dynOff = 0xb0;
    static const int dynSize = 11 * 16;
    static const int dynSymOff = 0x160;
    static const int dynSymSize = 5 * 24;

    const int dynStrOff = dynSymOff + dynSymSize;
    const int versymOff = dynStrOff + dynStrSize;
    const int versymSize = 5 * 2;
    const int verneedOff = versymOff + versymSize;
    // Elf64_Verneed (16) + 2 x Elf64_Verneedaux (16)
    const int verneedSize = 16 + 2 * 16;
    const int verdefOff = verneedOff + verneedSize;
    // Elf64_Verdef (20) + 1 x Elf64_Verdaux (8: vda_name + vda_next, the
    // layout of LIEF::ELF::details::Elf64_Verdaux in the LIEF version
    // CQtDeployer is built against)
    const int verdefSize = 20 + 8;
    const int shstrOff = verdefOff + verdefSize;
    const int shoff = (shstrOff + shstrtabSize + 7) / 8 * 8;
    const int fileSize = shoff + 8 * 64;

    ElfBytes e;

    // --- ELF header ---
    e.data.append('\x7f');
    e.data.append("ELF");
    e.w8(2); // EI_CLASS: ELFCLASS64
    e.w8(1); // EI_DATA: ELFDATA2LSB
    e.w8(1); // EI_VERSION: EV_CURRENT
    e.w8(0); // EI_OSABI: ELFOSABI_NONE
    e.w8(0); // EI_ABIVERSION
    for (int i = 0; i < 7; ++i)
        e.w8(0); // padding
    e.w16(3);      // e_type: ET_DYN
    e.w16(62);     // e_machine: EM_X86_64
    e.w32(1);      // e_version: EV_CURRENT
    e.w64(0);      // e_entry
    e.w64(phoff);  // e_phoff
    e.w64(shoff);  // e_shoff
    e.w32(0);      // e_flags
    e.w16(64);     // e_ehsize
    e.w16(56);     // e_phentsize
    e.w16(2);      // e_phnum
    e.w16(64);     // e_shentsize
    e.w16(8);      // e_shnum
    e.w16(7);      // e_shstrndx
    e.padTo(phoff);

    // --- Program headers ---
    // PT_LOAD covering the whole file (identity vaddr == offset)
    e.w32(1);        // p_type: PT_LOAD
    e.w32(7);        // p_flags: PF_R|PF_W|PF_X
    e.w64(0);        // p_offset
    e.w64(0);        // p_vaddr
    e.w64(0);        // p_paddr
    e.w64(fileSize); // p_filesz
    e.w64(fileSize); // p_memsz
    e.w64(0x1000);   // p_align
    // PT_DYNAMIC
    e.w32(2);        // p_type: PT_DYNAMIC
    e.w32(4);        // p_flags: PF_R
    e.w64(dynOff);   // p_offset
    e.w64(dynOff);   // p_vaddr
    e.w64(dynOff);   // p_paddr
    e.w64(dynSize);  // p_filesz
    e.w64(dynSize);  // p_memsz
    e.w64(8);        // p_align
    e.padTo(dynOff);

    // --- .dynamic ---
    e.w64(DT_NEEDED);     e.w64(sLibFoo);
    e.w64(DT_STRTAB);     e.w64(dynStrOff);
    e.w64(DT_STRSZ);      e.w64(dynStrSize);
    e.w64(DT_SYMTAB);     e.w64(dynSymOff);
    e.w64(DT_SYMENT);     e.w64(24);
    e.w64(DT_VERSYM);     e.w64(versymOff);
    e.w64(DT_VERNEED);    e.w64(verneedOff);
    e.w64(DT_VERNEEDNUM); e.w64(1);
    e.w64(DT_VERDEF);     e.w64(verdefOff);
    e.w64(DT_VERDEFNUM);  e.w64(1);
    e.w64(DT_NULL);       e.w64(0);
    e.padTo(dynSymOff);

    // --- .dynsym (Elf64_Sym) ---
    // [0] null symbol
    e.w32(0); e.w8(0); e.w8(0); e.w16(0); e.w64(0); e.w64(0);
    // [1] qDebug: GLOBAL | FUNC, UNDEF
    e.w32(sQDebug); e.w8(0x11); e.w8(0); e.w16(0); e.w64(0); e.w64(0);
    // [2] memcpy: GLOBAL | FUNC, UNDEF
    e.w32(sMemcpy); e.w8(0x11); e.w8(0); e.w16(0); e.w64(0); e.w64(0);
    // [3] myexport: GLOBAL | FUNC, section 1
    e.w32(sMyExport); e.w8(0x11); e.w8(0); e.w16(1); e.w64(0x1000); e.w64(16);
    // [4] other: GLOBAL | FUNC, section 1
    e.w32(sOther); e.w8(0x11); e.w8(0); e.w16(1); e.w64(0x1100); e.w64(16);
    e.padTo(dynStrOff);

    // --- .dynstr (each name is explicitly null-terminated) ---
    auto addStr = [&e](const char *s) {
        e.data.append(s);
        e.w8(0);
    };
    e.w8(0); // "" (index 0)
    addStr("libfoo.so");
    addStr("libc.so.6");
    addStr("qDebug");
    addStr("memcpy");
    addStr("myexport");
    addStr("other");
    addStr("GLIBC_2.2.5");
    addStr("GLIBC_2.14");
    addStr("MYLIB_1.0");
    e.padTo(versymOff);

    // --- .gnu.version (versym) ---
    //   value 0          : no version
    //   value N (1..0x7fff) : Nth Verneedaux entry (vna_other == N)
    //   value 0x8000 | N : Nth Verdef entry (vd_ndx == N), for defined symbols
    e.w16(0);       // [0] null
    e.w16(2);       // [1] qDebug   -> verneed aux #2 (GLIBC_2.14)
    e.w16(1);       // [2] memcpy   -> verneed aux #1 (GLIBC_2.2.5)
    e.w16(0x8003);  // [3] myexport -> verdef ndx 3 (MYLIB_1.0)
    e.w16(0);       // [4] other    -> no version
    e.padTo(verneedOff);

    // --- .gnu.version_r (verneed) ---
    // Elf64_Verneed:   vn_version(2) vn_cnt(2) vn_file(4) vn_aux(4) vn_next(4) = 16
    // Elf64_Verneedaux: vna_hash(4) vna_flags(2) vna_other(2) vna_name(4) vna_next(4) = 16
    e.w16(1);           // vn_version
    e.w16(2);           // vn_cnt: 2 aux entries
    e.w32(sLibc);       // vn_file
    e.w32(16);          // vn_aux: offset of first aux from the start of this Verneed
    e.w32(0);           // vn_next
    e.w32(0);           // vna_hash
    e.w16(0);           // vna_flags
    e.w16(1);           // vna_other (versym index)
    e.w32(sGlibc225);   // vna_name
    e.w32(16);          // vna_next: offset of the next aux
    e.w32(0);           // vna_hash
    e.w16(0);           // vna_flags
    e.w16(2);           // vna_other (versym index)
    e.w32(sGlibc214);   // vna_name
    e.w32(0);           // vna_next
    e.padTo(verdefOff);

    // --- .gnu.version_d (verdef) ---
    // Elf64_Verdef:  vd_version(2) vd_flags(2) vd_ndx(2) vd_cnt(2) vd_hash(4) vd_aux(4) vd_next(4) = 20
    // Elf64_Verdaux: vda_name(4) vda_next(4) = 8
    // (The LIEF version CQtDeployer is built against declares Elf64_Verdaux
    // without the vda_hash/vda_flags/vda_other fields, so the aux entries in
    // this fixture follow that 8-byte layout.)
    e.w16(1);           // vd_version
    e.w16(0);           // vd_flags
    e.w16(3);           // vd_ndx (versym index)
    e.w16(1);           // vd_cnt
    e.w32(0);           // vd_hash
    e.w32(20);          // vd_aux: offset of first aux from the start of this Verdef
    e.w32(0);           // vd_next
    e.w32(sMyLib);      // vda_name
    e.w32(0);           // vda_next
    e.padTo(shstrOff);

    // --- .shstrtab (each name is explicitly null-terminated) ---
    e.w8(0); // "" (index 0)
    addStr(".dynsym");
    addStr(".dynstr");
    addStr(".dynamic");
    addStr(".gnu.version");
    addStr(".gnu.version_r");
    addStr(".gnu.version_d");
    addStr(".shstrtab");
    e.padTo(shoff);

    // --- Section header table (8 x 64) ---
    auto secPos = [&](int i) {
        if (qEnvironmentVariableIsSet("ELF_DEBUG"))
            qInfo().noquote() << "section [" << i << "] expected at" << (shoff + i * 64)
                             << "actual" << e.data.size();
    };
    // [0] null
    e.w32(0); e.w32(0); e.w64(0); e.w64(0); e.w64(0); e.w64(0);
    e.w32(0); e.w32(0); e.w64(0); e.w64(0);
    secPos(1);
    // [1] .dynsym
    e.w32(nDynSym); e.w32(SHT_DYNSYM); e.w64(2); e.w64(dynSymOff);
    e.w64(dynSymOff); e.w64(dynSymSize); e.w32(2); e.w32(0); e.w64(8); e.w64(24);
    secPos(2);
    // [2] .dynstr
    e.w32(nDynStr); e.w32(SHT_STRTAB); e.w64(0); e.w64(dynStrOff);
    e.w64(dynStrOff); e.w64(dynStrSize); e.w32(0); e.w32(0); e.w64(1); e.w64(1);
    secPos(3);
    // [3] .dynamic
    e.w32(nDynamic); e.w32(SHT_DYNAMIC); e.w64(2); e.w64(dynOff);
    e.w64(dynOff); e.w64(dynSize); e.w32(2); e.w32(0); e.w64(8); e.w64(16);
    secPos(4);
    // [4] .gnu.version
    e.w32(nGnuVersion); e.w32(SHT_GNU_VERSYM); e.w64(2); e.w64(versymOff);
    e.w64(versymOff); e.w64(versymSize); e.w32(0); e.w32(0); e.w64(2); e.w64(2);
    secPos(5);
    // [5] .gnu.version_r
    e.w32(nGnuVersionR); e.w32(SHT_GNU_VERNEED); e.w64(0); e.w64(verneedOff);
    e.w64(verneedOff); e.w64(verneedSize); e.w32(0); e.w32(0); e.w64(4); e.w64(16);
    secPos(6);
    // [6] .gnu.version_d
    e.w32(nGnuVersionD); e.w32(SHT_GNU_VERDEF); e.w64(0); e.w64(verdefOff);
    e.w64(verdefOff); e.w64(verdefSize); e.w32(0); e.w32(0); e.w64(4); e.w64(16);
    secPos(7);
    // [7] .shstrtab
    e.w32(nShstrtab); e.w32(SHT_STRTAB); e.w64(0); e.w64(shstrOff);
    e.w64(shstrOff); e.w64(shstrtabSize); e.w32(0); e.w32(0); e.w64(1); e.w64(1);

    Q_ASSERT(e.data.size() == fileSize);
    return e.data;
}

} // namespace

void ElfTest::parseVersionedSymbols()
{
    QTemporaryDir tmp;
    QVERIFY(tmp.isValid());

    const QByteArray elf = buildTestElf();
    if (qEnvironmentVariableIsSet("KEEP_FIXTURE")) {
        QFile dbg(QStringLiteral("/tmp/elffixture.so"));
        if (dbg.open(QIODevice::WriteOnly)) dbg.write(elf);
    }
    const QString path = tmp.filePath("libtest.so");
    {
        QFile file(path);
        QVERIFY(file.open(QIODevice::WriteOnly));
        QCOMPARE(file.write(elf), qint64(elf.size()));
    }

    ELF elfParser;
    LibInfo info;
    QVERIFY(elfParser.getLibInfo(path, info));

    QCOMPARE(info.getPlatform(), Unix_x86_64);
    QCOMPARE(info.getName(), QStringLiteral("libtest.so"));
    QVERIFY(info.getDependncies().contains("libfoo.so"));

    // Imported symbols get their version from .gnu.version_r (verneed)
    QCOMPARE(info.getRequiredSymbols(),
             (QSet<QString>{QStringLiteral("qDebug@GLIBC_2.14"),
                            QStringLiteral("memcpy@GLIBC_2.2.5")}));

    // Exported symbols: the versioned one gets its version from
    // .gnu.version_d (verdef), the unversioned one stays plain
    QCOMPARE(info.getProvidedSymbols(),
             (QSet<QString>{QStringLiteral("myexport@MYLIB_1.0"),
                            QStringLiteral("other")}));

    // Defined version nodes are collected separately
    QCOMPARE(info.getProvidedVersions(),
             (QSet<QString>{QStringLiteral("MYLIB_1.0")}));
}

void ElfTest::parseUnversionedExports()
{
    QTemporaryDir tmp;
    QVERIFY(tmp.isValid());

    const QString path = tmp.filePath("libtest.so");
    {
        QFile file(path);
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write(buildTestElf());
    }

    ELF elfParser;
    LibInfo info;
    QVERIFY(elfParser.getLibInfo(path, info));

    // "other" is a plain exported symbol (no @version suffix)
    QVERIFY(info.getProvidedSymbols().contains(QStringLiteral("other")));
    // The null symbol and the exported symbols must not pollute the
    // required-symbols set (exactly the two imported symbols remain)
    QCOMPARE(info.getRequiredSymbols().size(), 2);
}

void ElfTest::parseInvalidFile()
{
    QTemporaryDir tmp;
    QVERIFY(tmp.isValid());

    const QString path = tmp.filePath("garbage.so");
    {
        QFile file(path);
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write(QByteArray(256, 'x'));
    }

    ELF elfParser;
    LibInfo info;
    QVERIFY(!elfParser.getLibInfo(path, info));
}

// Standard stand-alone QTest entry point, see the Qt test tutorial:
// https://doc.qt.io/qt-6/qttestlib-tutorial1-example.html
QTEST_GUILESS_MAIN(ElfTest)

