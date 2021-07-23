# CQtDeployer 

[![cqtdeployer](https://snapcraft.io//cqtdeployer/badge.svg)](https://snapcraft.io/cqtdeployer)
[![cqtdeployer](https://snapcraft.io//cqtdeployer/trending.svg?name=0)](https://snapcraft.io/cqtdeployer)

# ![Logo](res/CQtDeployer_banner_web.png)

****************************
## Support the project
If you like what we do and it benefits you, you can support the project on the official page [QuasarApp in Patreon](https://www.patreon.com/QuasarApp)

## What is CQtDeployer
The CQtDeployer is application for extract all depends library of executable and create launch script for your application.


Key differences of this program:
* Performance: this program deploys the application several times faster (up to 10 seconds)
* Flexibility: this application's got flags that help you to configure the deployment for your or your project's needs
* Crossdeploy: this application's support windows and linux distrebutives, This means that you can use it not only to deploy a project for your platform, but also to deploy a project on Linux for Windows and vice versa. 
* Fast create installers : Upon completion of the deployment, you will receive a self-contained installer of your distribution.

## Supported platforms:

* <img src="https://user-images.githubusercontent.com/12465465/67766480-929b5000-fa5f-11e9-8ae7-377d9ddb7c65.png"  height="100">
* <img src="https://upload.wikimedia.org/wikipedia/commons/8/8d/Windows_darkblue_2012.svg"  height="100">

## Support processors architectures:
* x86
* x86-64
* ARM
* ARM64

## How to use

#### Example: cqtdeployer <-bin    [params]> [options]

See list of all options [here](md/en/Options.md)


#### Example: cqtdeployer -bin myApp -qmlDir ~/MyAppProject/qml -qmake ~/Qt/5.12.4/gcc_64/bin/qmake clear

## More examples
#### Project structure after deploy:
``` bash
 #linux
 cqtdeployer -bin Guitar  
 
 #windows
 cqtdeployer -bin Guitar.exe -qmake /path/to/qmake.exe

```
<img src="https://user-images.githubusercontent.com/12465465/81007195-7ab2e780-8e59-11ea-9793-1eeeb0025b96.png"  height="400">

#### Project installer after deploy with the qif option.
``` bash
 #linux
 cqtdeployer -bin Application qif
 
 #windows
 cqtdeployer -bin Application.exe -qmake /path/to/qmake.exe qif

```
<img src="https://user-images.githubusercontent.com/12465465/78639719-197a1180-78b7-11ea-9e82-a03dd43f4bd2.png"  height="400">

#### Project installer after deploy with the qif and qifStyle option.

``` bash
 #linux
 cqtdeployer -bin Application qif -qifStyle quasar
 
 #windows
 cqtdeployer -bin Application.exe -qmake /path/to/qmake.exe qif -qifStyle quasar

```

<img src="https://user-images.githubusercontent.com/12465465/78639720-1aab3e80-78b7-11ea-9604-3dcdd7c78482.png"  height="400">

``` bash
 #linux
 cqtdeployer -bin Application qif -qifStyle path/to/myStyle.css
 
 #windows
 cqtdeployer -bin Application.exe -qmake /path/to/qmake.exe qif -qifStyle path/to/myStyle.css

```

<img src="https://user-images.githubusercontent.com/12465465/78639716-17b04e00-78b7-11ea-9002-a8b45a43807f.png"  height="400">

## Build and install
The Building or installation instructions see on wiki [page](md/en/Build-and-Install.md)

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

### If you want to get more information see our documentations [here](md/index.md)

***

<p align="center"><img src="res/Credits.jpg" alt="Credits"></p>

Join the development fund and support QuasarApp to help with the development of our projects.

<a href="https://www.patreon.com/bePatron?u=9304737"><img src="https://img.shields.io/badge/ become a patron-red?&style=for-the-badge&logo=patreon&logoColor=white" height=30 width=180></a>



If you would like to use bitcoins to support us, you can transfer coins to these address. If you want to be included in the patron list, send an e-mail message - QuasarApp@yandex.by.

**Note**: You must send a message signed with address of the bitcoin wallet from which the payment was made.

<p align="center"><img src="res/qr.png" alt="qrcode"></p>

<p align="center">bitcoin: bc1qzuymq7jc9q2x04wcwstr45tu8klm46zfl20wyp</p>

## Our patrons list:

### Silver Membership Patrons:

- Rustem Husnutdinov
- Semih Ufuk Güler

***


## Thanks
 * many thanks [Trail of Bits](https://github.com/trailofbits) for their library [pe-parser](https://github.com/trailofbits/pe-parse). Everything works perfectly.

***************************
