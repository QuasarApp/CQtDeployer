//#
//# Copyright (C) 2020-2026 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#include "peplatformtest.h"

#include <deploycore.h>
#include <ignorerule.h>
#include <libinfo.h>
#include <pe_type.h>

#include <QFile>
#include <QTemporaryDir>
#include <QVector>

PePlatformTest::PePlatformTest() {

}

PePlatformTest::~PePlatformTest() {

}

namespace {

struct PeBytes {
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
    }
};

} // namespace

/**
 * @brief buildTestPe Generates a minimal valid PE32/PE32+ file with a single
 * ".text" section containing an import table with the given DLL module names
 * (one "func" import per module).
 */
QByteArray PePlatformTest::buildTestPe(const QStringList &dllNames, bool pe64) {
    static const int eLfanew = 0x80;
    static const int headersSize = 0x200;
    static const int secRva = 0x1000;
    static const int secRawSize = 0x200;
    static const int secRawOff = 0x200;

    const int n = dllNames.size();
    const int entrySize = pe64 ? 8 : 4;
    const int optSize = pe64 ? 240 : 224;

    // Section-relative layout of the import data
    const int descsOff = 0;
    const int descsSize = (n + 1) * 20;
    const int iltOff = descsOff + descsSize;
    const int iltSize = (n + 1) * entrySize;
    const int iatOff = iltOff + iltSize;
    const int iatSize = iltSize;
    const int strOff = iatOff + iatSize;

    struct StrOffs {
        int importByName;
        int dllName;
    };
    QVector<StrOffs> offs;
    int cur = strOff;
    for (const QString &name : dllNames) {
        offs.push_back({cur, cur + 7}); // hint(2) + "func\0"
        cur += 7 + name.toUtf8().size() + 1;
    }
    Q_ASSERT(cur < secRawSize);

    auto rva = [](int off) { return secRva + off; };

    PeBytes b;
    // DOS header
    b.data.append("MZ");
    b.padTo(0x3c);
    b.w32(eLfanew);
    b.padTo(eLfanew);

    // PE signature + COFF header
    b.data.append(QByteArray("PE\0\0", 4));
    b.w16(pe64 ? 0x8664 : 0x14c); // Machine
    b.w16(1);                     // NumberOfSections
    b.w32(0);                     // TimeDateStamp
    b.w32(0);                     // PointerToSymbolTable
    b.w32(0);                     // NumberOfSymbols
    b.w16(optSize);               // SizeOfOptionalHeader
    b.w16(pe64 ? 0x0022 : 0x0102); // Characteristics

    // Optional header
    b.w16(pe64 ? 0x20b : 0x10b);  // Magic
    b.w8(0); b.w8(0);             // Linker version
    b.w32(secRawSize);            // SizeOfCode
    b.w32(0);                     // SizeOfInitializedData
    b.w32(0);                     // SizeOfUninitializedData
    b.w32(0);                     // AddressOfEntryPoint
    b.w32(secRva);                // BaseOfCode
    if (!pe64)
        b.w32(secRva);            // BaseOfData (PE32 only)
    if (pe64) {
        b.w64(0x140000000ULL);    // ImageBase
    } else {
        b.w32(0x400000);          // ImageBase
    }
    b.w32(0x1000);                // SectionAlignment
    b.w32(0x200);                 // FileAlignment
    b.w16(pe64 ? 6 : 4);          // MajorOperatingSystemVersion
    b.w16(0);                     // MinorOperatingSystemVersion
    b.w16(0); b.w16(0);           // Image version
    b.w16(pe64 ? 6 : 4);          // MajorSubsystemVersion
    b.w16(0);                     // MinorSubsystemVersion
    b.w32(0);                     // Win32VersionValue
    b.w32(0x2000);                // SizeOfImage
    b.w32(headersSize);           // SizeOfHeaders
    b.w32(0);                     // CheckSum
    b.w16(3);                     // Subsystem (Windows GUI)
    b.w16(0);                     // DllCharacteristics
    if (pe64) {
        b.w64(0x100000); b.w64(0x1000); // Stack
        b.w64(0x100000); b.w64(0x1000); // Heap
    } else {
        b.w32(0x100000); b.w32(0x1000); // Stack
        b.w32(0x100000); b.w32(0x1000); // Heap
    }
    b.w32(0);                     // LoaderFlags
    b.w32(16);                    // NumberOfRvaAndSizes
    for (int i = 0; i < 16; ++i) {
        if (i == 1) {            // Import data directory
            b.w32(rva(descsOff));
            b.w32(descsSize);
        } else {
            b.w32(0);
            b.w32(0);
        }
    }

    // Section header ".text"
    b.data.append(".text");
    b.data.append(QByteArray(3, '\0'));
    b.w32(secRva);                // VirtualSize
    b.w32(secRva);                // VirtualAddress
    b.w32(secRawSize);            // SizeOfRawData
    b.w32(secRawOff);             // PointerToRawData
    b.w32(0); b.w32(0);           // PointerToRelocations / PointerToLinenumbers
    b.w16(0); b.w16(0);           // NumberOfRelocations / NumberOfLinenumbers
    b.w32(0x60000020);            // Characteristics (code, init data, exec)

    // Import data
    b.padTo(headersSize);
    // Import descriptors
    for (int i = 0; i < n; ++i) {
        b.w32(rva(iltOff));             // OriginalFirstThunk
        b.w32(0);                       // TimeDateStamp
        b.w32(0);                       // ForwarderName
        b.w32(rva(offs[i].dllName));    // Name
        b.w32(rva(iatOff));             // FirstThunk
    }
    b.w32(0); b.w32(0); b.w32(0); b.w32(0); b.w32(0); // null descriptor
    // Import lookup table
    for (int i = 0; i < n; ++i) {
        if (pe64) b.w64(rva(offs[i].importByName));
        else b.w32(rva(offs[i].importByName));
    }
    if (pe64) b.w64(0); else b.w32(0);
    // Import address table
    for (int i = 0; i < n; ++i) {
        if (pe64) b.w64(rva(offs[i].importByName));
        else b.w32(rva(offs[i].importByName));
    }
    if (pe64) b.w64(0); else b.w32(0);
    // Strings
    for (int i = 0; i < n; ++i) {
        b.w16(0);                       // Hint
        b.data.append(QByteArray("func\0", 5));
        b.data.append(dllNames[i].toUtf8());
        b.data.append('\0');
    }
    b.padTo(secRawOff + secRawSize);

    return b.data;
}

void PePlatformTest::scanPlatform(const QTemporaryDir &dir,
                                  const QString &fileName,
                                  const QStringList &dllNames,
                                  bool pe64,
                                  Platform &platform) {
    const QString path = dir.filePath(fileName);
    {
        QFile f(path);
        QVERIFY2(f.open(QIODevice::WriteOnly),
                 qPrintable("cannot write " + path));
        const QByteArray pe = buildTestPe(dllNames, pe64);
        QCOMPARE(f.write(pe), qint64(pe.size()));
        f.close();
    }

    PE peScaner;
    LibInfo info;
    const bool ok = peScaner.getLibInfo(path, info);
    QVERIFY2(ok,
             qPrintable("PE parse failed for " + path + " importing: " +
                        dllNames.join(", ")));
    platform = info.getPlatform();
}

void PePlatformTest::test() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    // MinGW app linked against the MSVC CRT (PE32+)
    Platform platform = UnknownPlatform;
    scanPlatform(dir, "app.exe",
                 {"msvcrt.dll", "KERNEL32.dll",
                  "libgcc_s_seh-1.dll", "libstdc++-6.dll"}, true, platform);
    QCOMPARE(platform, Platform::Win64_MinGW_Msvcrt);

    // MinGW app linked against the UCRT (PE32+)
    scanPlatform(dir, "app.exe",
                 {"api-ms-win-crt-stdio-l1-1-0.dll",
                  "api-ms-win-crt-runtime-l1-1-0.dll",
                  "KERNEL32.dll",
                  "libgcc_s_seh-1.dll", "libstdc++-6.dll"}, true, platform);
    QCOMPARE(platform, Platform::Win64_MinGW_Ucrt);

    // MinGW app linked against the MSVC CRT (PE32)
    scanPlatform(dir, "app.exe",
                 {"msvcrt.dll", "KERNEL32.dll",
                  "libwinpthread-1.dll"}, false, platform);
    QCOMPARE(platform, Platform::Win32_MinGW_Msvcrt);

    // MSVC app: imports api-ms-win-core-* but no CRT module and no MinGW
    // runtime module, so it keeps the generic platform
    scanPlatform(dir, "app.exe",
                 {"api-ms-win-core-synch-l1-2-0.dll",
                  "KERNEL32.dll", "USER32.dll",
                  "VCRUNTIME140.dll"}, true, platform);
    QCOMPARE(platform, Platform::Win64);

    // MSVC app linked against the MSVC CRT: imports msvcrt.dll but no MinGW
    // runtime module, so it must NOT be tagged as MinGW
    scanPlatform(dir, "app.exe",
                 {"msvcrt.dll", "KERNEL32.dll"}, true, platform);
    QCOMPARE(platform, Platform::Win64);

    // MinGW runtime library recognized by its file name (MSVC CRT build)
    scanPlatform(dir, "libwinpthread-1.dll",
                 {"msvcrt.dll", "KERNEL32.dll"}, true, platform);
    QCOMPARE(platform, Platform::Win64_MinGW_Msvcrt);

    // MinGW runtime library recognized by its file name (UCRT build)
    scanPlatform(dir, "libwinpthread-1.dll",
                 {"api-ms-win-crt-runtime-l1-1-0.dll",
                  "KERNEL32.dll"}, true, platform);
    QCOMPARE(platform, Platform::Win64_MinGW_Ucrt);

    // MinGW runtime library stdc++ (MSVC CRT build)
    scanPlatform(dir, "libstdc++-6.dll",
                 {"msvcrt.dll", "libgcc_s_seh-1.dll",
                  "libwinpthread-1.dll", "KERNEL32.dll"}, true, platform);
    QCOMPARE(platform, Platform::Win64_MinGW_Msvcrt);

    // Dependency platform compatibility rules
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64_MinGW_Msvcrt,
                                              Platform::Win64_MinGW_Msvcrt), true);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64_MinGW_Ucrt,
                                              Platform::Win64_MinGW_Ucrt), true);
    // MSVCRT and UCRT MinGW runtime sets must never be mixed
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64_MinGW_Ucrt,
                                              Platform::Win64_MinGW_Msvcrt), false);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64_MinGW_Msvcrt,
                                              Platform::Win64_MinGW_Ucrt), false);
    // Generic (non flavored) platforms stay compatible with the same bitness
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64_MinGW_Msvcrt,
                                              Platform::Win64), true);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64,
                                              Platform::Win64_MinGW_Msvcrt), true);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64,
                                              Platform::Win64_MinGW_Ucrt), true);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64,
                                              Platform::Win64), true);
    // Different bitness or architecture stays incompatible
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win32_MinGW_Msvcrt,
                                              Platform::Win64_MinGW_Msvcrt), false);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64_MinGW_Msvcrt,
                                              Platform::Win_ARM_64), false);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64,
                                              Platform::Win32), false);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::UnknownPlatform,
                                              Platform::Win64), false);

    // IgnoreRule platform matching for the MinGW runtime flavors
    {
        LibInfo msvcrtLib;
        msvcrtLib.setName("libstdc++-6.dll");
        msvcrtLib.setPath("/opt/mingw/bin");
        msvcrtLib.setPlatform(Platform::Win64_MinGW_Msvcrt);

        LibInfo ucrtLib = msvcrtLib;
        ucrtLib.setPlatform(Platform::Win64_MinGW_Ucrt);

        IgnoreData win64Rule("libstdc++");
        win64Rule.platform = Platform::Win64;
        IgnoreRule win64Rules;
        win64Rules.addRule(win64Rule);
        // A generic Windows rule must match both MinGW flavors
        QVERIFY2(win64Rules.isIgnore(msvcrtLib) != nullptr,
                 "generic Win64 rule must match MSVCRT MinGW lib");
        QVERIFY2(win64Rules.isIgnore(ucrtLib) != nullptr,
                 "generic Win64 rule must match UCRT MinGW lib");

        IgnoreData msvcrtRule("libstdc++");
        msvcrtRule.platform = Platform::Win64_MinGW_Msvcrt;
        IgnoreRule msvcrtRules;
        msvcrtRules.addRule(msvcrtRule);
        // A flavored rule must only match the identical flavor
        QVERIFY2(msvcrtRules.isIgnore(msvcrtLib) != nullptr,
                 "MSVCRT rule must match MSVCRT MinGW lib");
        QVERIFY2(msvcrtRules.isIgnore(ucrtLib) == nullptr,
                 "MSVCRT rule must NOT match UCRT MinGW lib");

        IgnoreData ucrtRule("libstdc++");
        ucrtRule.platform = Platform::Win64_MinGW_Ucrt;
        IgnoreRule ucrtRules;
        ucrtRules.addRule(ucrtRule);
        QVERIFY2(ucrtRules.isIgnore(ucrtLib) != nullptr,
                 "UCRT rule must match UCRT MinGW lib");
        QVERIFY2(ucrtRules.isIgnore(msvcrtLib) == nullptr,
                 "UCRT rule must NOT match MSVCRT MinGW lib");
    }

    // Platform string conversion
    QCOMPARE(DeployCore::platformToString(Platform::Win64_MinGW_Msvcrt),
             QStringLiteral("win_x86_64, MinGW_Msvcrt"));
    QCOMPARE(DeployCore::platformToString(Platform::Win32_MinGW_Ucrt),
             QStringLiteral("win_x86, MinGW_Ucrt"));
    QCOMPARE(DeployCore::platformToString(Platform::Win64),
             QStringLiteral("win_x86_64"));
    QCOMPARE(DeployCore::getPlatformFromString("win_x86_64_msvcrt"),
             Platform::Win64_MinGW_Msvcrt);
    QCOMPARE(DeployCore::getPlatformFromString("win_x86_ucrt"),
             Platform::Win32_MinGW_Ucrt);
    QCOMPARE(DeployCore::getPlatformFromString("win_x86_64"),
             Platform::Win64);
}