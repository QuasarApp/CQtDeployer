# CQtDeployer

[![cqtdeployer](https://snapcraft.io//cqtdeployer/badge.svg)](https://snapcraft.io/cqtdeployer)
[![cqtdeployer](https://snapcraft.io//cqtdeployer/trending.svg?name=0)](https://snapcraft.io/cqtdeployer)

# ![Logo](res/CQtDeployer_banner_web.png)

****************************
## Support the project
![image](https://user-images.githubusercontent.com/12465465/194008498-7f069b4a-b87d-42d1-82f1-af7d8cbcf220.png)

We have a cqtdeployer extended support [subscription](https://www.patreon.com/QuasarApp)
on the Patrion service. 
* We'll help you solve your deployment issues personality. 
* All your tasks needed to solve your issues will have top priority.

Also, if you just want to support the project, you can subscribe to the general subscription plan on patrion

If you can't use Patreon, you can send **bitcoin** to support us and our work.
If you want to get support for bitcoin payment, write to me in [telegramm](https://t.me/Endrll) 

## bitcoin address
![image](https://user-images.githubusercontent.com/12465465/194010492-bae7b9e8-10a6-4e5f-9606-a5f3b4478a3e.png)

**bc1q3jnx7pleh7rxg6nm6navvuwwpd9znxvd4lye62**


## What is CQtDeployer

CQtDeployer helps you to extract all libraries your executable depends on and to create a launch script for your application.

Key differences of this program:
* Performance: this program deploys the application several times faster (up to 10 seconds)
* Flexibility: this application has flags that help you to configure the deployment for your or your project's needs
* Cross-deployment: this application supports Windows and Linux distributions. This means that you can deploy a project not only for the host platform, but also deploy a project for Windows target on Linux host and vice versa.
* Fast installers creation: Once the deployment is complete, you will receive a standalone installer for your distribution.

## Supported platforms:

* <img src="https://user-images.githubusercontent.com/12465465/67766480-929b5000-fa5f-11e9-8ae7-377d9ddb7c65.png"  height="100">
* <img src="https://upload.wikimedia.org/wikipedia/commons/8/8d/Windows_darkblue_2012.svg"  height="100">

## Supported processors architectures:
* x86
* x86-64
* ARM
* ARM64

## How to use

#### Example: `cqtdeployer <-bin    [params]> [options]`

See the list of all options [here](md/en/Options.md)


#### Example: `cqtdeployer -bin myApp -qmlDir ~/MyAppProject/qml -qmake ~/Qt/6.3.1/gcc_64/bin/qmake clear`

## More examples
#### Deployed project structure:
``` bash
 #linux
 cqtdeployer -bin Guitar  
 
 #windows
 cqtdeployer -bin Guitar.exe -qmake /path/to/qmake.exe

```
<p align="center"><img src="https://user-images.githubusercontent.com/12465465/81007195-7ab2e780-8e59-11ea-9793-1eeeb0025b96.png"  height="400"></p>

#### Project installer after deployment with the `qif` option.
``` bash
 #linux
 cqtdeployer -bin Application qif
 
 #windows
 cqtdeployer -bin Application.exe -qmake /path/to/qmake.exe qif

```
<p align="center"><img src="https://user-images.githubusercontent.com/12465465/78639719-197a1180-78b7-11ea-9e82-a03dd43f4bd2.png"  height="400"></p>

#### Project installer after deployment with the `qif` and `qifStyle` options.

``` bash
 #linux
 cqtdeployer -bin Application qif -qifStyle quasar
 
 #windows
 cqtdeployer -bin Application.exe -qmake /path/to/qmake.exe qif -qifStyle quasar

```

<p align="center"><img src="https://user-images.githubusercontent.com/12465465/78639720-1aab3e80-78b7-11ea-9604-3dcdd7c78482.png"  height="400"></p>

``` bash
 #linux
 cqtdeployer -bin Application qif -qifStyle path/to/myStyle.css
 
 #windows
 cqtdeployer -bin Application.exe -qmake /path/to/qmake.exe qif -qifStyle path/to/myStyle.css

```

<p align="center"><img src="https://user-images.githubusercontent.com/12465465/78639716-17b04e00-78b7-11ea-9002-a8b45a43807f.png"  height="400"></p>

## Build and install
See building and installation instructions on wiki [page](md/en/Build-and-Install.md)

## Projects using CQtDeployer:

- [QAmigo](https://github.com/QAmigo/QAmigo) - Qt cross-platform serial port socket assistant for visualizing data.
- [QtSecret](https://github.com/QuasarApp/Qt-Secret) - Simple encryption library supporting RSA and AES algorithms.
- [Hanoi-Towers](https://github.com/QuasarApp/Hanoi-Towers) -  Hanoi Towers Game
- [distbuilder](https://github.com/BuvinJT/distbuilder) -  Distribution Builder
- [Manager_v2](https://github.com/dimkatsi91/Manager_v2) - Qt Quick Controls 2 - Linux Manager Desktop Application
- [IDE65XX](https://github.com/emartisoft/IDE65XX) - IDE 65XX is one of the open source IDE options available for developers working with Kick Assembler.
- [Tanks](https://github.com/anevero/tanks) - 2D game written in C++ & Qt for Windows, Linux and Android
- [stegano](https://github.com/taskmaster9001/stegano) - Bit-level Image Manipulation Software with Qt Framework - Steganography Pet Project
- [Online_Board](https://github.com/MaxMalts/Online_Board) - This is a board that you can share with others.


***

### Check out our documentation for more information: [here](md/index.md)

***

## Thanks
 * many thanks [Trail of Bits](https://github.com/trailofbits) for their library [pe-parser](https://github.com/trailofbits/pe-parse). Everything works perfectly.

***************************

