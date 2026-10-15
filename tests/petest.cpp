//
// Copyright (C) 2026 QuasarApp.
// Distributed under the GPLv3 software license, see the accompanying
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