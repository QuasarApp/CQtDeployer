# Файл развертывания

**Файл развертывания** - это json файл, содержащий в себе параметры для cqtdeployer. Данный файл поддерживает все параметры кроме confFile, так как данный параметр отвечает за подключение и инициализацию самого файла.

### Структура файла конфигурации
```json
{
    "Флаг": true/false,
    "Параметр": "Значение",
    "Параметер с поддержкой перечислений": [
        "Значение 1",
        "Значение 2",
        "Значение 3",
    ],
    "Параметр с поддержкой многоуровневых перечислений": [
        [
           "Значение A1",
           "Значение A2"
        ],
        [
           "Значение B1",
           "Значение B2"
        ],
        [
           "Значение C1",
           "Значение C2"
        ],
    ]

}
```

Подробно о параметрах cqtdeployer читайте в разделе [Параметры](Options.md).

Примеры содержимого данного файла:

Файл без использования параметров с перечислениями
```json
{
    "noOverwrite": false,
    "bin": "./",
    "binDir": "./",
    "qmlDir": "./",
    "deploySystem": false,
    "deploySystem-with-libc": false,
    "qmake": "my/custom/path/to/qmake",
    "ignore": "ignoreLib.dll",
    "ignoreEnv": "/my/ignore/path",
    "clear": true,
    "force-clear": false,
    "allQmlDependes": false,
    "libDir": "./",
    "recursiveDepth": 3,
    "extraLibs": "myExtraLib (libssl)",
    "extraPlugin": "myExtraPulgin (platforms)",
    "targetDir": "./Distro",
    "targetPackage": "",
    "noStrip": false,
    "extractPlugins": false,
    "noTranslations": false,
    "qmlOut": "qml",
    "libOut": "lib",
    "trOut": "tr",
    "pluginOut": "plug",
    "binOut": "bin",
    "recOut": "rec",
    "verbose": 3,
    "qif": false,
    "noCheckRPATH": false,
    "noCheckPATH": false,
    "name": "mainApplication",
    "description": "this is description for default package",
    "deployVersion": "1.0.0",
    "releaseDate": "yyyy-MM-dd",
    "icon": "/path/main/icon.png",
    "publisher": "main team",
    "customScript": "echo 'I am use custom script!!!'"
}

```

Файл с использованием параметров с перечислениями
``` json
{
    "noOverwrite": false,
    "bin": "./",
    "binDir": "./",
    "qmlDir": [
        [
            "package2",
            "./TestQMLWidgets/"
        ],
        [
            "./qml/for/All"
        ]
    ],

    "deploySystem": false,
    "deploySystem-with-libc": false,
    "qmake": "this parameter should contain the path to your qmake, for Windows this field is required. On Linux, you can remove it, then qmake will be found by rpath.",
    "ignore": "ignoreLib.dll",
    "ignoreEnv": "/my/ignore/path",
    "clear": true,
    "force-clear": false,
    "allQmlDependes": false,
    "libDir": "./",
    "recursiveDepth": 3,
    "extraLibs": "myExtraLib (libssl)",
    "extraPlugin": "myExtraPulgin (platforms)",
    "targetDir": "./Distro",
    "targetPackage": [
        [
            "/package1/",
            "TestOnlyC"
        ],
        [
            "/package2/",
            "TestQMLWidgets"
        ]
    ],
    "noStrip": false,
    "extractPlugins": false,
    "noTranslations": false,
    "qmlOut": "qml",
    "libOut": "lib",
    "trOut": "tr",
    "pluginOut": "plug",
    "binOut": "bin",
    "recOut": "rec",
    "verbose": 3,
    "qif": false,
    "noCheckRPATH": false,
    "noCheckPATH": false,
    "name": [
        [
            "package2",
            "package2Application"
        ],
        [
            "mainApplication"
        ]
    ],
    "description": [
        [
            "package2",
            "this is description for package 'package2'"
        ],
        [
            "this is description for default package"
        ]
    ],
    "deployVersion": [
        [
            "package2",
            "1.0.0"
        ],
        [
            "1.0.0"
        ]
    ],
    "releaseDate": [
        [
            "package2",
            "yyyy-MM-dd"
        ],
        [
            "yyyy-MM-dd"
        ]
    ],
    "icon": [
        [
            "package2",
            "/path/icon.png"
        ],
        [
            "/path/main/icon.png"
        ]
    ],
    "publisher": [
        [
            "package2",
            "team of package2"
        ],
        [
            "main team"
        ]
    ],
    "customScript": "echo 'I am use custom script!!!'"
}

```

Цель файла- упростить вызов cqtdeployer и переместить часть или все параметры в файл. Все параметры, указанные в файле, имеют более низкий приоритет, чем параметры, добавленные в консоль при запуске.

### Создание файла

#### Способ 1

1. Вызовите `cqtdeployer init`  Для инициализации файла для приложения состоящего из одного пакета.
   Или `cqtdeployer -init multi` Для инициализации файла для проекта с несколькими пакетами.
2. Откройте CQtDeployer.json и отредактируйте его под свои нужды.
3. Вызовите cqtdeployer в дериктории с файлом CQtDeployer.json, или укажите путь к нему используя флаг -confFile path/to/my/CQtDeployer.json

#### Способ 2

Вызовите cqtdeployer с нужным вам набором параметров и укажите путь к файлу при помощи флага -confFile (файл должен отсутствовать). Если файл будет существовать, то cqtdeployer начнет выгружать из него параметры.

Пример:
``` bash
cqtdeployer -confFile deploy.json -bin CQtDeployerInstaller -targetDir ./test -qmlOut myqml -binOut mybin -libOut mylib -pluginOut myplugins noStrip noTranslations -recursiveDepth 5 -extraPlugin sqldrivers,audio,mediaservice force-clear deploySystem noOverwrite
```
Где:

* cqtdeployer - вызов cqtdeployer
* -confFile deploy.json - путь к файлу, который нужно создать
* "-bin CQtDeployerInstaller -targetDir ./test -qmlOut myqml -binOut mybin -libOut mylib -pluginOut myplugins noStrip noTranslations -recursiveDepth 5 -extraPlugin sqldrivers,audio,mediaservice force-clear deploySystem noOverwrite" - необходимые параметры.


### Использование файла развертывания.

Чтобы cqtdeployer начал читать параметры из файла, используйте флаг -confFile [путь к файлу]

#### Пример:

cqtdeployer -qmake ~/Qt/bin/qmake -confFile ./deploy.json

