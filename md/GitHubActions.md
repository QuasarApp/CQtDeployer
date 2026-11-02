# Using CQtDeployer in GitHub Actions

CQtDeployer can be used in the GitHub Actions pipelines. The QuasarApp
organization provides the [cqtdeployer-install-action](https://github.com/QuasarApp/cqtdeployer-install-action)
action that downloads, extracts and installs the CQtDeployer release for the
current runner, so the `cqtdeployer` command is available in the `PATH` of
all the following steps.

## Supported runners

| Runner | The release asset used |
|---|---|
| Linux x64 (for example `ubuntu-latest`, `ubuntu-22.04`) | `CQtDeployer_<version>_Linux_x86_64.zip` |
| Linux ARM64 (for example `ubuntu-24.04-arm`) | `CQtDeployer_<version>_Linux_ARM64.zip` |
| Windows x64 (for example `windows-latest`) | `CQtDeployer_<version>_Windows_AMD64.zip` |
| macOS ARM64 (for example `macos-latest`) | `CQtDeployer_<version>_Darwin_arm64.zip` |

The Qt Installer Framework tools and the AppImage tools of the CQtDeployer
distribution are made executable by the action, so the `qtifw` and the
`appimage` packaging options work in the pipelines without extra steps (the
zip archives do not always preserve the executable bit of the bundled tools,
issue #872, #884, #898 of QuasarApp/CQtDeployer).

## Basic usage

Add the following step to your workflow file (for example
`.github/workflows/build.yml`):

```yaml
- name: Install CQtDeployer
  uses: QuasarApp/cqtdeployer-install-action@v1.2.3
```

By default the step installs the latest stable CQtDeployer release.

## Options

| Option    | Type    | Default   | Description |
|-----------|---------|-----------|-------------|
| `latest`  | boolean | `true`    | Download the latest stable release. Only consulted when `version` is not set. |
| `version` | string  | *(empty)* | Download a specific release tag instead of the latest release, for example `v1.6.2588`. When set, it takes precedence over `latest`. |

To pin a specific release, set the `version` input to the desired release
tag:

```yaml
- name: Install a specific CQtDeployer version
  uses: QuasarApp/cqtdeployer-install-action@v1.2.3
  with:
    version: v1.6.2588
```

## Full workflow example

A complete example of a matrix build of a Qt application for Linux, Windows
and macOS with the CQtDeployer deployment step:

```yaml
name: CI Build and Deploy

on:
  push:
    branches: [ main ]
  pull_request:
    branches: [ main ]

env:
  # Leave empty to install the latest stable release,
  # or set an exact release tag (e.g. "v1.6.2588") to pin the version.
  CQT_VERSION: ""

jobs:
  build:
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
    runs-on: ${{ matrix.os }}

    steps:
      - name: Checkout Source Code
        uses: actions/checkout@v4

      # 1. Install Qt (the Qt of the application, for example 6.10.3).
      - name: Install Qt (Linux)
        if: runner.os == 'Linux'
        uses: jurplel/install-qt-action@v4.3.0
        with:
          version: '6.10.3'
          target: 'desktop'
          arch: 'linux_gcc_64'
          cache: true

      - name: Install Qt (Windows)
        if: runner.os == 'Windows'
        uses: jurplel/install-qt-action@v4.3.0
        with:
          version: '6.10.3'
          target: 'desktop'
          arch: 'win64_mingw'
          cache: true
          tools: 'tools_mingw1310'

      - name: Install Qt (macOS)
        if: runner.os == 'macOS'
        uses: jurplel/install-qt-action@v4.3.0
        with:
          version: '6.10.3'
          target: 'desktop'
          arch: 'clang_64'
          cache: true

      # 2. Build the application (the build system of your project).
      - name: Build Application
        shell: bash
        run: |
          mkdir -p build && cd build
          cmake .. -DCMAKE_BUILD_TYPE=Release
          cmake --build . --parallel

      # 3. Install CQtDeployer using this custom action.
      #    With an empty CQT_VERSION the latest stable release is installed.
      #    To pin a specific release, set CQT_VERSION to the exact release
      #    tag (e.g. "v1.6.2588") or pass "version: <tag>" directly.
      - name: Setup CQtDeployer
        uses: QuasarApp/cqtdeployer-install-action@v1.2.3
        with:
          version: ${{ env.CQT_VERSION }}

      # 4. Run the deployment for your Qt application.
      - name: Deploy Application (Linux / macOS)
        if: runner.os == 'Linux' || runner.os == 'macOS'
        shell: bash
        run: |
          cqtdeployer -bin build/my_qt_app \
            -qmake $QT_DIR/bin/qmake \
            -qmlDir ./src/qml \
            -libDir build \
            qtifw

      - name: Deploy Application (Windows)
        if: runner.os == 'Windows'
        shell: pwsh
        run: |
          cqtdeployer -bin build\my_qt_app.exe -qmake $env:QT_DIR\bin\qmake.exe -qmlDir .\src\qml -libDir build qtifw
```

The deployment creates the `DistributionKit` folder with the deployed
application and the installer (see the [how to deploy](Guide.md) article for
the full description of the deployment options).

## The CQtDeployer CI as a real-world example

The CQtDeployer project itself builds and deploys its releases on GitHub
Actions: the [CMake Release Builder
workflow](https://github.com/QuasarApp/CQtDeployer/blob/main/.github/workflows/cmake-multi-platform-build.yml)
installs Qt with the `jurplel/install-qt-action` action, installs
CQtDeployer with the `QuasarApp/cqtdeployer-install-action` action (pinned to
a specific release tag with the `version` input and the `latest: false`
option) and runs the `cqtdeployer` tool to create the release assets
(installer, deb, zip, AppImage and DMG packages) of the CQtDeployer
distribution.