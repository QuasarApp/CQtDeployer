/*
 * Copyright (C) 2018-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "libcreator.h"

#include <QFile>
#include <QFileInfo>
#include <dependenciesscanner.h>

const QMap<QString, Platform> &LibCreator::getLibplatform() const {
    return libplatform;
}

void LibCreator::createLib(const QString &resLib,
                           const QStringList &dep,
                           Platform platform) {
    QFile lib(resLib);

    if (lib.open(QIODevice::ReadOnly)) {

        QFile target(path + "/" + QFileInfo(resLib).fileName());

        if (target.open(QIODevice::ReadWrite)) {

            auto data = lib.readAll();
            target.write(data.data(), data.size());

            target.close();

            copyedLibs.push_back(target.fileName());
            libDep.insert(target.fileName(), dep);
            libplatform.insert(target.fileName(), platform);
        }

        lib.close();
    }
}

void LibCreator::initLinux64() {
    createLib(":/linux64", {
                  "libQuasarApp.so.1",
                  "libServerProtocol.so.1",
                  "libQt5Network.so.5",
                  "libQt5Core.so.5",
                  "libstdc++.so.6",
                  "libgcc_s.so.1",
                  "libc.so.6",
              },
              Platform::Unix_x86_64);
    createLib(":/linux64.so", {
                  "libQt5Core.so.5",
                  "libpthread.so.0",
                  "libstdc++.so.6",
                  "libm.so.6",
                  "libgcc_s.so.1",
                  "libc.so.6",

              },
              Platform::Unix_x86_64);

}

void LibCreator::initWin32() {
    createLib(":/win32mingw.dll", {
                  "libgcc_s_dw2-1.dll",
                  "KERNEL32.dll",
                  "msvcrt.dll",
                  "libGLESv2.dll",
              },
              Platform::Win32 );
    createLib(":/win32mingw.exe",{
                  "Qt5Core.dll",
                  "Qt5Gui.dll",
                  "Qt5Qml.dll",
                  "Qt5Widgets.dll",
                  "libgcc_s_dw2-1.dll",
                  "KERNEL32.dll",
                  "msvcrt.dll",
                  "SHELL32.dll",
                  "libstdc++-6.dll",
              },
              Platform::Win32
              );

    createLib(":/win32msvc.dll",{
                  "ole32.dll",
                  "OLEAUT32.dll",
                  "WINMM.dll",
                  "Qt5Multimedia.dll",
                  "Qt5Core.dll",
                  "MSVCP120.dll",
                  "MSVCR120.dll",
                  "KERNEL32.dll",
              },
              Platform::Win32);
    createLib(":/win32msvc.exe", {
                  "Qt5Core.dll",
                  "ViewFortis.dll",
                  "PocketProtocols.dll",
                  "ModelsFortis.dll",
                  "CommonBase.dll",
                  "Services.dll",
                  "SettingsMain.dll",
                  "CommonUtils.dll",
                  "CommonServices.dll",
                  "PFDF_Services.dll",
                  "DronTestControllers.dll",
                  "DronTestView.dll",
                  "DronTestModel.dll",
                  "DronTestSettings.dll",
                  "DronesProfiles.dll",
                  "Qt5Widgets.dll",
                  "Qt5Gui.dll",
                  "MSVCP120.dll",
                  "MSVCR120.dll",
                  "KERNEL32.dll",
                  "SHELL32.dll",

              },
              Platform::Win32);

}

void LibCreator::initWin64() {
    createLib(":/win64mingw.dll", {
                  "QuasarApp1.dll",
                  "Qt5Core.dll",
                  "libgcc_s_seh-1.dll",
                  "KERNEL32.dll",
                  "msvcrt.dll",
                  "SHLWAPI.dll",
                  "libstdc++-6.dll",
              },
              Platform::Win64);
    createLib(":/win64mingw.exe", {
                  "Deploy.dll",
                  "QuasarApp1.dll",
                  "Qt5Core.dll",
                  "libgcc_s_seh-1.dll",
                  "KERNEL32.dll",
                  "msvcrt.dll",
                  "libstdc++-6.dll",
              },
              Platform::Win64);
    createLib(":/win64msvc.dll", {
                  "Qt5Core.dll",
                  "MSVCP140.dll",
                  "KERNEL32.dll",
                  "VCRUNTIME140.dll",
                  "api-ms-win-crt-runtime-l1-1-0.dll",
                  "api-ms-win-crt-heap-l1-1-0.dll",
              },
              Platform::Win64);
    createLib(":/win64msvc.exe", {
                  "NetworkServiceEngine.dll",
                  "qtservice.dll",
                  "Qt5Core.dll",
                  "KERNEL32.dll",
                  "VCRUNTIME140.dll",
                  "api-ms-win-crt-heap-l1-1-0.dll",
                  "api-ms-win-crt-runtime-l1-1-0.dll",
                  "api-ms-win-crt-math-l1-1-0.dll",
                  "api-ms-win-crt-stdio-l1-1-0.dll",
                  "api-ms-win-crt-locale-l1-1-0.dll",
              },
              Platform::Win64);

}

LibCreator::LibCreator(const QString &path) {

    this->path = path;
    initWin32();
    initWin64();
    initLinux64();
}

const QStringList &LibCreator::getLibs() const {
    return copyedLibs;
}

const QMap<QString, QStringList>& LibCreator::getLibsDep() const {
    return libDep;
}

LibCreator::~LibCreator() {

    for(auto &lib : copyedLibs) {
        QFile::remove(lib);
    }

}
