# Install CQtDeployer

You can download the latest version of the application [here](https://github.com/QuasarApp/CQtDeployer/releases).

### Installer

- [Download](https://github.com/QuasarApp/CQtDeployer/releases) CQtDeployer_X.X.X.X_Installer_Linux64.run or CQtDeployer_X.X.X.X_Installer_Win64.exe

#### Install without gui

##### Linux

``` bash
wget https://github.com/QuasarApp/CQtDeployer/releases/download/1.5.3/CQtDeployer_1.5.3.0_Installer_Linux64.run
chmod +x CQtDeployer_1.5.3.0_Installer_Linux64.run
./CQtDeployer_1.5.3.0_Installer_Linux64.run install
```

##### Windows

``` bash
wget https://github.com/QuasarApp/CQtDeployer/releases/download/1.5.3/CQtDeployer_1.5.3.0_Installer_Win64.exe
CQtDeployer_1.5.3.0_Installer_Win64.exe install
```

### Deb

``` bash
wget https://github.com/QuasarApp/CQtDeployer/releases/download/1.5.3/CQtDeployer_1.5.3.0_Linux64.deb
sudo dpkg -i CQtDeployer_1.5.3.0_Linux64.deb
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

---

# Build from sources

## Build for Linux

- install qt and qt QtInstallFrameWork from [qt installer](https://www.qt.io/download-qt-installer?hsCtaTracking=9f6a2170-a938-42df-a8e2-a9f0b1d6cdce%7C6cb0de4f9bb77-7bb77-4bb77-4)
- git clone https://github.com/QuasarApp/CQtDeployer.git
- cd CQtDeployer
- git submodule update --init --recursive
- qmake -r
    - Here you must definitely call the qmake that was loaded from the 1st item.
    - Example: ~/Qt/5.15.0/gcc_64/bin/qmake -r
- make -j$(nproc)
- make deploy
    - this command requires installed [cqtdeployer](https://github.com/QuasarApp/CQtDeployer/releases)
- ./Distro/CQtDeployerInstaller.run

## Build for Windows (CMD)

- install qt and qt QtInstallFrameWork from [qt installer](https://www.qt.io/download-qt-installer?hsCtaTracking=9f6a2170-a938-42df-a8e2-a9f0b1d6cdce%7C6cb0de4f9bb77-7bb77-4bb77-4)
- git clone https://github.com/QuasarApp/CQtDeployer.git
- cd CQtDeployer
- git submodule update --init --recursive
- SET PATH=C:/Qt/Tools/mingw810_64/bin;%PATH%
    - It is important to set up the qt environment.
- qmake.exe -r
    - Here you must definitely call the qmake that was loaded from the 1st item.
    - Example: C:/Qt/5.15.0/mingw81_64/bin/qmake.exe -r
- migw32-make -j8
- migw32-make deploy
    - this command requires installed [cqtdeployer](https://github.com/QuasarApp/CQtDeployer/releases)
- ./Distro/CQtDeployerInstaller.exe

