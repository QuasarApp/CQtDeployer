# CQtDeployer for arm machines


At this time, cqtdeployer does not have precompiled distributions for arm platform. But it supports deploy arm applications, so the best solution for install cqtdeployer tool to arm host is building from source.

## Build

You can read a main article of building cqtdeployer from source [here](Build-and-Install.md).

## Qt installer framework.

The QIFW not available for arm platforms. So if you want to create classic installers you need to build Qt installer framework from [source](https://github.com/qtproject/installer-framework).

## Build from source

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
    cmake .. -DBUILD_SHARED_LIBS=0 -DCQT_DEPLOYER_TESTS=0
    ```
    
- build cqtdeployer 

    ``` bash
    make -j8
    ```
    
- create cqtdeployer executable.

    ``` bash
    make install 
    ```


Done. Now you can add cqtdeployer into your PATH variable and use it or create symbol link to cqtdeployer executable in /bin folder.


``` bash
ln -s ./bin/CQtDeployer /bin/cqtdeployer
```


