


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
|   -extraPlugin [list,params]| Sets an additional path to extraPlugin of an app                |
|   -recursiveDepth [params]  | Sets the Depth of recursive search of libs (default 0)          |
|   -targetDir [params]       | Sets target directory(by default it is the path to the first deployable file)|
|   -verbose [0-3]            | Shows debug log                                                 |
|  -qmlOut [params]           | Sets path to qml out directory                                  |
|  -libOut [params]           | Sets path to libraries out directory                            |
|  -trOut [params]            | Sets path to translations out directory                         |
|  -pluginOut [params]        | Sets path to plugins out directory                              |
|  -binOut [params]           | Sets path to binary out directory                               |
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
|   noOverwrite               | Prevents replacing existing files.                              |
|   v / version               | Shows compiled version                                          |



#### Example: cqtdeployer -bin myApp -qmlDir ~/MyAppProject/qml -qmake ~/Qt/5.12.4/gcc_64/bin/qmake clear

#### Параметры:
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
|  -qmlOut [params]           | Установит путь к папке с qml                                  |
|  -libOut [params]           | Установит путь к папке с зависимостями                            |
|  -trOut [params]            | Установит путь к папке с файламми переводов                         |
|  -pluginOut [params]        | Установит путь к папке с плагинами                               |
|  -binOut [params]           | Установит путь к папке с исполняемыми файлами                               |
|  -extraPlugin [list,params] | Устанавливает дополнительный путь для extraPlugin приложения |
|  -recursiveDepth [params]   | Устанавливает глубину поиска библиотек (по умолчанию 0)       |
|  -targetDir [params]        | Устанавливает целевой каталог (по умолчанию это путь к первому развертываемому файлу)|
|  -verbose [0-3]             | Показывает дебаг лога                                          |
|   deploySystem              | Копирует все библиотеки кроме libc                        |
|   clear                     | Удаляет все старые файлы (с прошлого запуска)              |
|   force-clear               | Удаляет целевую директорию перед развертыванием              |
|                             | пример -runScript myApp.sh                                |
|  allQmlDependes             | Извлекает все библиотеки qml.          |
|                             | (не рекомендуется, так как занимает много памяти)         |
|  deploySystem-with-libc     | Копирует все зависимости в том числе и libc     |
|  noStrip                    | Пропускает шаг strip                                      |
|  noTranslations             | Пропускает файлы переводов                                |
|                             | не работает без qmake и в snap                            |
|   noOverwrite               | Запрещает замену уже существующих файлов.                |
|  v / version                | Показывает версию приложения                                          |


#### Пример: cqtdeployer -bin myApp -qmlDir ~/MyAppProject/qml -qmake ~/Qt/5.12.4/gcc_64/bin/qmake clear
