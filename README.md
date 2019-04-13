# Welcome to the Console Qt Deployer!
# ![Logo](/res/icon.png)

***************************
## What is Console Qt Deployer
The Console QtDeployer is console veriosn of the [QtDeployer](https://quasarapp.github.io/QtDeployer/).

Key differences of this program:
* Performance: this program deploys the application several times faster (up to 10 seconds)
* Flexibility: this application's got flags that help you to configure the deployment for your or your project's needs



### How to use

#### Example: cqtdeployer <-bin    [params]> [options]

#### Options:
| Option                      | Descriptiion                                                    |
|-----------------------------|-----------------------------------------------------------------|
|   help / h                  | Shows help.                                                     |
|   always-overwrite          | Copies files and replaces the existing ones.                      |
|   -bin    [list, params]    | Deployable file or folder. For example -bin ~/my/project/bin/,~/my/project/bin.exe|
|   -binDir [params]          | A folder which includes deployable files (recursive search). WARNING: this flag supports  'so', 'dll' and 'exe' files only. Use '-bin' flag if you want to deploy linux binary files |
|   -qmlDir [params]          | Qml data dir. For example -qmlDir ~/my/project/qml               |
|   deploySystem              | Deploys all the libs                                                 |
|   -qmake  [params]          | Qmake path. For example                                         |
|                             | -qmake ~/Qt/5.11.1/gcc_64/bin/qmake                             |
|   -ignore [list,params]     | The list of the libs to ignore.                                          |
|                             | For example -ignore libicudata.so.56,libicudata2.so.56          |
|   clear                     | Deletes deployable files of the previous session.                                      |
|                             | For example -runScript myApp.sh                                 |
|  allQmlDependes             | Extracts all the qml libraries.              |
|                             | (not recommended, as it takes great amount of computer memory)               |
|  -libDir [list,params]      | Sets additional paths for extra libs of an app.                         |
|                             | For example -libDir ~/myLib,~/newLibs                           |
|  -extraPlugin [list,params] | Sets an additional path to extraPlugin of an app                      |
|  -recursiveDepth [params]   | Sets the Depth of recursive search of the libs (default 0)              |
|  -targetDir [params]        | Sets target directory(by default it is the path to the first deployable file)   |
|  noStrip                    | Skips strip step                                                 |
|  noTranslations             | Skips the translations files.                                         |
|  qmlExtern                  | Use qml external scanner (qmlimportscaner)                      |
|                             | It doesn't work without qmake and inside a snap package                      |
|  -verbose [0-3]             | Shows debug log                                                  |



#### Example: cqtdeployer -bin myApp -qmlDir ~/MyAppProject/qml -qmake ~/Qt/5.11.1/gcc_64/bin/qmake clear


## Build for Linux 
  -  ./staticBuild
  -  ./sharedBuild
  -  ./snapBuild

## Build for Windows
 - from qtCreator

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

Console QtDeployer является консольной реализацией [QtDeployer](https://quasarapp.github.io/QtDeployer/).

Основные отличия этой программы:
* Производительность - эта программа развертывает приложение в несколько раз быстрее (до 10 секунд)
* Гибкость - это приложение имеет флаги, которые помогут вам настроить деплоинг под ваши нужды и нужды вашего проекта


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
|   deploySystem              | Копирует все библиотеки                                 |
|   -qmake  [params]          | Путь к qmake. пример                                      |
|                             | -qmake ~/Qt/5.11.1/gcc_64/bin/qmake                       |
|   -ignore [list,params]     | Список библиотек для игнорирования                        |
|                             | Пример -ignore libicudata.so.56,libicudata2.so.56         |
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


#### Пример: cqtdeployer -bin myApp -qmlDir ~/MyAppProject/qml -qmake ~/Qt/5.11.1/gcc_64/bin/qmake clear

## Build для Linux 
  -  ./staticBuild
  -  ./sharedBuild
  -  ./snapBuild
  
## Build для Windows
 - Из qtCreator

### Snap
[![Загрузите из Snap Store](https://snapcraft.io/static/images/badges/ru/snap-store-black.svg)](https://snapcraft.io/cqtdeployer)


## Установить
Вы можете загрузить последнюю версию приложения [здесь](https://github.com/QuasarApp/Console-QtDeployer/releases).


## Пожертвовать
Если вы хотите помочь проекту, то вы можете пожертвовать небольшую сумму на наш биткойн-кошелек.

### Биткойн-адрес - 1NJNbDKmezcUcHRfzpBeq2fHeG21oEKX8Q
