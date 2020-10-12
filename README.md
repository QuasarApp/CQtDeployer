# CQtDeployer [![cqtdeployer](https://snapcraft.io//cqtdeployer/badge.svg)](https://snapcraft.io/cqtdeployer)
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

See list of all options [here](https://github.com/QuasarApp/CQtDeployer/wiki/Options)


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
The Building or installation instructions see on wiki [page](https://github.com/QuasarApp/CQtDeployer/wiki/Build-and-Install)

## Projects using CQtDeployer:

- [QAmigo](https://github.com/QAmigo/QAmigo) - Qt cross-platform serial port socket assistant for visualizing data. 
- [QtSecret](https://github.com/QuasarApp/Qt-Secret) - Simple encryption library supporting RSA and AES algorithms. 
- [Hanoi-Towers](https://github.com/QuasarApp/Hanoi-Towers) -  Hanoi Towers Game 
- [distbuilder](https://github.com/BuvinJT/distbuilder) -  Distribution Builder 
- [Manager_v2](https://github.com/dimkatsi91/Manager_v2) - Qt Quick Controls 2 - Linux Manager Desktop Application 
- [IDE65XX](https://github.com/emartisoft/IDE65XX) - IDE 65XX is one of the open source IDE options available for developers working with Kick Assembler. 
- [Tanks](https://github.com/anevero/tanks) - 2D game written in C++ & Qt for Windows, Linux and Android 
- [stegano](https://github.com/taskmaster9001/stegano) - Bit-level Image Manipulation Software with Qt Framework - Steganography Pet Project 

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
* Быстрое создание установщиков. По завершении развертывания вы получите автономный установщик вашего дистрибутива.

## Поддержите проект
Если вам нравится то, что мы делаем, и это приносит вам пользу, вы можете поддержать проект на официальной странице [QuasarApp в Patreon](https://www.patreon.com/QuasarApp)

## Поддерживаемые платформы:
* <img src="https://user-images.githubusercontent.com/12465465/67766480-929b5000-fa5f-11e9-8ae7-377d9ddb7c65.png"  height="100">
* <img src="https://upload.wikimedia.org/wikipedia/commons/8/8d/Windows_darkblue_2012.svg"  height="100">

## Поддержка архитектур процессоров:
* x86
* x86-64
* ARM
* ARM64

## Как использовать
#### пример : cqtdeployer <-bin    [params]> [options]
Смотрите список всех опций [здесь](https://github.com/QuasarApp/CQtDeployer/wiki/%D0%9F%D0%B0%D1%80%D0%B0%D0%BC%D0%B5%D1%82%D1%80%D1%8B
)

#### пример: cqtdeployer -bin myApp -qmlDir ~/MyAppProject/qml -qmake ~/Qt/5.12.4/gcc_64/bin/qmake clear

## Еще примеры
#### Стуктура проекта после развертывания:
``` bash
 #linux
 cqtdeployer -bin Guitar  
 
 #windows
 cqtdeployer -bin Guitar.exe -qmake /path/to/qmake.exe

```
<img src="https://user-images.githubusercontent.com/12465465/81007195-7ab2e780-8e59-11ea-9793-1eeeb0025b96.png"  height="400">

#### Установщик проекта после использования опции qif.
``` bash
 #linux
 cqtdeployer -bin Application qif
 
 #windows
 cqtdeployer -bin Application.exe -qmake /path/to/qmake.exe qif

```
<img src="https://user-images.githubusercontent.com/12465465/78639719-197a1180-78b7-11ea-9e82-a03dd43f4bd2.png"  height="400">

#### Установщик проекта после использования опции qif c пользовательским стилем.
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

## Сборка и установка
Инструкции по сборке или установке см. В [вики](https://github.com/QuasarApp/CQtDeployer/wiki/%D0%A1%D0%B1%D0%BE%D1%80%D0%BA%D0%B0-%D0%B8-%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0)


## Проекты использующие CQtDeployer :

- [QAmigo](https://github.com/QAmigo/QAmigo)
- [QtSecret](https://github.com/QuasarApp/Qt-Secret)
- [Hanoi-Towers](https://github.com/QuasarApp/Hanoi-Towers)
- [distbuilder](https://github.com/BuvinJT/distbuilder)
- [Manager_v2](https://github.com/dimkatsi91/Manager_v2)
- [IDE65XX](https://github.com/emartisoft/IDE65XX)
- [Tanks](https://github.com/anevero/tanks)
- [stegano](https://github.com/taskmaster9001/stegano)

## Благодарность
 * Большое спасибо  [Trail of Bits](https://github.com/trailofbits) за их библиотеку [pe-parser](https://github.com/trailofbits/pe-parse). Все работает отлично.
