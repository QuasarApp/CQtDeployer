## General rules for using Qt Installer Framwork

### What is Qt Installer Framwork

The **Qt Installer Framework** (QIF) provides a set of tools and utilities to create installers for the supported desktop Qt platforms: Linux, Microsoft Windows, and OS X.
In **CQtDeployer** QIF availabel in qif option.

### How to use qif option

The qif option invoke with path to custo qt installer template or with default installer template.

#### For example:

``` bash
    cqtdeployer ... qif
```

Where:
* **...** - is list of other options.
* **qif** - option for use qt installer framework with default template.


``` bash
    cqtdeployer ... -qif path/to/my/template/folder
```

Where:
* **...** - is list of other options.
* **-qif** - option for use qt installer framework with custom template available in path/to/my/template/folder.
* **path/to/my/template/folder** - path to  template folder.

### Qt Install Template.

**Qt Install Template** - is forder with configuration and packages information of installer.
The configuration info should be in the config folder. for information about configuration see [qt documentation](https://doc.qt.io/qtinstallerframework/ifw-globalconfig.html)
In the packages folder, each package should be in a separate folder and contain meta-information.
If you have only 1 package, then CQtDeployer will deploy all deployed targets in one package.
If there are several, then the package names must match the names of the deployed targets.
In this case, CQtDeployer will deploy each target in a separate package.
Targets whose names do not match the package names will be deployed in the first package.
Packages are sorted by name.

For more information about packages see [qt documentantion](https://doc.qt.io/qtinstallerframework/ifw-component-description.html)


## Общие правила использования Qt Installer Framwork

### Что такое Qt Installer Framwork

**Qt Installer Framework** (QIF) предоставляет набор инструментов и утилит для создания установщиков для поддерживаемых настольных платформ Qt: Linux, Microsoft Windows и OS X.
В **CQtDeployer** QIF доступен в виде опции qif.

### Как использовать опцию qif

Опция qif вызывается с путем к пользовательскому шаблону установщика или с шаблоном установщика по умолчанию.

#### Пример:

``` bash
    cqtdeployer ... qif
```

Где:
* **...** - список других опций.
* **qif** - опция для использования qt installer framework с шаблоном по умолчанию.


```bash
    cqtdeployer ... -qif path/to/my/template/folder
```

Где:
* **...** - список других опций.
* ** - qif ** - опция для использования пользовательского шаблона установщика qt, доступным в path/to/my/template/folder.
* **path/to/my/template/folder** - путь к папке шаблона.

### Шаблон установщика.

**Шаблон установщика** - это папка с информацией о конфигурации установщика состоящая из config и packages.
Информация о конфигурации должна быть в папке config. Для получения информации о конфигурации см. [документация qt](https://doc.qt.io/qtinstallerframework/ifw-globalconfig.html).

В папке packages каждый пакет должен находиться в отдельной папке и содержать метаинформацию.
Если у вас есть только 1 пакет, то CQtDeployer развернет все развернутые цели в одном пакете.
Если их несколько, то имена пакетов должны совпадать с именами развернутых целей.
В этом случае CQtDeployer будет развертывать каждую цель в отдельном пакете.
Цели, имена которых не совпадают с именами пакетов, будут развернуты в первом пакете.
Пакеты отсортированы по имени.

Для получения дополнительной информации о пакетах см. [Qt documentantion](https://doc.qt.io/qtinstallerframework/ifw-component-description.html).


