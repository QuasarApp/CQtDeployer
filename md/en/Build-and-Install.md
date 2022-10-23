# Install CQtDeployer

You can download the latest version of the application [here](https://github.com/QuasarApp/CQtDeployer/releases).

### Installer

- [Download](https://github.com/QuasarApp/CQtDeployer/releases) CQtDeployer*.run or CQtDeployer*.exe

#### Install without gui

##### Linux

``` bash
chmod +x CQtDeployer_1.5.3.0_Installer_Linux64.run
./CQtDeployer_1.5.3.0_Installer_Linux64.run install
```

##### Windows

``` bash
CQtDeployer_1.5.3.0_Installer_Win64.exe install
```

### Deb

``` bash
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
    
- create installers and packages (requered installed cqtdeployer)

    ``` bash
    make deploy 
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
- SET PATH=C:/Qt/Tools/mingw810_64/bin;%PATH%
    - It is important to set up the qt environment.
   
- run cmake 

    ``` bash
    cmake .. -DCMAKE_PREFIX_PATH=C:/path/to/qt/root/dir
    ```
    
- build cqtdeployer 

    ``` bash
    migw32-make -j8
    ```
    
- create installers and packages (requered installed cqtdeployer)

    ``` bash
    migw32-make deploy 
    ```


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
