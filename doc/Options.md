## General rules for using options
```
cqtdeployer -option1 value1 -option2 list, of, values ​​flag1 flag2 flag3
```

* All options must begin with a '-' sign.
* After specifying the desired option, you must specify the value.
* Some options support enumerations (arrays of values). When initializing such options, specify the list of values through ',' without prabels. If you use any programming language and call the cqtdeployer process, then the array of values must be passed as 1 parameter.
* Some options support redistributions with the ability to select the target for which the flag will be applied. Target and values are separated using the ';' sign. Example -libOut target1;value1,target2;value2,defaultValue.
* To include a flag, simply include the flag name in the list of options.

## Description of values in the table

* **-binDir [params]** - option without array support.
* **-bin [params, list]** - option with support for arrays. The delimiter used is ','.
* **-libOut [target1;path,path]** - parameter with support for selecting the target for which the flag value is set. As a separator, use ';'. Please note that the rule described above is used to enumerate parameter values. The last parameter does not have a target value, this means that this value will be set for all non-configured targets by default.
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
|   -targetPrefix [prefix;tar1,prefix;tar2]| Sets prefix for target( by default it is empty value)       |
|   -verbose [0-3]            | Shows debug log                                                 |
|  -qmlOut [prefix;path,path] | Sets path to qml out directory                                  |
|  -libOut [prefix;path,path] | Sets path to libraries out directory                            |
|  -trOut [prefix;path,path]  | Sets path to translations out directory                         |
|  -pluginOut [prefix;path,path]| Sets path to plugins out directory                            |
|  -binOut [prefix;path,path] | Sets path to binary out directory                               |
|  -recOut [prefix;path,path] | Sets path to recurses out directory                             |
|  -name [prefix;val,val]     | Sets name for prefix. If this if you do not specify a prefix, the value will be assigned to the default prefix ("")|
|  -description [prefix;val,val] | Sets description for prefix                                  |
|  -deployVersion [prefix;val,val] | Sets version for prefix                                    |
|  -releaseDate [prefix;val,val] | Sets release date for prefix                                 |
|  -icon [prefix;val,val]     | Sets path to icon for prefix                                    |
|  -qif [params]              | Create the QIF installer for deployement programm"              |
|                             | if skip the [params] then installer weel be created by default'"                           |
|                             | Examples:" },
|                             | cqtdeployer qif - for use default templates of qt installer framework."  |
|                             | cqtdeployer -qif path/to/folder/with/qifTemplate - for use custom templates of qt installer framework." |
|   deploySystem              | Deploys all libraries                                           |
|   clear                     | Deletes deployable files of the previous session.               |
|   force-clear               | Deletes the destination directory before deployment.            |
|                             | For example -runScript myApp.sh                                 |
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
* Некоторые параметры поддерживают пересисления с возможностью выбрать цель для которой будет применен флаг. Цель и значения разделяются при помощи знака ';'. Пример -libOut target1;value1,target2;value2,defaultValue.
* Для включения какого либо флага достаточно просто включить в список опций имя флага.

## Обозначения в таблице 

 * **-binDir [params]** - параметр без поддержки массивов.
 * **-bin [params,list]** - параметр c поддержкой массивов. В качестве разделителя используется ','.
 * **-libOut [target1;path,path]** - параметр c поддержкой выбора цели для которого бутет установленно значение флага. В качестве разделителя используется ';'. Обратитте внимание что для перечисления значений параметра используется правило описанное выше. Последний параметр не имеет значение таргета, это значит что это значение будет установленно для всех не настроенных таргетов по умолчанию.
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
|  -qmlOut [prefix;path,path]| Установит путь к папке с qml                                  |
|  -libOut [prefix;path,path]| Установит путь к папке с зависимостями                            |
|  -trOut [prefix;path,path] | Установит путь к папке с файламми переводов                         |
|  -pluginOut [prefix;path,path]| Установит путь к папке с плагинами                               |
|  -binOut [prefix;path,path]| Установит путь к папке с исполняемыми файлами                               |
|  -recOut [prefix;path,path]| Установит путь к папке с ресурсами                             |
|  -name [prefix;val,val]     | Установит имя префиксу. Если указать значение без префикса, то значение будет установленно префиксу по умолчанию. |
|  -description [prefix;val,val] | Установит описание префиксу                                  |
|  -deployVersion [prefix;val,val] | Установит версию префиксу                                    |
|  -releaseDate [prefix;val,val] | Установит дату выпуска префиксу                                 |
|  -icon [prefix;val,val]     | Установит путь к иконке или логотипу префиксу                                   |
|  -qif [params]              | Создать установщик QIF для развертываемой программы"|
|                             | если оставить параметр пустым то будет создан инсталлер По умолчанию|
|                             | Примеры:" },
|                             | cqtdeployer qif - для использования стандартных шаблонов фреймворка qt. "|
|                             | cqtdeployer -qif path/to/folder/with/qifTemplate - для использования пользовательских шаблонов среды установки qt. "|
|  -extraPlugin [list,params] | Устанавливает дополнительный путь для extraPlugin приложения |
|  -recursiveDepth [params]   | Устанавливает глубину поиска библиотек (по умолчанию 0)       |
|  -targetDir [params]        | Устанавливает целевой каталог (по умолчанию это путь к первому развертываемому файлу)|
|  -targetPrefix [prefix;tar1,prefix;tar2]| Устанавливает префикс для выбранной цели (по умолчанию это значение пустое)|
|  -verbose [0-3]             | Показывает дебаг лога                                          |
|   deploySystem              | Копирует все библиотеки кроме libc                        |
|   clear                     | Удаляет все старые файлы (с прошлого запуска)              |
|   force-clear               | Удаляет целевую директорию перед развертыванием              |
|                             | пример -runScript myApp.sh                                |
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
