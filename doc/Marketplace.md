## What is CQtDeployer
The CQtDeployer is application for extract all depends library of executable and create launch script for your application.

Key differences of this program:
* Performance: this program deploys the application several times faster (up to 10 seconds)
* Flexibility: this application's got flags that help you to configure the deployment for your or your project's needs
* Crossdeploy: this application's support windows and linux distrebutives, This means that you can use it not only to deploy a project for your platform, but also to deploy a project on Linux for Windows and vice versa.

## How to use
Deploying a Qt or QML application is easy!
To do this, simply install cqtdeployer on your computer and enter the following line:

#### Linux:
``` bash
cqtdeployer -bin myApp -qmake /media/D/Qt/5.12.3/gcc_64/bin/qmake -qmlDir ./
```

#### Windows:
``` bash
%cqtdeployer% -bin myApp.exe -qmake /media/D/Qt/5.12.3/gcc_64/bin/qmake.exe -qmlDir ./
```


#### Discription
* -bin -  this is option for set the path to executable files.
* -qmake - this is option for set the path to qmake with which the application was created.
* -qmlDir - this is option for set the path to folder with qml files of application.

## Install
You can download the latest version of the CQtDeployer [here](https://github.com/QuasarApp/CQtDeployer/releases) for Windows and linux platforms.
Or use the snap version for servers without gui.

[![Get it from the Snap Store](https://snapcraft.io/static/images/badges/en/snap-store-black.svg)](https://snapcraft.io/cqtdeployer)
