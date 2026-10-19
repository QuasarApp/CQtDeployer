//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#include "petest.h"
#include "dependenciesscanner.h"
#include "libinfo.h"
#include "macho_type.h"
#include "pe_type.h"
#include <LIEF/PE.hpp>

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

/**
 * @brief buildTestPe Generates a minimal valid PE32/PE32+ file with a single
 * ".text" section containing an import table with the given DLL module names
 * (one "func" import per module). Optionally a delay import directory with
 * the given delay DLL module names is added.
 */
QByteArray buildTestPe(const QStringList &dllNames, bool pe64,
                       const QStringList &delayNames = {}) {
    static const int eLfanew = 0x80;
    static const int headersSize = 0x200;
    static const int secRva = 0x1000;
    static const int secRawSize = 0x200;
    static const int secRawOff = 0x200;

    const int n = dllNames.size();
    const int m = delayNames.size();
    const int entrySize = pe64 ? 8 : 4;
    const int optSize = pe64 ? 240 : 224;

    // Section-relative layout of the import data
    const int descsOff = 0;
    const int descsSize = (n + 1) * 20;
    const int iltOff = descsOff + descsSize;
    const int iltSize = (n + 1) * entrySize;
    const int iatOff = iltOff + iltSize;
    const int iatSize = iltSize;
    const int delayDescsOff = iatOff + iatSize;
    const int delayDescsSize = (m > 0) ? (m + 1) * 32 : 0;
    const int delayIltOff = delayDescsOff + delayDescsSize;
    const int delayIltSize = (m > 0) ? (m + 1) * entrySize : 0;
    const int delayIatOff = delayIltOff + delayIltSize;
    const int delayIatSize = (m > 0) ? (m + 1) * entrySize : 0;
    const int delayHandleOff = delayIatOff + delayIatSize;
    const int delayHandleSize = (m > 0) ? entrySize : 0;
    const int strOff = delayHandleOff + delayHandleSize;

    struct StrOffs {
        int importByName;
        int dllName;
    };
    QVector<StrOffs> offs;
    QVector<StrOffs> delayOffs;
    int cur = strOff;
    for (const QString &name : dllNames) {
        offs.push_back({cur, cur + 7}); // hint(2) + "func\0"
        cur += 7 + name.toUtf8().size() + 1;
    }
    for (const QString &name : delayNames) {
        delayOffs.push_back({cur, cur + 7}); // hint(2) + "func\0"
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
    b.w32(secRva);                // AddressOfEntryPoint
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
        } else if (i == 13 && m > 0) { // Delay import data directory
            b.w32(rva(delayDescsOff));
            b.w32(delayDescsSize);
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
    // Delay import descriptors (IMAGE_DELAYLOAD_INFO, 8 dwords each)
    if (m > 0) {
        for (int i = 0; i < m; ++i) {
            b.w32(0);                         // Attributes
            b.w32(rva(delayOffs[i].dllName)); // Name
            b.w32(rva(delayHandleOff));       // ModuleHandle
            b.w32(rva(delayIatOff));          // ProcedureTable (delay IAT)
            b.w32(rva(delayIltOff));          // DelayImportNameTable (delay ILT)
            b.w32(0); b.w32(0); b.w32(0);     // Bound tables + Unk3
        }
        for (int i = 0; i < 8; ++i)
            b.w32(0);                         // null descriptor
        // Delay import lookup table
        for (int i = 0; i < m; ++i) {
            if (pe64) b.w64(rva(delayOffs[i].importByName));
            else b.w32(rva(delayOffs[i].importByName));
        }
        if (pe64) b.w64(0); else b.w32(0);
        // Delay import address table
        for (int i = 0; i < m; ++i) {
            if (pe64) b.w64(rva(delayOffs[i].importByName));
            else b.w32(rva(delayOffs[i].importByName));
        }
        if (pe64) b.w64(0); else b.w32(0);
        // Module handle slot
        if (pe64) b.w64(0); else b.w32(0);
    }
    // Strings
    for (const QStringList &names : {dllNames, delayNames}) {
        for (int i = 0; i < names.size(); ++i) {
            b.w16(0);                       // Hint
            b.data.append(QByteArray("func\0", 5));
            b.data.append(names[i].toUtf8());
            b.data.append('\0');
        }
    }
    b.padTo(secRawOff + secRawSize);

    return b.data;
}

/**
 * @brief buildTestMachO Generates a minimal valid Mach-O 64-bit binary
 * (mach_header_64) with one LC_LOAD_DYLIB load command per given dylib name.
 */
QByteArray buildTestMachO(const QStringList &dylibs, bool arm64) {
    static const int headerSize = 32;

    PeBytes b;
    // mach_header_64
    b.w32(0xFEEDFACF);                          // magic (MH_MAGIC_64)
    b.w32(arm64 ? 0x0100000C : 0x01000007);     // cputype (ARM64 / X86_64)
    b.w32(0);                                   // cpusubtype
    b.w32(2);                                   // filetype (MH_DYLIB)
    b.w32(dylibs.size());                       // ncmds
    int sizeofcmds = 0;
    for (const QString &dylib : dylibs)
        sizeofcmds += 24 + ((dylib.toUtf8().size() + 1 + 7) / 8) * 8;
    b.w32(sizeofcmds);                          // sizeofcmds
    b.w32(0);                                   // flags
    b.w32(0);                                   // reserved

    // Load commands (LC_LOAD_DYLIB)
    for (const QString &dylib : dylibs) {
        const int nameSize = dylib.toUtf8().size() + 1;
        const int cmdsize = 24 + ((nameSize + 7) / 8) * 8;
        const int cmdStart = b.data.size();

        b.w32(0x0000000C);      // cmd (LC_LOAD_DYLIB)
        b.w32(cmdsize);         // cmdsize
        b.w32(24);              // name offset (relative to the command)
        b.w32(0);               // timestamp
        b.w32(0x00010000);      // current_version (1.0.0)
        b.w32(0x00000001);      // compat_version (0.0.1)
        b.data.append(dylib.toUtf8());
        b.data.append('\0');

        while (b.data.size() - cmdStart < cmdsize)
            b.data.append('\0');
    }

    Q_ASSERT(b.data.size() == headerSize + sizeofcmds);
    return b.data;
}

// Writes a generated PE fixture into @p dir and returns the platform the
// deployer assigns to it during the PE parsing stage.
bool scanPlatform(const QTemporaryDir &dir,
                  const QString &fileName,
                  const QStringList &dllNames,
                  bool pe64,
                  Platform &platform) {
    const QString path = dir.filePath(fileName);
    {
        QFile f(path);
        if (!f.open(QIODevice::WriteOnly))
            return false;
        const QByteArray pe = buildTestPe(dllNames, pe64);
        if (f.write(pe) != pe.size())
            return false;
        f.close();
    }

    PE peScaner;
    LibInfo info;
    if (!peScaner.getLibInfo(path, info))
        return false;
    platform = info.getPlatform();
    return true;
}

// Writes a generated Mach-O fixture into @p dir and returns its path.
QString writeMachO(const QTemporaryDir &dir, const QString &fileName,
                   const QStringList &dylibs, bool arm64) {
    const QString path = dir.filePath(fileName);
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly))
        return {};
    const QByteArray macho = buildTestMachO(dylibs, arm64);
    if (f.write(macho) != macho.size())
        return {};
    f.close();
    return path;
}

} // namespace

void PeTest::parseValidFile() {
#if defined(Q_OS_MACOS)
    // The cross deployment for the Windows targets is supported from
    // the Linux hosts only, a macOS host can not deploy for the Windows
    // targets, so the PE tests are not useful there (issue #166).
    QSKIP("The Windows (PE) deployment tests are not supported on the macOS hosts");
#endif
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString path = dir.filePath("app.exe");
    {
        QFile f(path);
        QVERIFY(f.open(QIODevice::WriteOnly));
        const QByteArray pe = buildTestPe({"KERNEL32.dll", "USER32.dll"}, true);
        QCOMPARE(f.write(pe), qint64(pe.size()));
        f.close();
    }

    auto pe = LIEF::PE::Parser::parse(path.toStdString());
    QVERIFY2(pe != nullptr, "LIEF was unable to parse the generated PE fixture");
    if (pe == nullptr)
        return;

    // The fixture is a PE32+ x86-64 binary.
    QCOMPARE(static_cast<int>(pe->header().machine()),
             static_cast<int>(LIEF::PE::Header::MACHINE_TYPES::AMD64));
    QCOMPARE(pe->optional_header().magic(), LIEF::PE::PE_TYPE::PE32_PLUS);

    // The section table contains the single .text section.
    QCOMPARE(pe->sections().size(), 1u);
    QCOMPARE(pe->sections()[0].name(), QString(".text"));

    // The import table lists the imported modules.
    QSet<QString> modules;
    for (const auto &import : pe->imports()) {
        modules.insert(QString::fromStdString(import.name()));
    }
    QVERIFY(modules.contains("KERNEL32.dll"));
    QVERIFY(modules.contains("USER32.dll"));

    QCOMPARE(pe->entrypoint(), 0x140001000ull); // imagebase + offset
}

void PeTest::parseViaDeployWrapper() {
#if defined(Q_OS_MACOS)
    // The cross deployment for the Windows targets is supported from
    // the Linux hosts only, a macOS host can not deploy for the Windows
    // targets, so the PE tests are not useful there (issue #166).
    QSKIP("The Windows (PE) deployment tests are not supported on the macOS hosts");
#endif
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString path = dir.filePath("app.exe");
    {
        QFile f(path);
        QVERIFY(f.open(QIODevice::WriteOnly));
        const QByteArray pe = buildTestPe({"KERNEL32.dll"}, true);
        QCOMPARE(f.write(pe), qint64(pe.size()));
        f.close();
    }

    PE peParser;
    LibInfo info;
    QVERIFY(peParser.getLibInfo(path, info));

    QVERIFY(info.isValid());
    QCOMPARE(info.getName(), QString("app.exe"));
    QCOMPARE(info.getPlatform(), Win64);
    QCOMPARE(info.getWinApi(), WinAPI::NoWinAPI);

    // The fixture imports KERNEL32.
    QVERIFY(info.getDependncies().contains("KERNEL32.dll"));
}

void PeTest::parseDelayImports() {
#if defined(Q_OS_MACOS)
    // The cross deployment for the Windows targets is supported from
    // the Linux hosts only, a macOS host can not deploy for the Windows
    // targets, so the PE tests are not useful there (issue #166).
    QSKIP("The Windows (PE) deployment tests are not supported on the macOS hosts");
#endif
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString path = dir.filePath("delay.exe");
    {
        QFile f(path);
        QVERIFY(f.open(QIODevice::WriteOnly));
        const QByteArray pe = buildTestPe({"KERNEL32.dll"}, true, {"ADVAPI32.dll"});
        QCOMPARE(f.write(pe), qint64(pe.size()));
        f.close();
    }

    // LIEF exposes the delay import directory separately from the standard
    // import table.
    auto pe = LIEF::PE::Parser::parse(path.toStdString());
    QVERIFY2(pe != nullptr, "LIEF was unable to parse the delay import PE fixture");
    if (pe != nullptr) {
        QSet<QString> delay;
        for (const auto &delayImport : pe->delay_imports()) {
            delay.insert(QString::fromStdString(delayImport.name()));
        }
        QCOMPARE(delay.size(), 1u);
        QVERIFY(delay.contains("ADVAPI32.dll"));
    }

    // The deployer merges the delay imported modules into the dependencies.
    PE peParser;
    LibInfo info;
    QVERIFY(peParser.getLibInfo(path, info));
    QVERIFY(info.getDependncies().contains("KERNEL32.dll"));
    QVERIFY(info.getDependncies().contains("ADVAPI32.dll"));
}

void PeTest::parseInvalidFile() {
#if defined(Q_OS_MACOS)
    // The cross deployment for the Windows targets is supported from
    // the Linux hosts only, a macOS host can not deploy for the Windows
    // targets, so the PE tests are not useful there (issue #166).
    QSKIP("The Windows (PE) deployment tests are not supported on the macOS hosts");
#endif
    QTemporaryFile tmp;
    QVERIFY(tmp.open());
    tmp.write("this is not a PE file");
    tmp.close();

    auto pe = LIEF::PE::Parser::parse(tmp.fileName().toStdString());
    QVERIFY(pe == nullptr);

    PE peParser;
    LibInfo info;
    QVERIFY(!peParser.getLibInfo(tmp.fileName(), info));
}

void PeTest::parseMachO() {
#if defined(Q_OS_WIN)
    // The cross deployment for the macOS targets is supported from the
    // Linux hosts only, a Windows host can not deploy for the macOS
    // targets, so the Mach-O tests are not useful there (issue #166).
    QSKIP("The macOS (Mach-O) deployment tests are not supported on the Windows hosts");
#endif
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QStringList dylibs{"/usr/lib/libSystem.B.dylib", "libQt6Core.dylib"};

    // x86-64 slice
    const QString x64Path = writeMachO(dir, "libtest.dylib", dylibs, false);
    QVERIFY(!x64Path.isEmpty());

    MachO machoParser;
    LibInfo info;
    QVERIFY(machoParser.getLibInfo(x64Path, info));
    QVERIFY(info.isValid());
    QCOMPARE(info.getName(), QString("libtest.dylib"));
    QCOMPARE(info.getPlatform(), Mac_X86_64);
    for (const QString &dylib : dylibs)
        QVERIFY(info.getDependncies().contains(dylib));

    // ARM 64 slice
    const QString armPath = writeMachO(dir, "libtest_arm.dylib", dylibs, true);
    QVERIFY(!armPath.isEmpty());

    info.clear();
    QVERIFY(machoParser.getLibInfo(armPath, info));
    QCOMPARE(info.getPlatform(), Mac_ARM_64);
    for (const QString &dylib : dylibs)
        QVERIFY(info.getDependncies().contains(dylib));
}

void PeTest::parseMachORouting() {
#if defined(Q_OS_WIN)
    // The cross deployment for the macOS targets is supported from the
    // Linux hosts only, a Windows host can not deploy for the macOS
    // targets, so the Mach-O tests are not useful there (issue #166).
    QSKIP("The macOS (Mach-O) deployment tests are not supported on the Windows hosts");
#endif
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    // .dylib files are routed to the Mach-O scanner by the dependencies scanner
    const QString path = writeMachO(dir, "libtest.dylib",
                                    {"/usr/lib/libSystem.B.dylib"}, false);
    QVERIFY(!path.isEmpty());

    DependenciesScanner scanner;
    LibInfo info;
    QVERIFY(scanner.fillLibInfo(info, path));
    QCOMPARE(info.getPlatform(), Mac_X86_64);
    QVERIFY(info.getDependncies().contains("/usr/lib/libSystem.B.dylib"));
}

void PeTest::parseInvalidMachO() {
#if defined(Q_OS_WIN)
    // The cross deployment for the macOS targets is supported from the
    // Linux hosts only, a Windows host can not deploy for the macOS
    // targets, so the Mach-O tests are not useful there (issue #166).
    QSKIP("The macOS (Mach-O) deployment tests are not supported on the Windows hosts");
#endif
    QTemporaryFile tmp;
    QVERIFY(tmp.open());
    tmp.write("this is not a Mach-O file");
    tmp.close();

    MachO machoParser;
    LibInfo info;
    QVERIFY(!machoParser.getLibInfo(tmp.fileName(), info));
}
void PeTest::classifyMinGWFlavors() {
#if defined(Q_OS_MACOS)
    // The cross deployment for the Windows targets is supported from
    // the Linux hosts only, a macOS host can not deploy for the Windows
    // targets, so the PE tests are not useful there (issue #166).
    QSKIP("The Windows (PE) deployment tests are not supported on the macOS hosts");
#endif
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    Platform platform = UnknownPlatform;

    // MinGW app linked against the MSVC CRT (PE32+)
    QVERIFY(scanPlatform(dir, "app.exe",
                         {"msvcrt.dll", "KERNEL32.dll",
                          "libgcc_s_seh-1.dll", "libstdc++-6.dll"}, true, platform));
    QCOMPARE(platform, Platform::Win64_MinGW_Msvcrt);

    // MinGW app linked against the UCRT (PE32+)
    QVERIFY(scanPlatform(dir, "app.exe",
                         {"api-ms-win-crt-stdio-l1-1-0.dll",
                          "api-ms-win-crt-runtime-l1-1-0.dll",
                          "KERNEL32.dll",
                          "libgcc_s_seh-1.dll", "libstdc++-6.dll"}, true, platform));
    QCOMPARE(platform, Platform::Win64_MinGW_Ucrt);

    // MinGW app linked against the MSVC CRT (PE32)
    QVERIFY(scanPlatform(dir, "app.exe",
                         {"msvcrt.dll", "KERNEL32.dll",
                          "libwinpthread-1.dll"}, false, platform));
    QCOMPARE(platform, Platform::Win32_MinGW_Msvcrt);
}

void PeTest::classifyMSVCFlavor() {
#if defined(Q_OS_MACOS)
    // The cross deployment for the Windows targets is supported from
    // the Linux hosts only, a macOS host can not deploy for the Windows
    // targets, so the PE tests are not useful there (issue #166).
    QSKIP("The Windows (PE) deployment tests are not supported on the macOS hosts");
#endif
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    Platform platform = UnknownPlatform;

    // MSVC app (PE32+): imports the MSVC CRT (msvcr*.dll) and no MinGW
    // runtime module, so it gets the MSVC platform
    QVERIFY(scanPlatform(dir, "app.exe",
                         {"MSVCR140.dll", "msvcp140.dll", "KERNEL32.dll"}, true, platform));
    QCOMPARE(platform, Platform::Win64_MSVC);

    // MSVC app (PE32): older MSVC CRT version (msvcr100.dll)
    QVERIFY(scanPlatform(dir, "app.exe",
                         {"MSVCR100.dll", "KERNEL32.dll"}, false, platform));
    QCOMPARE(platform, Platform::Win32_MSVC);

    // MSVC app with the split CRT (msvcr140_1.dll)
    QVERIFY(scanPlatform(dir, "app.exe",
                         {"msvcr140_1.dll", "KERNEL32.dll"}, true, platform));
    QCOMPARE(platform, Platform::Win64_MSVC);
}

void PeTest::classifyPrecedenceAndGeneric() {
#if defined(Q_OS_MACOS)
    // The cross deployment for the Windows targets is supported from
    // the Linux hosts only, a macOS host can not deploy for the Windows
    // targets, so the PE tests are not useful there (issue #166).
    QSKIP("The Windows (PE) deployment tests are not supported on the macOS hosts");
#endif
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    Platform platform = UnknownPlatform;

    // A MinGW binary keeps the MinGW flavor even if it also imports the
    // MSVC CRT
    QVERIFY(scanPlatform(dir, "app.exe",
                         {"msvcrt.dll", "MSVCR140.dll", "libgcc_s_seh-1.dll"}, true, platform));
    QCOMPARE(platform, Platform::Win64_MinGW_Msvcrt);

    // MSVC app: imports api-ms-win-core-* and vcruntime140.dll, but no
    // CRT module and no MinGW runtime module, so it keeps the generic
    // platform (UCRT MSVC binaries cannot be told apart from other UCRT
    // binaries by their import table)
    QVERIFY(scanPlatform(dir, "app.exe",
                         {"api-ms-win-core-synch-l1-2-0.dll",
                          "KERNEL32.dll", "USER32.dll",
                          "VCRUNTIME140.dll"}, true, platform));
    QCOMPARE(platform, Platform::Win64);

    // msvcrt.dll alone is the legacy CRT of the MinGW toolchain, not a
    // toolchain marker: without a MinGW runtime module it keeps the
    // generic platform
    QVERIFY(scanPlatform(dir, "app.exe",
                         {"msvcrt.dll", "KERNEL32.dll"}, true, platform));
    QCOMPARE(platform, Platform::Win64);
}

void PeTest::recognizeMinGWRuntimes() {
#if defined(Q_OS_MACOS)
    // The cross deployment for the Windows targets is supported from
    // the Linux hosts only, a macOS host can not deploy for the Windows
    // targets, so the PE tests are not useful there (issue #166).
    QSKIP("The Windows (PE) deployment tests are not supported on the macOS hosts");
#endif
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    Platform platform = UnknownPlatform;

    // MinGW runtime library recognized by its file name (MSVC CRT build)
    QVERIFY(scanPlatform(dir, "libwinpthread-1.dll",
                         {"msvcrt.dll", "KERNEL32.dll"}, true, platform));
    QCOMPARE(platform, Platform::Win64_MinGW_Msvcrt);

    // MinGW runtime library recognized by its file name (UCRT build)
    QVERIFY(scanPlatform(dir, "libwinpthread-1.dll",
                         {"api-ms-win-crt-runtime-l1-1-0.dll",
                          "KERNEL32.dll"}, true, platform));
    QCOMPARE(platform, Platform::Win64_MinGW_Ucrt);

    // MinGW runtime library stdc++ (MSVC CRT build)
    QVERIFY(scanPlatform(dir, "libstdc++-6.dll",
                         {"msvcrt.dll", "libgcc_s_seh-1.dll",
                          "libwinpthread-1.dll", "KERNEL32.dll"}, true, platform));
    QCOMPARE(platform, Platform::Win64_MinGW_Msvcrt);
}

// Standard stand-alone QTest entry point, see the Qt test tutorial:
// https://doc.qt.io/qt-6/qttestlib-tutorial1-example.html
QTEST_GUILESS_MAIN(PeTest)
