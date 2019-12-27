## General rules for using options
```
cqtdeployer -option1 value1 -option2 list, of, values ​​flag1 flag2 flag3
```

* All options must begin with a '-' sign.

* After specifying the desired option, you must specify the value.

* Some options support enumerations (arrays of values). When initializing such options, specify the list of values through ',' without prabels. If you use any programming language and call the cqtdeployer process, then the array of values must be passed as 1 parameter.

* To include a flag, simply include the flag name in the list of options.

## Description of values in the table

* **-binDir [params]** - option without array support.

* **-bin [params, list]** - option with support for arrays. The delimiter used is ','.

* **clear** - flag


## Options:
| Option                      | Descriptiion                                                    |
|-----------------------------|-----------------------------------------------------------------|
|   -bin    [list, params]    | Deployable file or folder. For example -bin ~/my/project/bin/,~/my/project/bin.exe|
|   -binDir [params]          | A folder which includes deployable files (recursive search).    |
|   -confFile [params]        | The path to the json file with all deployment configurations. Using this file, you can add the necessary options, thereby simplifying the command invocation in the console. However, the parameters in Kansol have a higher priority than in the file. For more info about this flag see [wiki](https://github.com/QuasarApp/CQtDeployer/wiki/DeployConfigFileEn)                              |
|   -qmlDir [params]          | Qml data dir. For example -qmlDir ~/my/project/qml              |
|   -qmake  [params]          | Qmake path. For example                                         |
|                             | -qmake ~/Qt/5.14.0/gcc_64/bin/qmake                             |
|   -ignore [list,params]     | The list of libs to ignore.                                     |
|                             | For example -ignore libicudata.so.56,libicudata2.so.56          |
|   -ignoreEnv [list,params]  | The list of the environment to ignore.                          |
|                             | For example -ignoreEnv /bad/dir,/my/bad/Dir                     |
|   -libDir [list,params]     | Sets additional paths for extra libs of an app.                 |
|                             | For example -libDir ~/myLib,~/newLibs                           |
|   -extraLibs [list,params]  | Sets the mask of the library name for forced copying. Example: "-extraLib mySql" - forces to copy all libraries whose names contain mySql to the project folder. This option is case sensitive. |
|   -extraPlugin [list,params]| Sets an additional path to extraPlugin of an app                |
|   -recursiveDepth [params]  | Sets the Depth of recursive search of libs (default 0)          |
|   -targetDir [params]       | Sets target directory(by default it is the path to the first deployable file)|
|   -verbose [0-3]            | Shows debug log                                                 |
|  -qmlOut [params]           | Sets path to qml out directory                                  |
|  -libOut [params]           | Sets path to libraries out directory                            |
|  -trOut [params]            | Sets path to translations out directory                         |
|  -pluginOut [params]        | Sets path to plugins out directory                              |
|  -binOut [params]           | Sets path to binary out directory                               |
|  -customScript [scriptCode] | Insert extra code inTo All run script.                          |
|   deploySystem              | Deploys all libraries                                           |
|   clear                     | Deletes deployable files of the previous session.               |
|   force-clear               | Deletes the destination directory before deployment.            |
|   allQmlDependes            | Extracts all the qml libraries.                                 |
|                             | (not recommended, as it takes great amount of computer memory)  |
|   version / v               | Shows compiled version                                          |
|   help / h                  | Shows help.                                                     |
|   deploySystem-with-libc    | deploy all libs libs (only linux)                               |
|   noStrip                   | Skips strip step                                                |
|   noTranslations            | Skips the translations files.                                   |
|                             | It doesn't work without qmake and inside a snap package         |
|   extractPlugins            | This flag will cause cqtdeployer to retrieve dependencies from plugins. Starting with version 1.4, this option has been disabled by default, as it can add low-level graphics libraries to the distribution, which will not be compatible with equipment on users' hosts. |
|   noOverwrite               | Prevents replacing existing files.                              |
|   noAutoCheckQmake          | Disables automatic search of paths to qmake in executable files.|
|   v / version               | Shows compiled version                                          |



#### Example: cqtdeployer -bin myApp -qmlDir ~/MyAppProject/qml -qmake ~/Qt/5.12.4/gcc_64/bin/qmake clear


## Общие правила использования параметров
```
cqtdeployer -option1 value1 -option2 list,of,values flag1 flag2 flag3
```

* Все параметры должны начинается с знака '-'.

* После указания нужного параметра нужно в обязательном порядке указать значение

* Некоторые параметры поддерживают перечисления (массивы значений). При инициализации таких параметров укажите список значений через ',' без прабелов. Если вы используете какой либо язык программирования и вызываете процесс cqtdeployer то массив значений должен передаваться как 1 параметр.

* Для включения какого либо флага достаточно просто включить в список опций имя флага.

## Обозначения в таблице 

 * **-binDir [params]** - параметр без поддержки массивов.

 * **-bin [params,list]** - параметр c поддержкой массивов. В качестве разделителя используется ','.

* **clear** - флаг 

## Параметры:
| Option                      | Descriptiion                                              |
|-----------------------------|-----------------------------------------------------------|
|   help / h                  | Показывает справку                                          |
|   -bin    [list, params]    | Развертываемый файл или папка. пример -bin ~/my/project/bin/,~/my/project/bin.exe|
|   -binDir [params]          | Папка с развертываемыми файлами (с рекурсивным поиском). |
|   -confFile [params]        | Путь к файлу json со всеми конфигурациями развертывания. С помощью этого файла можно добавить нужные опции, тем самым упрастить вызов комманды в консоле. Однако пораметры в кансоле имеют больший приоритет чем в файле. Для получения дополнительной информации об этом флаге см. [Вики](https://github.com/QuasarApp/CQtDeployer/wiki/DeployConfigFileRu) |
|   -qmlDir [params]          | Папка qml. пример -qmlDir ~/my/project/qml                |
|   -qmake  [params]          | Путь к qmake. пример                                      |
|                             | -qmake ~/Qt/5.14.0/gcc_64/bin/qmake                       |
|   -ignore [list,params]     | Список библиотек для игнорирования                        |
|                             | Пример -ignore libicudata.so.56,libicudata2.so.56         |
|   -ignoreEnv [list,params]  | Список путей для игнорирования.                          |
|                             | Пример -ignoreEnv /bad/dir,/my/bad/Dir                                |
|  -libDir [list,params]      | Устанавливает дополнительные пути к библиотекам               |
|                             | Пример -libDir ~/myLib,~/newLibs                          |

|  -extraLibs [list,params]   | Устанавливает маску имени библиотеки, для принудительного копирования, Пример  "-extraLib mySql" - заставит скопировать в папку с проектом все библиотеки чии имена содержат mySql. Данная опция чуствительна к регистру.  |
|  -qmlOut [params]           | Установит путь к папке с qml                                  |
|  -libOut [params]           | Установит путь к папке с зависимостями                            |
|  -trOut [params]            | Установит путь к папке с файламми переводов                         |
|  -pluginOut [params]        | Установит путь к папке с плагинами                               |
|  -binOut [params]           | Установит путь к папке с исполняемыми файлами                               |
|  -customScript [scriptCode] | Установит дополнительный код в скрипты запуска.                                  |
|  -extraPlugin [list,params] | Устанавливает дополнительный путь для extraPlugin приложения |
|  -recursiveDepth [params]   | Устанавливает глубину поиска библиотек (по умолчанию 0)       |
|  -targetDir [params]        | Устанавливает целевой каталог (по умолчанию это путь к первому развертываемому файлу)|
|  -verbose [0-3]             | Показывает дебаг лога                                          |
|   deploySystem              | Копирует все библиотеки кроме libc                        |
|   clear                     | Удаляет все старые файлы (с прошлого запуска)              |
|   force-clear               | Удаляет целевую директорию перед развертыванием              |
|  allQmlDependes             | Извлекает все библиотеки qml.          |
|                             | (не рекомендуется, так как занимает много памяти)         |
|  deploySystem-with-libc     | Копирует все зависимости в том числе и libc     |
|  noStrip                    | Пропускает шаг strip                                      |
|  extractPlugins             | Заставит извлекать зависимости из плагинов, данная опция начиная с версии 1.4 отключена по умолчанию, так как может добавить в дистрибутив низкоуровневые графические библиотеки, которые будут не совместимы с железом на хостах пользователей.  |
|  noTranslations             | Пропускает файлы переводов                                |
|                             | не работает без qmake и в snap                            |
|   noOverwrite               | Запрещает замену уже существующих файлов.                |
|   noAutoCheckQmake          | Отключает автоматический поиск путей к qmake в исполняемых файлах.|
|  v / version                | Показывает версию приложения                                          |


#### Пример: cqtdeployer -bin myApp -qmlDir ~/MyAppProject/qml -qmake ~/Qt/5.12.4/gcc_64/bin/qmake clear
