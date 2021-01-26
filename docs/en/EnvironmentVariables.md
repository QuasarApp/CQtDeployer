# Environment variables of the CQtDeployer
This page contains information about environment variables of the default launcher script.

## Initialization 
All environment variables initialized in the default launcher script. So if you use the runScript option then you must be reunited all needed variables manually.


## Variables list

| Variable name  | Description  |
|---|---|
| CQT_PKG_ROOT  | This variable contains path to root of the current package.|

## Description of definitions

**Root of the current package** - This root is local root for any separate packages.


Example :
``` bash
.
├── pakcage1 << This is local root of the package1 
│   ├── bin
│   ├── lib
│   ├── plugins
│   ├── qml
│   └── translations
├── package2 << This is local root of the package2 
│   ├── bin
│   ├── lib
│   ├── plugins
│   ├── qml
│   └── translations
└── pakcage3 << This is local root of the package3
    ├── bin
    ├── lib
    ├── plugins
    ├── qml
    └── translations

```



