# CQtDeployer for macOS

CQtDeployer supports macOS (Apple Silicon / ARM64 and Intel / x86_64) as a host and a deploy target.

## Platform support

* Host platforms:
    - Linux
    - Windows
    - macOS (ARM64 and x86_64)

* Deploy targets:
    - Linux (ELF binaries)
    - Windows (PE binaries)
    - macOS (Mach-O binaries)

Mach-O binaries are parsed with the full file parsing on the LIEF backend, so
the dependency scanning (the linked dylibs and the Qt frameworks) works on
every host platform. A full deploy of a macOS application requires the qmake
of a macOS Qt build, so run the deploy on a macOS host.

## Install CQtDeployer on macOS

- Download the `CQtDeployer_*_dmg` archive from the [releases page](https://github.com/QuasarApp/CQtDeployer/releases).
- Open the DMG file and move the `CQtDeployer.app` bundle into the `Applications` folder.

``` bash
open CQtDeployer_*.dmg
cp -R /Volumes/CQtDeployer/CQtDeployer.app /Applications/
```

To run cqtdeployer from the command line, add a symbolic link to the bundle binary:

``` bash
ln -s /Applications/CQtDeployer.app/Contents/MacOS/CQtDeployer /usr/local/bin/cqtdeployer
```

Alternatively, you can build cqtdeployer from source - see the [Build and Install](Build-and-Install.md) page (the Build for macOS section).

## How to use

### Deploy a macOS application

``` bash
cqtdeployer -bin myApp -qmake /Users/<user>/Qt/6.10.3/macos/bin/qmake
```

If your application uses QML, add the qmlDir option:

``` bash
cqtdeployer -bin myApp -qmake /Users/<user>/Qt/6.10.3/macos/bin/qmake -qmlDir /path/to/my/qml/sources
```

If you use extra libraries, add the libDir option:

``` bash
cqtdeployer -bin myApp -qmake /Users/<user>/Qt/6.10.3/macos/bin/qmake -libDir /PathToMyExtraLibs -recursiveDepth 5
```

To create a portable archive of the deployed application, add the `zip` packager option:

``` bash
cqtdeployer -bin myApp -qmake /Users/<user>/Qt/6.10.3/macos/bin/qmake -qmlDir /path/to/my/qml/sources zip
```

## How the macOS deployment works

- CQtDeployer detects the Mach-O binaries (thin and fat binaries are supported) with the full file parsing on the LIEF backend and extracts the linked dylibs and the Qt frameworks.
- The deployed binaries are **not rewritten**: the original install names are kept. The generated run script points the macOS dynamic loader to the deployed frameworks and dylibs with the `DYLD_FRAMEWORK_PATH` and `DYLD_LIBRARY_PATH` environment variables.
- The distribution is launched with the generated run script (the same way as on Linux).

## Limitations

- The Qt Installer Framework (QIF) installer is not available on macOS - the QIFW tools (binarycreator) are not published for this platform. If you run cqtdeployer with the `qif` option on a macOS host, the QIF step is skipped gracefully and the remaining deployment steps are executed. Use the `zip` packager to create a portable archive of the deployed application.