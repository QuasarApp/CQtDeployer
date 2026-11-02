# Install CQtDeployer

You can download the latest version of the application [here](https://github.com/QuasarApp/CQtDeployer/releases).

The release assets follow the naming convention `CQtDeployer_<version>_<platform>.<extension>`:

| Asset | Platform | Description |
|---|---|---|
| `CQtDeployer_<version>_Linux_x86_64.run` | Linux | The Qt Installer Framework installer of the CQtDeployer distribution |
| `CQtDeployer_<version>_Windows_AMD64.exe` | Windows | The Qt Installer Framework installer of the CQtDeployer distribution |
| `CQtDeployer_<version>_Linux_x86_64.AppImage` | Linux | The AppImage of the CQtDeployer distribution (starting with CQtDeployer 1.7) |
| `CQtDeployer_<version>_Linux_x86_64.deb` | Linux | The Debian package of the CQtDeployer distribution |
| `CQtDeployer_<version>_Linux_x86_64.snap` | Linux | The Snap package of the CQtDeployer distribution |
| `CQtDeployer_<version>_Linux_x86_64.zip` | Linux | The raw binary distribution of CQtDeployer (the executable, the QIF tools, the AppImage tools and the libraries) |
| `CQtDeployer_<version>_Windows_AMD64.zip` | Windows | The raw binary distribution of CQtDeployer |
| `CQtDeployer_<version>_Darwin_arm64.dmg` | macOS | The DMG of the CQtDeployer distribution (starting with CQtDeployer 1.7, the previous releases used the `CQtDeployer_<version>_Darwin_arm64.app.zip` archive of the app bundle) |
| `CQtDeployer_<version>_Darwin_arm64.zip` | macOS | The raw binary distribution of CQtDeployer |

The Qt Installer Framework tools and the AppImage tools are bundled within
the CQtDeployer distribution for all supported platforms (the QIFW binaries
are stored in the CQtDeployer repository, issue #867), so the `qtifw` option
works on every host platform, the `appimage` option on the Linux hosts and
the `dmg` option on the macOS hosts.

### Installer

- [Download](https://github.com/QuasarApp/CQtDeployer/releases) `CQtDeployer_<version>_Linux_x86_64.run` or `CQtDeployer_<version>_Windows_AMD64.exe`

#### Install without gui

##### Linux

``` bash
chmod +x CQtDeployer_<version>_Linux_x86_64.run
./CQtDeployer_<version>_Linux_x86_64.run install
```

##### Windows

``` bash
CQtDeployer_<version>_Windows_AMD64.exe install
```

##### macOS

Starting with CQtDeployer 1.7 the release asset for macOS is the DMG package
(`CQtDeployer_<version>_Darwin_arm64.dmg`). Open the DMG file and move the
`CQtDeployer.app` bundle into the `Applications` folder.

``` bash
open CQtDeployer_<version>_Darwin_arm64.dmg
cp -R /Volumes/CQtDeployer/CQtDeployer.app /Applications/
```

For the releases before 1.7 use the `CQtDeployer_<version>_Darwin_arm64.app.zip`
archive: extract it and move the `CQtDeployer.app` bundle into the
`Applications` folder.

To run cqtdeployer from the command line, add a symbolic link to the bundle binary:

``` bash
ln -s /Applications/CQtDeployer.app/Contents/MacOS/CQtDeployer /usr/local/bin/cqtdeployer
```

### AppImage

Available from CQtDeployer 1.7. Extract the AppImage and run it directly:

``` bash
chmod +x CQtDeployer_<version>_Linux_x86_64.AppImage
./CQtDeployer_<version>_Linux_x86_64.AppImage
```

The AppImage requires the FUSE support of the host. On the hosts without the
FUSE support (for example the minimal container images) extract the
distribution and run the executable from the `bin` folder:

``` bash
./CQtDeployer_<version>_Linux_x86_64.AppImage --appimage-extract
cd squashfs-root
./bin/CQtDeployer
```

### Deb

``` bash
sudo dpkg -i CQtDeployer_<version>_Linux_x86_64.deb
```

**Note** the package name and links can be changed, please check [Download](https://github.com/QuasarApp/CQtDeployer/releases) page before installation.

### Snap 

[![Get it from the Snap Store](https://snapcraft.io/static/images/badges/en/snap-store-black.svg)](https://snapcraft.io/cqtdeployer)

``` bash
sudo snap install cqtdeployer
```

#### Features of the snap version

If you are using the snap version of the application. Enable all permissions for cqtdeployer.
this can be done in the snap-store
Or starting with ubuntu 20.04 in the ubuntu application settings manager.

If you do not have the GUI then you can enable all permissions using next commands:

``` bash
sudo snap connect cqtdeployer:process-control
sudo snap connect cqtdeployer:removable-media
sudo snap connect cqtdeployer:system-backup
```

### GitHub Actions

To install CQtDeployer in the GitHub Actions pipelines use the
[cqtdeployer-install-action](https://github.com/QuasarApp/cqtdeployer-install-action)
action of the QuasarApp organization. The action downloads the CQtDeployer
release for the current runner (the Linux x64/ARM64, the Windows x64 and the
macOS ARM64 runners), extracts it to a temporary folder and adds it to the
`PATH` of the following steps:

```yaml
- name: Install CQtDeployer
  uses: QuasarApp/cqtdeployer-install-action@v1.2.3
```

By default the latest stable release is installed. To pin a specific release,
set the `version` input to the desired release tag:

```yaml
- name: Install a specific CQtDeployer version
  uses: QuasarApp/cqtdeployer-install-action@v1.2.3
  with:
    version: v1.6.2588
```

See the [GitHub Actions documentation](GitHubActions.md) for the full
description and the complete workflow example.

---

# Build from sources

## Build for Linux (using official qt binaries)

- install qt and qt QtInstallFrameWork from [qt installer](https://www.qt.io/download-qt-installer?hsCtaTracking=9f6a2170-a938-42df-a8e2-a9f0b1d6cdce%7C6cb0de4f9bb77-7bb77-4bb77-4)
- clone project 
 
    ``` bash
    git clone https://github.com/QuasarApp/CQtDeployer.git
    git submodule update --init --recursive
    cd CQtDeployer
    ```
    
- create temp build directory

    ``` bash
    mkdir build 
    cd build
    ```
    
- run cmake 

    ``` bash
    cmake .. -DCMAKE_PREFIX_PATH=/path/to/qt/root/dir
    ```
    
- build cqtdeployer 

    ``` bash
    make -j8
    ```
    
- create installers and packages (requires installed cqtdeployer)

    ``` bash
    make deploy 
    ```
    

## Build for Linux (using native OS binaries)

- clone project 
 
    ``` bash
    git clone https://github.com/QuasarApp/CQtDeployer.git
    git submodule update --init --recursive
    cd CQtDeployer
    ```
    
- create temp build directory

    ``` bash
    mkdir build 
    cd build
    ```
    
- Install qt from package manager (for example apt on ubuntu)

    ``` bash
    sudo apt install qt6-base-dev
    ```
    
- run cmake 

    ``` bash
    cmake .. -DCQT_DEPLOYER_TESTS=0
    ```
    
- build cqtdeployer 

    ``` bash
    cmake --build .. --parallel 
    ```
    
- create cqtdeployer executable.

    ``` bash
    make install 
    ```

## Build for Windows (CMD)

- install qt and qt QtInstallFrameWork from [qt installer](https://www.qt.io/download-qt-installer?hsCtaTracking=9f6a2170-a938-42df-a8e2-a9f0b1d6cdce%7C6cb0de4f9bb77-7bb77-4bb77-4)
- clone project 
 
    ``` bash
    git clone https://github.com/QuasarApp/CQtDeployer.git
    git submodule update --init --recursive
    cd CQtDeployer
    ```
    
- create temp build directory

    ``` bash
    mkdir build 
    cd build
    ```
- SET PATH=C:/Qt/Tools/mingw1310_64/bin;%PATH%
    - It is important to set up the qt environment (use the bin folder of the MinGW toolchain of your Qt installation, the CQtDeployer CI uses the `win64_mingw` build of Qt with the MinGW 13 toolchain).
   
- run cmake 

    ``` bash
    cmake .. -DCMAKE_PREFIX_PATH=C:/path/to/qt/root/dir
    ```
    
- build cqtdeployer 

    ``` bash
    mingw32-make -j8
    ```
    
- create installers and packages (requires installed cqtdeployer)

    ``` bash
    mingw32-make deploy 
    ```

## Build for macOS

- install qt from [qt installer](https://www.qt.io/download-qt-installer?hsCtaTracking=9f6a2170-a938-42df-a8e2-a9f0b1d6cdce%7C6cb0de4f9bb77-7bb77-4bb77-4) (the `clang_64` desktop build is a universal binary and works on both the Intel and the Apple Silicon machines)
- clone project 

    ``` bash
    git clone https://github.com/QuasarApp/CQtDeployer.git
    git submodule update --init --recursive
    cd CQtDeployer
    ```
    
- create temp build directory

    ``` bash
    mkdir build 
    cd build
    ```
    
- run cmake 

    ``` bash
    cmake .. -DCMAKE_PREFIX_PATH=/path/to/qt/root/dir
    ```
    
- build cqtdeployer 

    ``` bash
    cmake --build . --parallel 
    ```
    
- create cqtdeployer executable.

    ``` bash
    make install 
    ```

The Qt Installer Framework tools are stored in the CQtDeployer repository for all supported platforms (including macOS), so the QIF installer can be created on macOS as well. The release artifact for macOS is the DMG archive.

### Note 

If you do not have installed cqtdeployer on your build machine, you can compile cqtdeployer tool as a static. For this, disable the BUILD_SHARED_LIBS option.

```bash

cmake .. -DBUILD_SHARED_LIBS=0
make install

```

If you want to change Qt, Just run cmake with override qt location.

```bash

cmake .. -DCMAKE_PREFIX_PATH=/path/to/qt/root/dir
# or
cmake .. -DCMAKE_PREFIX_PATH=~/Qt/6.4.3/gcc_64

```
