//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "installdiroptionstest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void InstallDirsOptionsTest::test() {
#ifdef QT_DEBUG
#ifdef Q_OS_UNIX
    QStringList binMulti = {TestBinDir + "TestOnlyC" , TestBinDir + "TestCPPOnly"};

#else
    QStringList binMulti = {TestBinDir + "TestOnlyC.exe" , TestBinDir + "TestCPPOnly.exe"};

#endif


    runTestParams({"-bin", binMulti.join(","), "clear",
                   "qif", "deb",
                   "-targetPackage", "pkg;TestCPPOnly",
                   "-installDirDeb", "pkg;/var",
                   "-installDirQIFW", "/opt"});

#endif
}
