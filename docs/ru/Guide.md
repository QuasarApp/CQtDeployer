# Как развернуть приложение C ++

В этой статье я описываю процессы развертывания приложения c++/qt с помощью cqtdeployer.

## Рекомендация
Если вам нужна максимальная стабильность вашего дистрибутива и вы хотите запускать свое приложение в разных дистрибутивах с разными версиями, вам необходимо использовать Qt из официального [build](https://www.qt.io/download-open-source?hsCtaTracking=9f6a2170-a938-42df-a8e2-a9f0b1d6cdce%7C6cb0de4f-9bb5-4778-ab02-bfb62735f3e5).

## Кейс
В этом случае мы развертываем приложение Qt GUI для Ubuntu 16.04, Ubuntu 18.04 и Ubuntu 20.04++.

**Если вы хотите поддерживать все дистрибутивы, начиная с Ubuntu 16.04, вам необходимо собрать и развернуть приложение в более старом дистрибутиве (в моем случае это 16.04)**

## Установить CQtDeployer
Если вы хотите использовать системный qt из своей ОС, установите [классическую](https://github.com/QuasarApp/CQtDeployer/releases/download/1.4.7/LinuxInstaller.run) версию cqtdeployer.

``` bash
wget https://github.com/QuasarApp/CQtDeployer/releases/download/1.4.7/LinuxInstaller.run
```

```
chmod + x LinuxInstaller.run
./LinuxInstaller.run
```
![Настройка CQtDeployer](https://user-images.githubusercontent.com/12465465/100077575-9280a600-2e53-11eb-8d9d-d184341cfa7c.png)

Выберите последнюю стабильную версию (1.4)

![Настройка CQtDeployer](https://user-images.githubusercontent.com/12465465/100077707-bb08a000-2e53-11eb-9838-6f896af33105.png)

Выполнено.

Если вы используете официальную сборку qt, вы можете использовать snap-версию cqtdeployer.

```
sudo snap установить cqtdeployer
```

После установки добавьте необходимые разрешения.

```
sudo snap connect cqtdeployer:process-control
sudo snap connect cqtdeployer:removable-media
sudo snap connect cqtdeployer:system-backup
```

Выполнено!!!

## Развертывание приложения Qt GUI

### Разверните исполняемый файл.

* Для систем Linux:
```bash
cqtdeployer -bin myexecutable
```

* Для систем Windows:
``` bash
cqtdeployer -bin myexecutable.exe -qmake C: /Qt/5.15.0/min_gw/bin/qmake.exe
```

* Для кроссплатформенной сборки (Linux -> Windows)
``` bash
cqtdeployer -bin myexecutable.exe -qmake ~/crossbuildet-Qt/5.15.0/min_gw/ bin / qmake
```

## Дополнительные параметры

Если вы используете дополнительные библиотеки, просто добавьте путь для cqtdeployer к используемым библиотекам.
``` bash
cqtdeployer -bin myexecutable -libDir/PathToMyExtraLibs
```

Если вы хотите найти библиотеки рекурсивно из пути libDir, просто добавьте параметр рекурсивной глубины.
``` bash
cqtdeployer -bin myexecutable -libDir/PathToMyExtraLibs -recursiveDepth 5
```

Если ваше приложение использует qml, просто добавьте опцию qmlDir
``` bash
cqtdeployer -bin myexecutable -libDir/PathToMyExtraLibs -recursiveDepth 5 -qmlDir /path/to/my/qml/sources
```

Если вы хотите создать простой установщик для своего приложения, просто добавьте опцию gif.
```
cqtdeployer -bin myexecutable -libDir/PathToMyExtraLibs -recursiveDepth 5 -qmlDir /path/to/my/qml/sources qif
```

Если cqtdeployer не нашел qmake, добавьте параметр -qmake в команду вызова.

```
cqtdeployer -bin myexecutable -libDir / PathToMyExtraLibs -recursiveDepth 5 -qmlDir /path/to/my/qml/sources qif -qmake/path/to/my/qmake

```
