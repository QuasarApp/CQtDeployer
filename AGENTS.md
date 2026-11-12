# AGENTS.md

Guidance for AI agents and automated coding assistants working in this
repository. Read this file before making any change to the code.

CQtDeployer is a cross-platform Qt C++ command line tool. It extracts all the
libraries an executable depends on, creates a launch script for the
application and generates standalone installers (Qt Installer Framework, DEB,
ZIP, AppImage and DMG) of the deployed application. It supports
cross-deployment: a project for one target platform can be deployed from a
host of a different platform (for example a Windows target on a Linux host).

## Maintainer

Andrei Yankovich (GitHub: [EndrII](https://github.com/EndrII)), QuasarApp.

- Repository: https://github.com/QuasarApp/CQtDeployer
- Documentation: https://quasarapp.github.io/CQtDeployer/
- License: LGPL-3.0 (see the `LICENSE` file)

## Project Architecture

The project is a CMake build that produces two components:

1. **The `Deploy` library** (`src/Deploy`, the CMake target `Deploy`) — the
   deployment engine:
   - `deploycore` / `deploy` — the core: it parses the target executable and
     the libraries it links to (the PE, ELF and Mach-O binary formats are
     parsed with the LIEF library), resolves the Qt libraries, QML modules and
     plugins that are needed, and copies them into the deployment directory
     together with the launch script.
   - `src/Deploy/src/Distributions/` — one class per output distribution,
     all based on the `idistribution` interface: `deb`, `qif` (the Qt
     Installer Framework), `ziparhive` (ZIP), `appimage`, `dmg`.
   - `src/Deploy/src/ScriptsTemplates/` — the launch scripts that are
     generated next to the deployed executable (`linux.sh`, `macos.sh`,
     `windows.bat`).
   - The supporting classes: `filemanager` / `metafilemanager` (the file
     copy and metadata handling), `pathutils`, `ignorerule` (the `-ignore`
     rules), `configparser` / `deployconfig` (the JSON configuration file),
     `envirement`, `zipcompresser`, `pluginsparser`, `qmlqt5` / `qmlqt6`
     (the QML import scanning), `libinfo`, `dependenciesscanner` /
     `dependencymap`, `targetinfo`, and the binary type detectors
     (`pe_type`, `elf_type`, `macho_type`).

2. **The `CQtDeployer` console tool** (`src/CQtDeployer`, the CMake target
   `CQtDeployer`) — the command line front-end: `main.cpp` parses the
   command line options (see `md/Options.md`) and the JSON configuration
   file, then runs the `Deploy` engine. The `src/CQtDeployer/Deploy` folder
   contains the self-deployment configuration of the tool itself
   (`CQtDeployer.json` for the multi-package QtIFW installer,
   `CQtDeployerSingle.json` for the flat DEB/ZIP/DMG distributions), the
   QIFW installer template (`QIFWTemplate/`) and the bundled Qt Installer
   Framework and AppImage binaries (`QIFW/binaries`, `AppImageSources`), so
   the `qtifw`, `appimage` and `dmg` options work on every host platform
   without an extra system installation.

Submodules (update them with `git submodule update --init --recursive`):

- `submodules/LIEF` — the LIEF library that parses the PE, ELF and Mach-O
  binaries. Third-party code, it is not modified by this project (the build
  only silences two of its compiler warnings, see the top level
  `CMakeLists.txt`).
- `submodules/QuasarAppLib` — the QuasarApp CMake helpers (`initAll`,
  `addTestsArg`, `initTests`, the documentation and CI targets) used by the
  build and the test system of this repository.
- `submodules/zip` — the zip archive writer used by the ZIP distribution.

The user facing documentation lives in `md/` (the option reference is
`md/Options.md`, the build instructions are `md/Build-and-Install.md`, the
changelog is `md/Changelog.md`). The `mkdocs.yml` and `doxygen.conf` files
drive the documentation builds (the `CQT_DEPLOYER_DOCS` option).

## Project Structure

```
CQtDeployer/
├── CMakeLists.txt              # Top level CMake (project CQtDeployer, C++17)
├── AGENTS.md                   # This file
├── README.md
├── CONTRIBUTING.md             # Contribution workflow and the code guidelines
├── SECURITY.md
├── LICENSE                     # LGPL-3.0
├── mkdocs.yml                  # The user documentation (md/) site configuration
├── doxygen.conf(.in)           # The Doxygen API documentation configuration
├── src/
│   ├── Deploy/                 # The Deploy shared library (the engine)
│   │   └── src/
│   │       ├── Distributions/  # deb, qif, ziparhive, appimage, dmg generators
│   │       ├── ScriptsTemplates/  # linux.sh, macos.sh, windows.bat launch scripts
│   │       └── ...             # deploycore, deploy, filemanager, pathutils, config, ...
│   └── CQtDeployer/            # The console tool (main.cpp, the command line parsing)
│       └── Deploy/             # self-deploy configs, QIFWTemplate, QIFW/AppImage binaries
├── tests/                      # The unit tests (one QTest executable per test class)
├── submodules/
│   ├── LIEF/                   # The PE/ELF/Mach-O parser (third party)
│   ├── QuasarAppLib/           # The QuasarApp CMake helpers
│   └── zip/                    # The zip archive writer
├── md/                         # The user documentation (Options.md, Build-and-Install.md, ...)
├── res/                        # The logos and banners
├── snap/                       # The snap package definition
├── Distro/                     # The build output directory of the self-deployment
└── .github/workflows/          # CI: pr.yml (PR build+test), cmake-multi-platform-build.yml
                                # (releases), deploy-docs.yml (the documentation site)
```

## Contribution Process

The full rules are in `CONTRIBUTING.md`. The workflow for contributors (and
for agents):

1. **Branching**: internal developers create a new branch from the version
   branch they work on (for example `v1.6`) unless the task instructs
   otherwise (in that case follow the task, for example branch from
   `main`). External developers fork the repository first and branch from
   the version branch. The task branches follow the `task_<issue number>`
   convention.
2. **Pull request**: the pull request must target the branch it was created
   from, it must reference the issue number it solves, it must contain a
   complete description of everything that was done, and while the work is
   still in progress its title must start with the `[WIP]` tag.
3. **Forbidden**: `git push --force` (all conflicts are solved by a new
   commit), pushing code directly to `main` or a version branch, moving the
   code for no particular reason, breaking the existing code style (if an
   aspect is not described, write in the same style as the code before the
   change).
4. **Code guidelines** (in order of importance): performance over style (a
   performance gain of more than 10 % justifies breaking a rule); no
   C-style casts; keep the `#include`s in the `.cpp` files where possible;
   use forward declarations for the pointer members of the classes; the
   template functions are described in the `.cpp` files; the class access
   specifiers are in the order `public`, `public slots`, `signals`,
   `protected`, `protected slots`, `private`, `private slots`; the 4-space
   shift and the opening bracket stay on the same line; run the tests before
   pushing the code.
5. **Continuous integration**: every pull request is built and tested on
   Windows (MinGW), Linux x86-64, Linux ARM64 and macOS with Qt 6.10.3
   (`.github/workflows/pr.yml`). The pull request is merged only after the
   CI checks pass.

## Building the Project

Requirements: CMake 3.18 or newer, a C++17 compiler (GCC or Clang; on
Windows the CI uses the MinGW 13 toolchain of Qt), Qt 6 (Qt 5 can build the
project, but the unit tests are only available with Qt 6).

Install Qt with [aqtinstall](https://github.com/michaelfisch/aqtinstall):

```bash
pip install aqtinstall
aqt install-qt linux desktop 6.10.3 gcc_64 -O ~/Qt
```

or with the system package manager (for example `sudo apt install
qt6-base-dev` on Ubuntu).

Build steps (Linux/macOS shell, the Windows CMD steps are in
`md/Build-and-Install.md`):

```bash
git clone https://github.com/QuasarApp/CQtDeployer.git
cd CQtDeployer
git submodule update --init --recursive

cmake -S . -B build -G Ninja \
      -DCMAKE_PREFIX_PATH=~/Qt/6.10.3/gcc_64 \
      -DCMAKE_BUILD_TYPE=Release \
      -DCQT_DEPLOYER_TESTS=ON
cmake --build build --parallel
cmake --install build        # optional
```

CMake options (see the top level `CMakeLists.txt`):

- `CQT_DEPLOYER_TESTS` — build the unit tests (default `OFF`, must be `ON`
  to run the test suite)
- `CQT_DEPLOYER_DOCS` — build the Doxygen documentation (default `OFF`)
- `CQT_DEPLOYER_TOOL` — build the console tool (default `ON`)
- `BUILD_SHARED_LIBS` — build the `Deploy` library as a shared library
  (default `ON`, forced `OFF` on iOS); set it to `OFF` to get a single
  static executable when cqtdeployer is not installed on the build machine
- `CMAKE_PREFIX_PATH` — the Qt root directory (for example
  `~/Qt/6.10.3/gcc_64` on Linux or `C:/Qt/6.10.3/mingw_64` on Windows)

The `make deploy` target (after `make install`) creates the installers and
packages of CQtDeployer itself with the installed `cqtdeployer` tool.

## Testing the Project

The unit tests are 16 stand-alone QTest executables in `tests/` (one per
test class, built with the `QTEST_GUILESS_MAIN` macro): AppImage,
DeployConfig, DeployCore, Dmg, DistroModule, DistroStruct, Envirement,
FileManager, IgnoreRule, LibInfo, MacOS, PathUtils, Pe, Qifw, TargetInfo and
ZipCompresser. The executables are registered with the `addTestsArg` helper
of the QuasarAppLib CMake targets: for every test it creates a
`deployTest<Name>` target that deploys the executable with the `cqtdeployer`
tool of the current build and a `test<Name>` target that runs the deployed
executable. Every test writes its text report into the `Testing` directory
of the source tree: `Testing/<Name>/test_report.txt`.

Run the whole test suite from the source directory (Qt 6 required, the same
command the CI uses):

```bash
cmake --build build --target test
```

Run a single test class the same way with its target name (for example
`cmake --build build --target testQifw`). The `Qifw`, `AppImage` and `Dmg`
tests exercise the `cqtdeployer` executable of the current build tree, so
they verify the code under review; the `copyQIFW` / `copyAppImage` targets
stage the bundled Qt Installer Framework and AppImage binaries next to it.
