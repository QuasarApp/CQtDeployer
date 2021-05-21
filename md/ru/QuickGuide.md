

Как развернуть приложение с помощью CQtDeployer

1. Прежде всего вам нужно установить CQtDeployer на вашу платформу

    * Snap (более 14 дистрибутивов Linux):
    ```
    sudo snap install cqtdeployer
    ```
    или 

    Установите из магазина snap и включите все необходимые приложению разрешения.

    [![Download from Snap Store](https://snapcraft.io/static/images/badges/ru/snap-store-black.svg)](https://snapcraft.io/cqtdeployer)

    * Другие Linux дистрибутивы [LinuxInstaller.run][1].
    ```
    chmod +x LinuxInstaller.run
    ./LinuxInstaller.run
    ```

    * Windows [installer][1]

2. Скомпилируйте ваш проект в режиме релиза.
3. Разверните ваш исполняемый файл:

    * Для Linux систем:
    ``` bash
    cqtdeployer -bin myexecutable
    ```

    * Для Windows систем:
    ``` bash
    cqtdeployer -bin myexecutable.exe -qmake C:/Qt/5.15.0/min_gw/bin/qmake.exe
    ```

    * Для кросплатформенной сборки (Linux -> Windows)
    ``` bash
    cqtdeployer -bin myexecutable.exe -qmake ~/crossbuildet-Qt/5.15.0/min_gw/bin/qmake
    ```

## Дополнительные опции

Если вы используете дополнительные библиотеки, просто добавьте путь к cqtdeployer для используемых библиотек.
``` bash
cqtdeployer -bin myexecutable -libDir /PathToMyExtraLibs 
```

Если вы хотите найти библиотеки рекурсивно по пути libDir, просто добавьте рекурсивный параметр Depth.
``` bash
cqtdeployer -bin myexecutable -libDir /PathToMyExtraLibs -recursiveDepth 5
```

Если ваше приложение использует qml, просто добавьте опцию qmlDir
``` bash
cqtdeployer -bin myexecutable -libDir /PathToMyExtraLibs -recursiveDepth 5 -qmlDir /path/to/my/qml/sources
```

Если вы хотите создать простой установщик для вашего приложения, просто добавьте опцию qif.
```
cqtdeployer -bin myexecutable -libDir /PathToMyExtraLibs -recursiveDepth 5 -qmlDir /path/to/my/qml/sources qif
```

Если вы используете системные библиотеки из системного PATH, просто добавьте опцию deploySystem.

#### Предупреждение: версия Snap не имеет прав доступа к системным файлам. Для развертывания системных библиотек используйте версию cqtdeployer из [установщика][1]

```
cqtdeployer -bin myexecutable -libDir /PathToMyExtraLibs -recursiveDepth 5 -qmlDir /path/to/my/qml/sources qif deploySystem
```


### Если у вас есть какие-либо проблемы с использованием cqtdeployer, тогда задавайте вопросы на официальной  github [странице][2].

### Дополнительные параметры CQtDeployer см. В официальной [документации][3].

[1]: https://github.com/QuasarApp/CQtDeployer/releases
[2]: https://github.com/QuasarApp/CQtDeployer/issues
[3]: https://github.com/QuasarApp/CQtDeployer/wiki/Options

