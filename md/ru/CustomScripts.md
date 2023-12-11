# Пользовательские скрипты

После версии 1.5.1 cqtdeployer поддерживает собственные переменные в пользовательских скриптах.
Для использования пользовательских сценариев используйте параметр **runScript**.

### Примеры:

```bash
cqtdeployer -bin MyExecutable.exe -runScript MyExecutable,path/to/my/custom/script.bat 
```

```bash
cqtdeployer -bin MyExecutable -runScript MyExecutable,path/to/my/custom/script.sh 
```

## Список поддерживаемых переменных:

* CQT_LIB_PATH - это относительный путь к библиотекам развернутого дистрибутива.
* CQT_QML_PATH - это относительный путь к библиотекам qml развернутого дистрибутива.
* CQT_PLUGIN_PATH - это относительный путь к плагинам qt развернутого дистрибутива.
* CQT_BIN_PATH - это относительный путь к целям развернутого дистрибутива.

* CQT_SYSTEM_LIB_PATH - это относительный путь к системным библиотекам развернутого дистрибутива.
* CQT_BASE_NAME - это базовое имя исполняемого файла, который будет запущен после запуска этого скрипта.
* CQT_CUSTOM_SCRIPT_BLOCK - это код из опции customScript
* CQT_RUN_COMMAND - это команда для запуска приложения. Требуется переменная BASE_DIR. Важно: Эта переменная уже содержит символы перенапровления аргументов скрипта в исполняемый файл.

## Шаблоны скриптов запуска

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


Или вы можете получить последнюю версию этих скриптов из github [репозитория](https://github.com/QuasarApp/CQtDeployer/tree/main/src/Deploy/src/ScriptsTemplates).
