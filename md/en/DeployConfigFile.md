# Deployment file

**Deployment file** - it's a json file containing the parameters for cqtdeployer. This file supports all parameters except confFile, since this parameter is responsible for connecting and initializing the file itself.

### Configuration file structure

```json
{
    "Bool option": true/false,
    "Option": "Value",
    "Enumeration-enabled option": [
        "Value 1",
        "Value 2",
        "Value 3",
    ],
    "Option with support for multilevel enumerations": [
        [
           "Value A1",
           "Value A2"
        ],
        [
           "Value B1",
           "Value B2"
        ],
        [
           "Value C1",
           "Value C2"
        ],
    ]

}
```

Read more about cqtdeployer options in the [options](Options.md) section.

Examples of the contents of this file:

File without using enumerated parameters
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

File Using Enumerated Parameters
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

The purpose of file is to simplify the cqtdeployer's call and move some of the parameters into a file. The parameters specified in the file have a lower priority than the parameters added to the console at the run.

### Creating of the file

#### The first method

1. Call `cqtdeployer init` to initialize the file for a single-package application.
    Or `cqtdeployer -init multi` to initialize the file for a project with multiple packages.
2. Open CQtDeployer.json and edit it to fit your needs.
3. Call 'cqtdeployer' in the directory with the file 'CQtDeployer.json', or specify the path to it using the flag '-confFile path/to/my/CQtDeployer.json'


#### The second method
Call the cqtdeployer with a set of parameters you need and specify the path to the file using -confFile flag (the file should be absent). If the file exists, cqtdeployer will start unloading the parameters from it.

An example:
``` bash
cqtdeployer -confFile deploy.json -bin CQtDeployerInstaller -targetDir ./test -qmlOut myqml -binOut mybin -libOut mylib -pluginOut myplugins noStrip noTranslations -recursiveDepth 5 -extraPlugin sqldrivers,audio,mediaservice force-clear deploySystem noOverwrite
```

Where:

* cqtdeployer - the call of cqtdeployer
* -confFile deploy.json - the path to the file,that should be created
* "-bin CQtDeployerInstaller -targetDir ./test -qmlOut myqml -binOut mybin -libOut mylib -pluginOut myplugins noStrip noTranslations -recursiveDepth 5 -extraPlugin sqldrivers,audio,mediaservice force-clear deploySystem noOverwrite" - necessary parameters.


### Using of the deployment file.

 Use the flag -confFile [path to the file] to make cqtdeployer start reading the parameters from the file

#### Example:

cqtdeployer -qmake ~/Qt/bin/qmake -confFile ./deploy.json

