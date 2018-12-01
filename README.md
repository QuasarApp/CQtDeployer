# Welcome to the Console Qt Deployer!
# ![Logo](/res/icon.png)

***************************
## What is Console Qt Deployer
The Console QtDeployer is console veriosn of the [QtDeployer](https://quasarapp.github.io/QtDeployer/).

Key differences of this program:
* Performance-this program deploys the application several times faster (up to 10 seconds)
* Flexibility-this application has flags that will help you configure the deployment as you need and your project

## Attention
##### This project is designed to build projects consisting of only one pro file, if you need to collect a large project, then you need to use our [template](https://quasarapp.github.io/QtDeployTemplate/) to build and deploy qt applications

### How to use

#### Usage: cqtdeployer <-bin    [params]> [options]

#### Options:
| Option                      | Descriptiion                                              |
|-----------------------------|-----------------------------------------------------------|
|   help / h                  | show help.                                                |
|   always-overwrite          | Copy files even if the target file exists.                |
|   -bin    [params]          | deployment binry.                                         |
|   -qmlDir [params]          | qml datadir. for example -qmlDir ~/my/project/qml         |
|   deploy-not-qt             | deploy all libs                                           |
|   -qmake  [params]          | qmake path. for example                                   |
|                             | -qmake ~/Qt/5.11.1/gcc_64/bin/qmake                       |
|   -ignore [list,params]     | ignore filter for libs                                    |
|                             | for example -ignore libicudata.so.56,libicudata2.so.56    |
|   clear                     | delete all old deploy data                                |
|  -runScript [params]        | set new name of out file (AppRun.sh by default and) (linux omly)|
|                             | for example -runScript myApp.sh                           |
|  allQmlDependes             | This flag will force to extract all qml libraries.        |
|                             | (not recommended, as it takes up a lot of memory)         |
|  -libDir [list,params]      | set additional path for extralib of app                   |
|                             | for example -libDir ~/myLib,~/newLibs                     |
|  -extraPlugin [list,params] | set additional path for extraPlugin of app                |
|  -recursiveDepth [params]   | set Depth for recursive search of libs (default 0)        |
|  verbose                    | show debug log                                            |



#### Example: cqtdeployer -bin myApp -qmlDir ~/MyAppProject/qml -qmake ~/Qt/5.11.1/gcc_64/bin/qmake clear


## Build for Linux 
  -  qmake & make or install qt creator and build on qtcreator


## Install 
You can download the latest version of the application [here](https://github.com/QuasarApp/Console-QtDeployer/releases).

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

## Внимание
##### Данный проект преднозначен для сборки проектов состоящих всего из одного pro файла, если вам необходимо собрать крупный проект то вам нужно воспользоватся нашим [шаблоном](https://quasarapp.github.io/QtDeployTemplate/) для сборки и развертывания qt приложений 

### Как использовать
#### пример : cqtdeployer <-bin    [params]> [options]

#### Options:
| Option                      | Descriptiion                                              |
|-----------------------------|-----------------------------------------------------------|
|   help / h                  | Показать справку                                                |
|   always-overwrite          | Копирует файлы с заменой уже существующих                |
|   -bin    [params]          | Исполняемый файл над которым будет выплнятся деплои                                        |
|   -qmlDir [params]          | Папка qml. пример -qmlDir ~/my/project/qml   |
|   deploy-not-qt             | Копировать все библиотеки                                          |
|   -qmake  [params]          | Путь к qmake. пример                                   |
|                             | -qmake ~/Qt/5.11.1/gcc_64/bin/qmake                       |
|   -ignore [list,params]     | Список библиотек для игнорирования                                     |
|                             | пример -ignore libicudata.so.56,libicudata2.so.56    |
|   clear                     | удалит все старые файлы (с прошлого запуска)                               |
|  -runScript [params]        | установить новое имя результирующего файла (AppRun.sh по умолчанию) (только linux) |
|                             | пример -runScript myApp.sh                           |
|  allQmlDependes             | Этот флаг заставит извлекать все библиотеки qml.        |
|                             | (не рекомендуется, так как занимает много памяти)         |
|  -libDir [list,params]      | установит дополнительные пути к библиотекам               |
|                             | пример -libDir ~/myLib,~/newLibs                          |
|  -extraPlugin [list,params] | установить дополнительный путь для extraPlugin приложения |
|  -recursiveDepth [params]   | установит глубену поиска библиотек (по умолчанию 0)       |
|  verbose                    | show debug log                                            |


#### Пример: cqtdeployer -bin myApp -qmlDir ~/MyAppProject/qml -qmake ~/Qt/5.11.1/gcc_64/bin/qmake clear

## Build для Linux 
  -  qmake & make или установите qtcreator и соберите проект с qt5.9 или выше.


## Установить
Вы можете загрузить последнюю версию приложения [здесь](https://github.com/QuasarApp/Console-QtDeployer/releases).


## Пожертвовать
Если вы хотите помочь проекту, то вы можете пожертвовать небольшую сумму на наш биткойн-кошелек.

### Биткойн-адрес - 1NJNbDKmezcUcHRfzpBeq2fHeG21oEKX8Q
