# CQtDeployer [![cqtdeployer](https://snapcraft.io//cqtdeployer/badge.svg)](https://snapcraft.io/cqtdeployer)
[![cqtdeployer](https://snapcraft.io//cqtdeployer/trending.svg?name=0)](https://snapcraft.io/cqtdeployer)
# ![Logo](https://i.ibb.co/phDBvfr/qt.png)

****************************

## What is CQtDeployer
The CQtDeployer is application for extract all depends library of executable and create launch script for your application.


Key differences of this program:
* Performance: this program deploys the application several times faster (up to 10 seconds)
* Flexibility: this application's got flags that help you to configure the deployment for your or your project's needs
* Crossdeploy: this application's support windows and linux distrebutives, This means that you can use it not only to deploy a project for your platform, but also to deploy a project on Linux for Windows and vice versa. 

## Supported platforms:
* <img src="https://user-images.githubusercontent.com/12465465/67766480-929b5000-fa5f-11e9-8ae7-377d9ddb7c65.png"  height="100">
* <img src="https://upload.wikimedia.org/wikipedia/commons/8/8d/Windows_darkblue_2012.svg"  height="100">

## Support processors architectures:
* x86
* x86-64
* ARM
* ARM64

### How to use

#### Example: cqtdeployer <-bin    [params]> [options]

See list of all options [here](./doc/Options.md)


#### Example: cqtdeployer -bin myApp -qmlDir ~/MyAppProject/qml -qmake ~/Qt/5.12.4/gcc_64/bin/qmake clear


## Build for Linux
- install qt and qt QtInstallFrameWork from [qt installer](https://www.qt.io/download-qt-installer?hsCtaTracking=9f6a2170-a938-42df-a8e2-a9f0b1d6cdce%7C6cb0de4f9bb77-7bb77-4bb77-4)
- git clone https://github.com/QuasarApp/CQtDeployer.git
- cd CQtDeployer
- git submodule update --init --recursive
- qmake -r
     - Here you must definitely call the qmake that was loaded from the 1st item.
     - Example: ~/Qt/5.14.0/gcc_64/bin/qmake -r
- make -j$(nproc)
- make deploy
     - #this command requires installed [cqtdeployer](https://github.com/QuasarApp/CQtDeployer/releases)
- ./Distro/CQtDeployerInstaller.run

## Build for Windows (CMD)
- install qt and qt QtInstallFrameWork from [qt installer](https://www.qt.io/download-qt-installer?hsCtaTracking=9f6a2170-a938-42df-a8e2-a9f0b1d6cdce%7C6cb0de4f9bb77-7bb77-4bb77-4)
- git clone https://github.com/QuasarApp/CQtDeployer.git
- cd CQtDeployer
- git submodule update --init --recursive
- SET PATH=C:/Qt/Tools/mingw730_64/bin;%PATH%
     - It is important to set up the qt environment.
- qmake.exe -r
     - Here you must definitely call the qmake that was loaded from the 1st item.
     - Example: C:/Qt/5.14.0/mingw73_64/bin/qmake.exe -r
- migw32-make.exe -j$(nproc)
- migw32-make.exe deploy
     - #this command requires installed [cqtdeployer](https://github.com/QuasarApp/CQtDeployer/releases)
- ./Distro/CQtDeployerInstaller.exe

## Install 
You can download the latest version of the application [here](https://github.com/QuasarApp/CQtDeployer/releases).

### Snap 
[![Get it from the Snap Store](https://snapcraft.io/static/images/badges/en/snap-store-black.svg)](https://snapcraft.io/cqtdeployer)

## Projects using CQtDeployer:

- [QAmigo](https://github.com/QAmigo/QAmigo)
- [QtSecret](https://github.com/QuasarApp/Qt-Secret)
- [Hanoi-Towers](https://github.com/QuasarApp/Hanoi-Towers)
- [distbuilder](https://github.com/BuvinJT/distbuilder)

## Donate
If you want to support the project,you can donate into our bitcoin wallet.

### Bitcoin address - 1NJNbDKmezcUcHRfzpBeq2fHeG21oEKX8Q

## Thanks
 * many thanks [Trail of Bits](https://github.com/trailofbits) for their library [pe-parser](https://github.com/trailofbits/pe-parse). Everything works perfectly.

***************************
***************************
***************************

**CQtDeployer** - это приложение преднознаена для извлечения всех зависимых библиотек исполняемого файла и создания сценария запуска для вашего приложения.

Основные отличия этой программы:
* Производительность - эта программа развертывает приложение в несколько раз быстрее (до 10 секунд)
* Гибкость - это приложение имеет флаги, которые помогут вам настроить деплоинг под ваши нужды и нужды вашего проекта
* Crossdeploy: приложение поддерживает Windows и дистрибутивы Linux, это означает, что вы можете использовать его не только для развертывания проекта для вашей платформы, но и для развертывания проекта в Linux для Windows и наоборот.

## Поддерживаемые платформы:
* <img src="https://user-images.githubusercontent.com/12465465/67766480-929b5000-fa5f-11e9-8ae7-377d9ddb7c65.png"  height="100">
* <img src="https://upload.wikimedia.org/wikipedia/commons/8/8d/Windows_darkblue_2012.svg"  height="100">

## Поддержка архитектур процессоров:
* x86
* x86-64
* ARM
* ARM64

### Как использовать
#### пример : cqtdeployer <-bin    [params]> [options]

Смотрите список всех опций [здесь](./doc/Options.md)


## Build для Linux
- установите qt и qt QtInstallFrameWork из [Установщик qt](https://www.qt.io/download-qt-installer?hsCtaTracking=9f6a2170-a938-42df-a8e2-a9f0b1d6cdce%7C6cb0de4f-9bb5-4778-ab02-bfb62735f3e5)
- git clone https://github.com/QuasarApp/CQtDeployer.git
- cd CQtDeployer
- git submodule update --init --recursive
- qmake -r
     - Здесь нужно обязательно вызвать тот qmake, который был загружен из 1го пункта.
     - Пример: ~/Qt/5.14.0/gcc_64/bin/qmake -r
- make -j$(nproc)
- make deploy
     - эта команда требует установленный [cqtdeployer](https://github.com/QuasarApp/CQtDeployer/releases)
- ./Distro/CQtDeployerInstaller.run

## Build для Windows (CMD)
- установите qt и qt QtInstallFrameWork из [Установщик qt](https://www.qt.io/download-qt-installer?hsCtaTracking=9f6a2170-a938-42df-a8e2-a9f0b1d6cdce%7C6cb0de4f-9bb5-4778-ab02-bfb62735f3e5)
- git clone https://github.com/QuasarApp/CQtDeployer.git
- cd CQtDeployer
- git submodule update --init --recursive
- SET PATH=C:/Qt/Tools/mingw730_64/bin;%PATH%
    - Здесь важно настроить окружение qt.
- qmake.exe -r
    - Здесь нужно обязательно вызвать тот qmake, который был загружен из 1го пункта.
    - Пример: C:/Qt/5.14.0/mingw73_64/bin/qmake.exe -r
- migw32-make.exe -j$(nproc)
- migw32-make.exe deploy
    - эта команда требует установленный [cqtdeployer](https://github.com/QuasarApp/CQtDeployer/releases)
- ./Distro/CQtDeployerInstaller.exe

### Snap
[![Загрузите из Snap Store](https://snapcraft.io/static/images/badges/ru/snap-store-black.svg)](https://snapcraft.io/cqtdeployer)


## Установить
Вы можете загрузить последнюю версию приложения [здесь](https://github.com/QuasarApp/CQtDeployer/releases).

## Проекты использующие CQtDeployer :

- [QAmigo](https://github.com/QAmigo/QAmigo)
- [QtSecret](https://github.com/QuasarApp/Qt-Secret)
- [Hanoi-Towers](https://github.com/QuasarApp/Hanoi-Towers)
- [distbuilder](https://github.com/BuvinJT/distbuilder)

## Пожертвовать
Если вы хотите помочь проекту, то вы можете пожертвовать небольшую сумму на наш биткойн-кошелек.

### Биткойн-адрес - 1NJNbDKmezcUcHRfzpBeq2fHeG21oEKX8Q

## Благодарность
 * Большое спасибо  [Trail of Bits](https://github.com/trailofbits) за их библиотеку [pe-parser](https://github.com/trailofbits/pe-parse). Все работает отлично.
