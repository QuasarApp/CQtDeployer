# CQtDeployer [![CQtDeployer](https://snapcraft.io/cqtdeployer/badge.svg)](https://snapcraft.io/cqtdeployer)
# ![Logo](https://i.ibb.co/phDBvfr/qt.png)

****************************
## What is CQtDeployer
The CQtDeployer is application for extract all depends library of executable and create launch script for your application.


Key differences of this program:
* Performance: this program deploys the application several times faster (up to 10 seconds)
* Flexibility: this application's got flags that help you to configure the deployment for your or your project's needs
* Crossdeploy: this application's support windows and linux distrebutives, This means that you can use it not only to deploy a project for your platform, but also to deploy a project on Linux for Windows and vice versa. 



### How to use

#### Example: cqtdeployer <-bin    [params]> [options]

#### cqtdeployer -bin myApp -qmlDir ~/MyAppProject/qml -qmake ~/Qt/5.12.4/gcc_64/bin/qmake clear

#### Options:
| Option                      | Descriptiion                                                    |
|-----------------------------|-----------------------------------------------------------------|
|   help / h                  | Shows help.                                                     |
|   always-overwrite          | Copies files and replaces the existing ones.                    |
|   -bin    [list, params]    | Deployable file or folder. For example -bin ~/my/project/bin/,~/my/project/bin.exe|
|   -binDir [params]          | A folder which includes deployable files (recursive search). WARNING: this flag supports  'so', 'dll' and 'exe' files only. Use '-bin' flag if you want to deploy linux binary files |
|   -qmlDir [params]          | Qml data dir. For example -qmlDir ~/my/project/qml              |
|   deploySystem              | Deploys all libraries                                           |
|   noLibc                    | Skip Deploys libc and ld-linux libs                             |
|   -qmake  [params]          | Qmake path. For example                                         |
|                             | -qmake ~/Qt/5.11.1/gcc_64/bin/qmake                             |
|   -ignore [list,params]     | The list of libs to ignore.                                     |
|                             | For example -ignore libicudata.so.56,libicudata2.so.56          |
|   -ignoreEnv [list,params]  | The list of the environment to ignore.                          |
|                             | For example -ignoreEnv /bad/dir,/my/bad/Dir                     |
|   clear                     | Deletes deployable files of the previous session.               |
|                             | For example -runScript myApp.sh                                 |
|  allQmlDependes             | Extracts all the qml libraries.                                 |
|                             | (not recommended, as it takes great amount of computer memory)  |
|  -libDir [list,params]      | Sets additional paths for extra libs of an app.                 |
|                             | For example -libDir ~/myLib,~/newLibs                           |
|  -extraPlugin [list,params] | Sets an additional path to extraPlugin of an app                |
|  -recursiveDepth [params]   | Sets the Depth of recursive search of libs (default 0)      |
|  -targetDir [params]        | Sets target directory(by default it is the path to the first deployable file)|
|  noStrip                    | Skips strip step                                                |
|  noTranslations             | Skips the translations files.                                   |
|  qmlExtern                  | Use qml external scanner (qmlimportscaner)                      |
|                             | It doesn't work without qmake and inside a snap package         |
|  -verbose [0-3]             | Shows debug log                                                 |



#### Example: cqtdeployer -bin myApp -qmlDir ~/MyAppProject/qml -qmake ~/Qt/5.12.4/gcc_64/bin/qmake clear


## Build for Linux 
  - install qt and qt QtInstallFrameWork from [qtInstaller](https://www.qt.io/download-qt-installer?hsCtaTracking=9f6a2170-a938-42df-a8e2-a9f0b1d6cdce%7C6cb0de4f-9bb5-4778-ab02-bfb62735f3e5)
  - qmake -r
  - make -j$(nproc)
  - make deploy
  - ./Distro/CQtDeployerInstaller

## Build for Windows
  - install qt and qt QtInstallFrameWork from [qtInstaller](https://www.qt.io/download-qt-installer?hsCtaTracking=9f6a2170-a938-42df-a8e2-a9f0b1d6cdce%7C6cb0de4f-9bb5-4778-ab02-bfb62735f3e5)
  - qmake -r
  - make -j$(nproc)
  - make deploy
  - ./Distro/CQtDeployerInstaller.exe
  
## Install 
You can download the latest version of the application [here](https://github.com/QuasarApp/Console-QtDeployer/releases).

### Snap 
[![Get it from the Snap Store](https://snapcraft.io/static/images/badges/en/snap-store-black.svg)](https://snapcraft.io/cqtdeployer)

## Donate
If you want to support the project,you can donate into our bitcoin wallet.

### Bitcoin address - 1NJNbDKmezcUcHRfzpBeq2fHeG21oEKX8Q

***************************
***************************
***************************

Qt CQtDeployer - это приложением для извлечения всех зависимых библиотек исполняемого файла и создает сценарий запуска для вашего приложения.

Основные отличия этой программы:
* Производительность - эта программа развертывает приложение в несколько раз быстрее (до 10 секунд)
* Гибкость - это приложение имеет флаги, которые помогут вам настроить деплоинг под ваши нужды и нужды вашего проекта
* Crossdeploy: приложение поддерживает Windows и дистрибутивы Linux, это означает, что вы можете использовать его не только для развертывания проекта для вашей платформы, но и для развертывания проекта в Linux для Windows и наоборот.

### Как использовать
#### пример : cqtdeployer <-bin    [params]> [options]

#### Options:
| Option                      | Descriptiion                                              |
|-----------------------------|-----------------------------------------------------------|
|   help / h                  | Показывает справку                                          |
|   always-overwrite          | Копирует файлы с заменой уже существующих                 |
|   -bin    [list, params]    | Развертываемый файл или папка. пример -bin ~/my/project/bin/,~/my/project/bin.exe|
|   -binDir [params]          | Папка с развертываемыми файлами (с рекурсивным поиском). ВНИМАНИЕ! Этот флаг поддерживает только файлы 'so', 'dll' и 'exe'. Если вы хотите развернуть бинарный файл Linux, используйте флаг '-bin'  |
|   -qmlDir [params]          | Папка qml. пример -qmlDir ~/my/project/qml                |
|   deploySystem              | Копирует все библиотеки кроме libc                        |
|   deployLibc                | Копирует libc и все остальные библиотеки                  |
|   -qmake  [params]          | Путь к qmake. пример                                      |
|                             | -qmake ~/Qt/5.12.4/gcc_64/bin/qmake                       |
|   -ignore [list,params]     | Список библиотек для игнорирования                        |
|                             | Пример -ignore libicudata.so.56,libicudata2.so.56         |
|   -ignoreEnv [list,params]  | Список путей для игнорирования.                          |
|                             | Пример -ignoreEnv /bad/dir,/my/bad/Dir                                |
|   clear                     | Удаляет все старые файлы (с прошлого запуска)              |
|                             | пример -runScript myApp.sh                                |
|  allQmlDependes             | Извлекает все библиотеки qml.          |
|                             | (не рекомендуется, так как занимает много памяти)         |
|  -libDir [list,params]      | Устанавливает дополнительные пути к библиотекам               |
|                             | Пример -libDir ~/myLib,~/newLibs                          |
|  -extraPlugin [list,params] | Устанавливает дополнительный путь для extraPlugin приложения |
|  -recursiveDepth [params]   | Устанавливает глубину поиска библиотек (по умолчанию 0)       |
|  -targetDir [params]        | Устанавливает целевой каталог (по умолчанию это путь к первому развертываемому файлу)|
|  noStrip                    | Пропускает шаг strip                                      |
|  noTranslations             | Пропускает файлы переводов                                |
|  qmlExtern                  | Использует внешний сканер qml (qmlimportscaner)         |
|                             | не работает без qmake и в snap                            |
|  -verbose [0-3]             | Показывает дебаг лога                                          |


#### Пример: cqtdeployer -bin myApp -qmlDir ~/MyAppProject/qml -qmake ~/Qt/5.12.4/gcc_64/bin/qmake clear

## Build для Linux 
  - установите qt и qt QtInstallFrameWork из [Установщик qt](https://www.qt.io/download-qt-installer?hsCtaTracking=9f6a2170-a938-42df-a8e2-a9f0b1d6cdce%7C6cb0de4f-9bb5-4778-ab02-bfb62735f3e5)
  - qmake -r
  - make -j$(nproc)
  - make deploy
  - ./Distro/CQtDeployerInstaller
  
## Build для Windows
  - установите qt и qt QtInstallFrameWork из [Установщик qt](https://www.qt.io/download-qt-installer?hsCtaTracking=9f6a2170-a938-42df-a8e2-a9f0b1d6cdce%7C6cb0de4f-9bb5-4778-ab02-bfb62735f3e5)
  - qmake -r
  - make -j$(nproc)
  - make deploy
  - ./Distro/CQtDeployerInstaller.exe
  
### Snap
[![Загрузите из Snap Store](https://snapcraft.io/static/images/badges/ru/snap-store-black.svg)](https://snapcraft.io/cqtdeployer)


## Установить
Вы можете загрузить последнюю версию приложения [здесь](https://github.com/QuasarApp/Console-QtDeployer/releases).


## Пожертвовать
Если вы хотите помочь проекту, то вы можете пожертвовать небольшую сумму на наш биткойн-кошелек.

### Биткойн-адрес - 1NJNbDKmezcUcHRfzpBeq2fHeG21oEKX8Q
