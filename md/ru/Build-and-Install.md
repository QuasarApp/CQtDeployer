# Сборка и установка

## Build для Linux

- установите qt и qt QtInstallFrameWork из [Установщик qt](https://www.qt.io/download-qt-installer?hsCtaTracking=9f6a2170-a938-42df-a8e2-a9f0b1d6cdce%7C6cb0de4f-9bb5-4778-ab02-bfb62735f3e5)
- git clone https://github.com/QuasarApp/CQtDeployer.git
- cd CQtDeployer
- git submodule update --init --recursive
- qmake -r
     - Здесь нужно обязательно вызвать тот qmake, который был загружен из 1го пункта.
     - Пример: ~/Qt/5.15.0/gcc_64/bin/qmake -r
- make -j$(nproc)
- make deploy
     - эта команда требует установленный [cqtdeployer](https://github.com/QuasarApp/CQtDeployer/releases)

## Build для Windows (CMD)

- установите qt и qt QtInstallFrameWork из [Установщик qt](https://www.qt.io/download-qt-installer?hsCtaTracking=9f6a2170-a938-42df-a8e2-a9f0b1d6cdce%7C6cb0de4f-9bb5-4778-ab02-bfb62735f3e5)
- git clone https://github.com/QuasarApp/CQtDeployer.git
- cd CQtDeployer
- git submodule update --init --recursive
- SET PATH=C:/Qt/Tools/mingw810_64/bin;%PATH%
    - Здесь важно настроить окружение qt.
- qmake.exe -r
    - Здесь нужно обязательно вызвать тот qmake, который был загружен из 1го пункта.
    - Пример: C:/Qt/5.15.0/mingw81_64/bin/qmake.exe -r
- migw32-make -j8
- migw32-make deploy
    - эта команда требует установленный [cqtdeployer](https://github.com/QuasarApp/CQtDeployer/releases)
- ./Distro/CQtDeployerInstaller.exe

---

# Установка без сборки

## Snap

[![Загрузите из Snap Store](https://snapcraft.io/static/images/badges/ru/snap-store-black.svg)](https://snapcraft.io/cqtdeployer)

#### Особенности snap версии

Если вы используете snap версию приложения. Включите все разрешения для cqtdeployer.
это можно сделать в snap-store 
Или начиная с ubuntu 20.04 в менеджере настроек приложений ubuntu. 

Если у вас нет графического интерфейса, вы можете включить все разрешения, используя следующие команды:

``` bash
sudo snap connect cqtdeployer:process-control
sudo snap connect cqtdeployer:removable-media
sudo snap connect cqtdeployer:system-backup
```

## Установщик

Вы можете загрузить последнюю версию приложения [здесь](https://github.com/QuasarApp/CQtDeployer/releases).
