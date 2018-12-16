# Welcome to the Console Qt Deployer!
# ![Logo](/res/icon.png)

***************************
## What is Console Qt Deployer
The Console QtDeployer is console veriosn of the [QtDeployer](https://quasarapp.github.io/QtDeployer/).

Key differences of this program:
* Performance-this program deploys the application several times faster (up to 10 seconds)
* Flexibility-this application has flags that will help you configure the deployment as you need and your project


### How to use

#### Usage: cqtdeployer <-bin    [params]> [options]

#### Options:
| Option                      | Descriptiion                                                    |
|-----------------------------|-----------------------------------------------------------------|
|   help / h                  | show help.                                                      |
|   always-overwrite          | Copy files even if the target file exists.                      |
|   -bin    [list, params]    | deployment binry or directory. example -bin ~/my/project/bin/,~/my/project/bin.exe|
|   -binDir [params]          | folder with deployment binaries with recursive search. WARNING this flag support only 'so', 'dll' and 'exe' files. If you want deploy linux binary then use '-bin' flag |
|   -qmlDir [params]          | qml datadir. for example -qmlDir ~/my/project/qml               |
|   deploy-not-qt             | deploy all libs                                                 |
|   -qmake  [params]          | qmake path. for example                                         |
|                             | -qmake ~/Qt/5.11.1/gcc_64/bin/qmake                             |
|   -ignore [list,params]     | ignore filter for libs                                          |
|                             | for example -ignore libicudata.so.56,libicudata2.so.56          |
|   clear                     | delete all old deploy data                                      |
|  -runScript [params]        | set new name of out file (AppRun.sh by default and) (linux omly)|
|                             | for example -runScript myApp.sh                                 |
|  allQmlDependes             | This flag will force to extract all qml libraries.              |
|                             | (not recommended, as it takes up a lot of memory)               |
|  -libDir [list,params]      | set additional path for extralib of app                         |
|                             | for example -libDir ~/myLib,~/newLibs                           |
|  -extraPlugin [list,params] | set additional path for extraPlugin of app                      |
|  -recursiveDepth [params]   | set Depth for recursive search of libs (default 0)              |
|  -targetDir [params]        | set target Dir for binaryes (default is path of first target)   |
|  verbose                    | show debug log                                                  |



#### Example: cqtdeployer -bin myApp -qmlDir ~/MyAppProject/qml -qmake ~/Qt/5.11.1/gcc_64/bin/qmake clear


## Build for Linux 
  -  ./staticBuild
  -  ./sharedBuild
  -  ./snapBuild

## Install 
You can download the latest version of the application [here](https://github.com/QuasarApp/Console-QtDeployer/releases).

### Snap 
[![Get it from the Snap Store](https://snapcraft.io/static/images/badges/en/snap-store-black.svg)](https://snapcraft.io/cqtdeployer)

#### Attention!!!
This application may not work stably in an isolated container. To solve these problems, use the console installation in the Classic box.
```bash
snap install cqtdeployer --classic 
```

## Donate
If you want to help the project, then you can donate a small amount to our bitcoin wallet.

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
|   help / h                  | Показать справку                                          |
|   always-overwrite          | Копирует файлы с заменой уже существующих                 |
|   -bin    [list, params]    | Развертываемый файл или папка. пример -bin ~/my/project/bin/,~/my/project/bin.exe|
|   -binDir [params]          | Папка с развертываемыми файлами (с рекурсивным поиском). ВНИМАНИЕ! Этот флаг поддерживает только файлы 'so', 'dll' и 'exe'. Если вы хотите развернуть бинарный файл Linux, используйте флаг '-bin'  |
|   -qmlDir [params]          | Папка qml. пример -qmlDir ~/my/project/qml                |
|   deploy-not-qt             | Копировать все библиотеки                                 |
|   -qmake  [params]          | Путь к qmake. пример                                      |
|                             | -qmake ~/Qt/5.11.1/gcc_64/bin/qmake                       |
|   -ignore [list,params]     | Список библиотек для игнорирования                        |
|                             | Пример -ignore libicudata.so.56,libicudata2.so.56         |
|   clear                     | удалит все старые файлы (с прошлого запуска)              |
|  -runScript [params]        | установить новое имя результирующего файла (AppRun.sh по умолчанию) (только linux) |
|                             | пример -runScript myApp.sh                                |
|  allQmlDependes             | Этот флаг заставит извлекать все библиотеки qml.          |
|                             | (не рекомендуется, так как занимает много памяти)         |
|  -libDir [list,params]      | Установит дополнительные пути к библиотекам               |
|                             | Пример -libDir ~/myLib,~/newLibs                          |
|  -extraPlugin [list,params] | Установить дополнительный путь для extraPlugin приложения |
|  -recursiveDepth [params]   | Установит глубену поиска библиотек (по умолчанию 0)       |
|  -targetDir [params]        | Установит целевой коталог (по умолчанию это путь к первому развертываемому файлу)|
|  verbose                    | Показ дебаг лог                                        |


#### Пример: cqtdeployer -bin myApp -qmlDir ~/MyAppProject/qml -qmake ~/Qt/5.11.1/gcc_64/bin/qmake clear

## Build для Linux 
  -  ./staticBuild
  -  ./sharedBuild
  -  ./snapBuild

### Snap
[![Загрузите из Snap Store](https://snapcraft.io/static/images/badges/ru/snap-store-black.svg)](https://snapcraft.io/cqtdeployer)

#### Внимание!!!
это приложение может работать не стабильно в изолированном контейнере, Для решения этих проблемм используйте становку через консоль в рижеми Classic
```bash
snap install cqtdeployer --classic 
```


## Установить
Вы можете загрузить последнюю версию приложения [здесь](https://github.com/QuasarApp/Console-QtDeployer/releases).


## Пожертвовать
Если вы хотите помочь проекту, то вы можете пожертвовать небольшую сумму на наш биткойн-кошелек.

### Биткойн-адрес - 1NJNbDKmezcUcHRfzpBeq2fHeG21oEKX8Q
