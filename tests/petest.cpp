//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#include "petest.h"
#include "libinfo.h"
#include "pe_type.h"
#include <pe-parse/parse.h>

namespace {

int sectionCb(void *cbd, const peparse::VA &, const std::string &, const peparse::image_section_header &, const peparse::bounded_buffer *) {
    *static_cast<int *>(cbd) += 1;
    return 0;
}

int importCb(void *cbd, const peparse::VA &, const std::string &moduleName, const std::string &) {
    auto *modules = static_cast<QSet<QString> *>(cbd);
    *modules << QString::fromStdString(moduleName);
    return 0;
}

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
 * (one "func" import per module).
 */
QByteArray buildTestPe(const QStringList &dllNames, bool pe64) {
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

} // namespace

void PeTest::initTestCase() {
#ifdef PE_EXAMPLE_EXE
    m_exampleExe = QString(PE_EXAMPLE_EXE);
#else
    QSKIP("PE_EXAMPLE_EXE is not provided by CMake");
#endif
    if (!QFile::exists(m_exampleExe))
        QSKIP(qPrintable("PE fixture not found: " + m_exampleExe));
}

void PeTest::cleanupTestCase() {
}

void PeTest::parseValidFile() {
    auto *pe = peparse::ParsePEFromFile(m_exampleExe.toLatin1());
    QVERIFY2(pe != nullptr, qPrintable(QString::fromStdString(peparse::GetPEErrString())));
    if (pe == nullptr)
        return;

    // example.exe is a PE32+ x86-64 console application.
    QCOMPARE(QString(peparse::GetMachineAsString(pe)), QString("x64"));
    QCOMPARE(static_cast<unsigned int>(pe->peHeader.nt.FileHeader.Machine),
             static_cast<unsigned int>(peparse::IMAGE_FILE_MACHINE_AMD64));

    int sections = 0;
    peparse::IterSec(pe, sectionCb, &sections);
    QVERIFY(sections > 0);

    QSet<QString> modules;
    peparse::IterImpVAString(pe, importCb, &modules);
    QVERIFY(modules.size() > 0);
    // The fixture import table lists the module with the .DLL suffix.
    QVERIFY(modules.contains("KERNEL32.DLL"));

    peparse::VA entry = 0;
    QVERIFY(peparse::GetEntryPoint(pe, entry));
    QVERIFY(entry > 0);

    peparse::DestructParsedPE(pe);
}

void PeTest::parseViaDeployWrapper() {
    PE peParser;
    LibInfo info;
    QVERIFY(peParser.getLibInfo(m_exampleExe, info));

    QVERIFY(info.isValid());
    QCOMPARE(info.getName(), QString("example.exe"));
    QCOMPARE(info.getPlatform(), Win64);
    QCOMPARE(info.getWinApi(), WinAPI::NoWinAPI);

    // The fixture imports KERNEL32 (the module name keeps the .DLL suffix).
    QVERIFY(info.getDependncies().contains("KERNEL32.DLL"));
}

void PeTest::parseInvalidFile() {
    QTemporaryFile tmp;
    QVERIFY(tmp.open());
    tmp.write("this is not a PE file");
    tmp.close();

    auto *pe = peparse::ParsePEFromFile(tmp.fileName().toLatin1());
    QVERIFY(pe == nullptr);
}
void PeTest::classifyMinGWFlavors() {
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
