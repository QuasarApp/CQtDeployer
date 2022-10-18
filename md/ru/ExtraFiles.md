# Добавление дополнительных данных

Добавление дополнительных данных в дистрибутив.


CQtDeployer поддерживает работу с дополнительными файлами данных.

Для этого используйте параметры -extraData и -extraDataOut.

## Примеры:

### Добавление собственных файлов в корневой каталог дистрибутива.

```
cqtdeployer -extraData путь / к / my / customFile
```

Или в configFile.json

```json
{
    ...
    "extraData": "./path/to/my/customFile"
    ...
}
```

Эта команда поддерживает список аргументов.


```bash
cqtdeployer -extraData path/to/my/customFile,path/to/my/customFile2,path/to/my/customFile3
```

Или в configFile.json

```json
{
    ...
    "extraData": [
        "./path/to/my/customFile",
        "./path/to/my/customFile2",
        "./path/to/my/customFile3"
    ]
    ...
}
```

### Изменение расположения дополнительных файлов данных.

Для изменения местоположения используйте параметры extraDataOut. Эта опция меняет путь назначения относительно корня дистрибутива.

### Пример. Добавьте пользовательские скрипты в папку скриптов в Distribution.

```bash
cqtdeployer -extraData /path/to/myScripts.sh,/another/path/to/myScripts.sh -extraDataOut scripts
```

Или в configFile.json

```json
{
    ...
    "extraData": [
        "./path/to/myScripts.sh",
        "./another/path/to/myScripts.sh"
    ],
    "extraDataOut": "скрипты"
    ...
}
```

## Пакеты
Опции extraData и extraDataOut поддерживают работу с пакетами. Дополнительную информацию о пакетах см. На [странице](Packing.md) упаковки.

Иногда нам нужно создать многопакетное приложение. Например, создайте установщик для приложения client-server с клиентскими и серверными приложениями. Клиент и сервер имеют собственные файлы конфигурации по умолчанию. Мы добавляем файлы config с помощью опции extraData.

Для создания большого дистрибьютора я рекомендую использовать [confFile](DeployConfigFile.md).

## Пример применения мультиупаковки.


```json
{

    "корзина": [
        "myClientExecutable",
        "myServerExecutable"
    ],

    "targetPackage": [
        ["Клиент", "myClientExecutable"],
        ["Сервер", "myServerExecutable"]
    ],

    "extraData": [
        ["Клиент", "./path/to/clientConfig.conf"],
        ["Сервер", "./path/to/serverConfig.conf"]
    ],
    "extraDataOut": [
        ["Клиент", "defaultConfigurations"],
        ["Сервер", "defaultConfigurations"]
    ]
}
```

Эта конфигурация создает дистрибутив с двумя приложениями (клиент и сервер). Каждое приложение имеет собственный файл конфигурации в папке defaultConfigurations.
