# Qt Installer Framework (IFW) binaries

Prebuilt `binarycreator` and `installerbase` executables of the Qt Installer
Framework **4.11.0** for every platform CQtDeployer is released for.

Since aqtinstall does not know IFW 4.8+ (miurahr/aqtinstall#792), these
binaries are stored in the repository instead of being downloaded at build
time (QuasarApp/CQtDeployer#867). The CMake build
(`src/CQtDeployer/CMakeLists.txt`, target `downloadQIFW`) simply copies the
platform-matching pair from this folder into `Deploy/QIFW/QIF`, which is
bundled into the installer (package `QIF`) and the flat zip packages.

## Files

| Platform (folder) | Files | Notes |
|---|---|---|
| `windows/` | `binarycreator.exe`, `installerbase.exe` | Windows x86_64 |
| `mac/` | `binarycreator`, `installerbase` | macOS universal (x86_64 + arm64) |
| `linux-x86_64/` | `binarycreator`, `installerbase` | Linux x86_64 |
| `linux-arm64/` | `binarycreator`, `installerbase` | Linux arm64 |

## Provenance (verified 2026-10-20)

Source: Qt online repository, package `qt.tools.ifw.411`
(version `4.11.0-0-202603231357`, macOS `4.11.0-0-202603311245`):

| Folder | Archive | Size (bytes) | SHA1 | SHA256 |
|---|---|---|---|---|
| `windows/` | `ifw-win-x64.7z` | 56804453 | `d8b7596463f3d3215db7eaa25545b5b54cf3b8e9` | `c47201c4f6a82a8b607daa245237f40831d78425e904edd1514b71fd17efefc1` |
| `mac/` | `ifw-mac-universal.7z` | 115139148 | `c2b4aade961e4664b7054da7806da158c0a17a85` | `140397cb4776a00a8efb76e5b8a362f8e6adc9a213da7a074e81b9b912f0c8f3` |
| `linux-x86_64/` | `ifw-linux-x64.7z` | 65327452 | `47f3199133a347c27a5063cbcdf526753e0d1abe` | `79ef906970c7e4be9ec15447fd5acb0a8ae2ab41aef5f9184086ce3972eddbc7` |
| `linux-arm64/` | `ifw-linux-arm64.7z` | 60989033 | `52681bcc8ac3c313ed7761e6416f3ebd4b24b181` | `6d02441a14bc14fea5555690b63a5d8d6ef262d7ef8eb1a009d1a541b2ef0adb` |

Archive URLs:

    https://download.qt.io/online/qtsdkrepository/<segment>/ifw/tools_ifw_411/qt.tools.ifw.411/<version><archive>

with `<segment>` = `windows_x86`, `mac_x64`, `linux_x64`, `linux_arm64`.

The SHA1/SHA256/size values above are the ones published by the Qt Project
in the Metalink4 files of the same repository
(`<archive>.meta4`), and all of them matched the downloaded archives.
Note: the `<SHA1>`/`<UpdateFile>` values inside the repository `Updates.xml`
files are stale (they do not match the published archives), so the Metalink
files are the source of truth.

Only `bin/binarycreator[.exe]` and `bin/installerbase[.exe]` of each archive
are stored here (the full IFW tree is not needed by CQtDeployer).

## Updating the binaries

To refresh the binaries to a newer IFW release:

1. Find the new `tools_ifw_4XX` folder and the `qt.tools.ifw.4XX` package in
   `https://download.qt.io/online/qtsdkrepository/<segment>/ifw/`
   (`Updates.xml` + Metalink files).
2. Download the four platform archives, verify the hashes from the Metalink
   files.
3. Extract `bin/binarycreator[.exe]` and `bin/installerbase[.exe]` into the
   matching platform folder of this directory.
4. Update `QIFW_VERSION` in the top-level `CMakeLists.txt` and the table
   above.
