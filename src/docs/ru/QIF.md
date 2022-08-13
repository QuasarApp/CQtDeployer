# Использование Qt Installer Framwork

Общие правила использования Qt Installer Framwork

### Что такое Qt Installer Framwork

**Qt Installer Framework** (QIF) предоставляет набор инструментов и утилит для создания установщиков для поддерживаемых настольных платформ Qt: Linux, Microsoft Windows и OS X.
В **CQtDeployer** QIF доступен в виде опции qif.

### Как использовать опцию qif

Опция qif является булевским параметром для создания Установочного файла.

#### Пример:

``` bash
    cqtdeployer ... qif
```

Где:
* **...** - список других опций.
* **qif** - опция для использования qt installer framework с шаблоном по умолчанию.


По умолчанию cqtdeployer создаст установочный файл с одним пакетом в котором будет лежать весь ваш дистрибутив.
Если вы хотите его настроить вы должны использовать [параметр c поддержкой выбора пакета](Options.md)
Для создание нового пакета используйте флаг -targetPackage [package;tar1]


Где:

* **package** - это имя пакета
* **tar1** - это название таргета который должен попасть в этот пакет.

Или вы можете описать пакеты в [конфигурационном файле](DeployConfigFile.md)

```json
{
"targetPackage": [
    [
        "package",
        "tar1"
    ]
],
}
```

# Пользовательский Шаблон для QIF

Начиная с Версии CQtDeployer 1.5 вы можете использовать свой собственный шаблон установщика. Для этого вы должны передать в параметр qif путь к вашему шаблону.

```
cqtdeployer ... -qif /path/to/my/custom/qif/template
```

### Шаблон 

Шаблон qif долже содержать 2 папки:
* packages 
* config

### Внимание

Название пакетов в папке packages должны совподать с названием пакетов при развертывание.

Например:

```json
"targetPackage": [
    [
        "myCustomInstaller",
        "tar1"
    ]
],
```

```bash
.
├── config
│   ├── config.xml
│   └── style.qss
└── packages
    └── myCustomInstaller
        └── meta
            ├── installscript.qs
            └── package.xml

4 directories, 4 files

```

Если имена папок не совподут с именами пакетов то CQtDeployer будет использовать стандартный пакет для QIF.

Если у вас только один пакет, вы можете использовать опцию **name** для установки имени приложения.

```
cqtdeployer ... -qif /path/to/my/custom/qif/template -name myCustomInstaller
```

#### Не забудьте переименовать папку вашего пакета в имя пакета из опции имени.


```bash
.
├── config
│   ├── config.xml
│   └── style.qss
└── packages
    └── myCustomInstaller  << "This is folder of the your package"
        └── meta
            ├── installscript.qs
            └── package.xml

4 directories, 4 files

```


## Инициализация шаблона qifw

Для инициализации шаблонов по умолчанию вы можете использовать параметр getDefaultTemplate.
Параметр getDefaultTemplate извлекает шаблон по умолчанию для cqtdeployer. Это может быть очень удобно, если вы хотите переопределить поведение установщика или другого пакета распространения по умолчанию.

### Пример

### Получение шаблона фреймворка установщика qt по умолчанию.

```bash
cqtdeployer getDefaultTemplate qif
```

### Примечание

CQtDeployer пропустит создание каталога пакетов для шаблона, поскольку эта команда не содержит данных для развертывания.
Если вы хотите подготовить шаблон с конфигурациями пакетов, вам следует добавить развернутые данные в свою команду или в файл config.json, используя параметры bin или extraData.
Если вы создаете дистрибутив с несколькими пакетами, вам необходимо настроить свои пакеты в развертываемой команде.


#### Пример:

Извлечение шаблона с пакетами:

```bash
cqtdeployer getDefaultTemplate qif -bin myExecutable
```

Извлечение шаблона для распространения нескольких пакетов

```bash
cqtdeployer getDefaultTemplate qif -bin myExecutable1,myExecutable2 -targetPackage p1;myExecutable1,p2;myExecutable2
```

Вы также можете использовать файл конфигурации для настройки шаблонов.

Config.json

```json
{
    "qif": true,
    "bin": [
        "myExecutable1",
        "myExecutable2"
    ],
    "targetPackage": [
        ["p1", "myExecutable1"],
        ["p2", "myExecutable2"]
    ]
}
```

Запустите CQtDeployer для создания шаблона:

```bash
cqtdeployer -confFile Config.json getDefaultTemplate
```
