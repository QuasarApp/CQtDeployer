# Custom Scripts

After version 1.5.1 cqtdeployer support self variables in the custom scripts.
For using custom scripts use the **runScript option**

### Exmaples :

```bash
cqtdeployer -bin MyExecutable.exe -runScript MyExecutable,path/to/my/custom/script.bat 
```

```bash
cqtdeployer -bin MyExecutable -runScript MyExecutable,path/to/my/custom/script.sh 
```

## Suported variables list:

* CQT_LIB_PATH - are relative path to libraries of a deployed distribution.
* CQT_QML_PATH - are relative path to qml libraries of a deployed distribution.
* CQT_PLUGIN_PATH - are relative path to qt plugins of a deployed distribution.
* CQT_BIN_PATH - are relative path to targets of a deployed distribution.

* CQT_SYSTEM_LIB_PATH - are relative path to system libraries of a deployed distribution.
* CQT_BASE_NAME - are base name of the executable that will be launched after run this script.
* CQT_CUSTOM_SCRIPT_BLOCK - This is code from the customScript option
* CQT_RUN_COMMAND - This is command for run application. Require BASEDIR variable. Note: This variable already contains symbols for redirect input script arguments to executable.


## The Scripts tempalte

### Linux 

```bash
#!/bin/sh

BASE_DIR=$(dirname "$(readlink -f "$0")")
export LD_LIBRARY_PATH="$BASE_DIR"CQT_LIB_PATH:"$BASE_DIR":$LD_LIBRARY_PATH
export QML_IMPORT_PATH="$BASE_DIR"CQT_QML_PATH:$QML_IMPORT_PATH
export QML2_IMPORT_PATH="$BASE_DIR"CQT_QML_PATH:$QML2_IMPORT_PATH
export QT_PLUGIN_PATH="$BASE_DIR"CQT_PLUGIN_PATH:$QT_PLUGIN_PATH
export QTWEBENGINEPROCESS_PATH="$BASE_DIR"CQT_BIN_PATH/QtWebEngineProcess
export QTDIR="$BASE_DIR"
export CQT_PKG_ROOT="$BASE_DIR"
export CQT_RUN_FILE="$BASE_DIR/CQT_BASE_NAME.sh"

export QT_QPA_PLATFORM_PLUGIN_PATH="$BASE_DIR"CQT_PLUGIN_PATH/platforms:$QT_QPA_PLATFORM_PLUGIN_PATH

CQT_CUSTOM_SCRIPT_BLOCK

CQT_RUN_COMMAND
```

### Windows 

```bash

@echo off
SET BASE_DIR=%~dp0
SET PATH=%BASE_DIR%CQT_LIB_PATH;%PATH%;CQT_SYSTEM_LIB_PATH
SET CQT_PKG_ROOT=%BASE_DIR%
SET CQT_RUN_FILE=%BASE_DIR%CQT_BASE_NAME.bat

CQT_CUSTOM_SCRIPT_BLOCK

CQT_RUN_COMMAND
```


Or you can get last version of this scripts from the github [repository](https://github.com/QuasarApp/CQtDeployer/tree/main/src/Deploy/src/ScriptsTemplates).
