# Создания установщика прямо из qt creator. 
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

# На живом примере animation из [Примеров Qt](https://doc.qt.io/qt-5/qtquick-animation-example.html)


1. Собираем проект в релизную сборку 
    1. Выполняем qmake с опцие -r для релизной сборки.

        ```bash
        andrei@X570-GAMING-X:~/Qt/Examples/Qt-5.15.2/quick/animation$ ~/Qt/5.15.2/gcc_64/bin/qmake -r .
        ```

    2. Вызываем ваш make generator

        * Для Linux:
            ```bash
            make -j${nproc}
            ```
        * Для Windows 
            ```bash
                Qt/Tools/QtCreator/bin/jom.exe
            ```

        ``` bash
        andrei@X570-GAMING-X:~/Qt/Examples/Qt-5.15.2/quick/animation$ make -j${nproc}
        g++ -c -pipe -O2 -Wall -Wextra -D_REENTRANT -fPIC -DQT_NO_DEBUG -DQT_QUICK_LIB -DQT_GUI_LIB -DQT_QMLMODELS_LIB -DQT_QML_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB -I. -I../../../../5.15.2/gcc_64/include -I../../../../5.15.2/gcc_64/include/QtQuick -I../../../../5.15.2/gcc_64/include/QtGui -I../../../../5.15.2/gcc_64/include/QtQmlModels -I../../../../5.15.2/gcc_64/include/QtQml -I../../../../5.15.2/gcc_64/include/QtNetwork -I../../../../5.15.2/gcc_64/include/QtCore -I. -I/usr/include/libdrm -I../../../../5.15.2/gcc_64/mkspecs/linux-g++ -o main.o main.cpp
        /home/andrei/Qt/5.15.2/gcc_64/bin/rcc -name shared ../shared/shared.qrc -o qrc_shared.cpp
        /home/andrei/Qt/5.15.2/gcc_64/bin/rcc -name animation animation.qrc -o qrc_animation.cpp
        g++ -c -pipe -O2 -Wall -Wextra -D_REENTRANT -fPIC -DQT_NO_DEBUG -DQT_QUICK_LIB -DQT_GUI_LIB -DQT_QMLMODELS_LIB -DQT_QML_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB -I. -I../../../../5.15.2/gcc_64/include -I../../../../5.15.2/gcc_64/include/QtQuick -I../../../../5.15.2/gcc_64/include/QtGui -I../../../../5.15.2/gcc_64/include/QtQmlModels -I../../../../5.15.2/gcc_64/include/QtQml -I../../../../5.15.2/gcc_64/include/QtNetwork -I../../../../5.15.2/gcc_64/include/QtCore -I. -I/usr/include/libdrm -I../../../../5.15.2/gcc_64/mkspecs/linux-g++ -o qrc_animation.o qrc_animation.cpp
        g++ -c -pipe -O2 -Wall -Wextra -D_REENTRANT -fPIC -DQT_NO_DEBUG -DQT_QUICK_LIB -DQT_GUI_LIB -DQT_QMLMODELS_LIB -DQT_QML_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB -I. -I../../../../5.15.2/gcc_64/include -I../../../../5.15.2/gcc_64/include/QtQuick -I../../../../5.15.2/gcc_64/include/QtGui -I../../../../5.15.2/gcc_64/include/QtQmlModels -I../../../../5.15.2/gcc_64/include/QtQml -I../../../../5.15.2/gcc_64/include/QtNetwork -I../../../../5.15.2/gcc_64/include/QtCore -I. -I/usr/include/libdrm -I../../../../5.15.2/gcc_64/mkspecs/linux-g++ -o qrc_shared.o qrc_shared.cpp
        g++ -Wl,-O1 -Wl,-rpath,/home/andrei/Qt/5.15.2/gcc_64/lib -o animation main.o qrc_shared.o qrc_animation.o   /home/andrei/Qt/5.15.2/gcc_64/lib/libQt5Quick.so /home/andrei/Qt/5.15.2/gcc_64/lib/libQt5Gui.so /home/andrei/Qt/5.15.2/gcc_64/lib/libQt5QmlModels.so /home/andrei/Qt/5.15.2/gcc_64/lib/libQt5Qml.so /home/andrei/Qt/5.15.2/gcc_64/lib/libQt5Network.so /home/andrei/Qt/5.15.2/gcc_64/lib/libQt5Core.so -lGL -lpthread   
        ```
3. Находим наш получившийся исполняемый файл 
    Если вы собирали проект с помощью qtCreator ваш исполняемый файл будет находиться на уровень выше в папке **build-Animations-Desktop_Qt_5_15_2_GCC_64bit-Release**
    В моем случае исполняемый файл лежит в корневом катологе проекта.
    ``` bash
    andrei@X570-GAMING-X:~/Qt/Examples/Qt-5.15.2/quick/animation$ tree
    .
    ├── animation # Вот он.
    ├── animation.pro
    ├── animation.qml
    ├── animation.qmlproject
    ├── animation.qrc
    ├── basics
    │   ├── animators.qml
    │   ├── color-animation.qml
    │   ├── images
    │   │   ├── face-smile.png
    │   │   ├── moon.png
    │   │   ├── shadow.png
    │   │   ├── star.png
    │   │   └── sun.png
    │   └── property-animation.qml
    ├── behaviors
    │   ├── behavior-example.qml
    │   ├── FocusRect.qml
    │   ├── SideRect.qml
    │   ├── tvtennis.qml
    │   └── wigglytext.qml
    ├── doc
    │   ├── images
    │   │   └── qml-animations-example.png
    │   └── src
    │       └── animation.qdoc
    ├── easing
    │   └── easing.qml
    ├── main.cpp
    ├── main.o
    ├── Makefile
    ├── pathanimation
    │   └── pathanimation.qml
    ├── pathinterpolator
    │   └── pathinterpolator.qml
    ├── qrc_animation.cpp
    ├── qrc_animation.o
    ├── qrc_shared.cpp
    ├── qrc_shared.o
    └── states
        ├── qt-logo.png
        ├── states.qml
        └── transitions.qml

    10 directories, 33 files
    ```

4. Вызываем cqtdeployer для формирования Базового Дистрибутива

    Для Linux:
    ``` bash 
    cqtdeployer -bin animation -qmlDir . qif -qmake ~/Qt/5.15.2/gcc_64/bin/qmake
    ```
    Для Windows:
    ``` bash 
    cqtdeployer -bin animation -qmlDir . qif -qmake ~/Qt/5.15.2/mingw_810_64/bin/qmake.exe
    ```

    ``` bash
    andrei@X570-GAMING-X:~/Qt/Examples/Qt-5.15.2/quick/animation$ cqtdeployer -bin animation -qmlDir . qif -qmake ~/Qt/5.15.2/gcc_64/bin/qmake
    Info: Deploy ...
    Info: flag targetDir not  used. use default target dir :/home/andrei/Qt/Examples/Qt-5.15.2/quick/animation/DistributionKit
    Info: target deploy started!!
    Info: copy :/home/andrei/Qt/Examples/Qt-5.15.2/quick/animation/animation
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/qml/QtQml/libqmlplugin.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/qml/QtQml/Models.2/libmodelsplugin.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/qml/QtQml/Models.2/plugins.qmltypes
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/qml/QtQml/Models.2/qmldir
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/qml/QtQml/plugins.qmltypes
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/qml/QtQml/qmldir
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/qml/QtQml/RemoteObjects/libqtqmlremoteobjects.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/qml/QtQml/RemoteObjects/plugins.qmltypes
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/qml/QtQml/RemoteObjects/qmldir
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/qml/QtQml/StateMachine/libqtqmlstatemachine.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/qml/QtQml/StateMachine/plugins.qmltypes
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/qml/QtQml/StateMachine/qmldir
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/qml/QtQml/WorkerScript.2/libworkerscriptplugin.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/qml/QtQml/WorkerScript.2/plugins.qmltypes
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/qml/QtQml/WorkerScript.2/qmldir
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/qml/QtQuick/Particles.2/libparticlesplugin.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/qml/QtQuick/Particles.2/plugins.qmltypes
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/qml/QtQuick/Particles.2/qmldir
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/qml/QtQuick.2/libqtquick2plugin.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/qml/QtQuick.2/plugins.qmltypes
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/qml/QtQuick.2/qmldir
    Info: platform : libqvnc is disabled
    Info: platform : libqvnc is disabled
    Info: platform : libqwebgl is disabled
    Info: platform : libqwebgl is disabled
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/bearer/libqconnmanbearer.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/bearer/libqgenericbearer.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/bearer/libqnmbearer.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/iconengines/libqsvgicon.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/imageformats/libqgif.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/imageformats/libqicns.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/imageformats/libqico.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/imageformats/libqjpeg.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/imageformats/libqsvg.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/imageformats/libqtga.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/imageformats/libqtiff.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/imageformats/libqwbmp.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/imageformats/libqwebp.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/platforminputcontexts/libcomposeplatforminputcontextplugin.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/platforminputcontexts/libibusplatforminputcontextplugin.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/platforms/libqeglfs.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/platforms/libqlinuxfb.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/platforms/libqminimal.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/platforms/libqminimalegl.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/platforms/libqoffscreen.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/platforms/libqwayland-egl.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/platforms/libqwayland-generic.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/platforms/libqwayland-xcomposite-egl.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/platforms/libqwayland-xcomposite-glx.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/platforms/libqxcb.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/platformthemes/libqgtk3.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/platformthemes/libqxdgdesktopportal.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/wayland-decoration-client/libbradient.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/wayland-graphics-integration-client/libdmabuf-server.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/wayland-graphics-integration-client/libdrm-egl-server.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/wayland-graphics-integration-client/libqt-plugin-wayland-egl.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/wayland-graphics-integration-client/libshm-emulation-server.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/wayland-graphics-integration-client/libvulkan-server.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/wayland-graphics-integration-client/libxcomposite-egl.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/wayland-graphics-integration-client/libxcomposite-glx.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/wayland-graphics-integration-server/libqt-wayland-compositor-dmabuf-server-buffer.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/wayland-graphics-integration-server/libqt-wayland-compositor-drm-egl-server-buffer.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/wayland-graphics-integration-server/libqt-wayland-compositor-linux-dmabuf-unstable-v1.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/wayland-graphics-integration-server/libqt-wayland-compositor-shm-emulation-server.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/wayland-graphics-integration-server/libqt-wayland-compositor-vulkan-server.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/wayland-graphics-integration-server/libqt-wayland-compositor-wayland-egl.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/wayland-graphics-integration-server/libqt-wayland-compositor-wayland-eglstream-controller.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/wayland-graphics-integration-server/libqt-wayland-compositor-xcomposite-egl.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/wayland-graphics-integration-server/libqt-wayland-compositor-xcomposite-glx.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/wayland-shell-integration/libfullscreen-shell-v1.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/wayland-shell-integration/libivi-shell.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/wayland-shell-integration/libwl-shell.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/wayland-shell-integration/libxdg-shell-v5.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/wayland-shell-integration/libxdg-shell-v6.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/wayland-shell-integration/libxdg-shell.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/xcbglintegrations/libqxcb-egl-integration.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/plugins/xcbglintegrations/libqxcb-glx-integration.so
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/lib/libQt5Core.so.5
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/lib/libQt5Network.so.5
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/lib/libQt5EglFSDeviceIntegration.so.5
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/lib/libQt5XcbQpa.so.5
    Info: copy :/home/andrei/CQtDeployer/1.5/lib/libicui18n.so.56
    Info: copy :/usr/lib/x86_64-linux-gnu/libxcb-xinerama.so.0
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/lib/libQt5RemoteObjects.so.5
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/lib/libQt5QuickParticles.so.5
    Info: copy :/home/andrei/CQtDeployer/1.5/lib/libicudata.so.56
    Info: copy :/home/andrei/CQtDeployer/1.5/lib/libicuuc.so.56
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/lib/libQt5QmlWorkerScript.so.5
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/lib/libQt5Widgets.so.5
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/lib/libQt5WaylandClient.so.5
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/lib/libQt5DBus.so.5
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/lib/libQt5Qml.so.5
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/lib/libQt5Quick.so.5
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/lib/libQt5QmlModels.so.5
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/lib/libQt5Gui.so.5
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/lib/libQt5Svg.so.5
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/lib/libQt5WaylandCompositor.so.5
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtbase_ar.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtbase_bg.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtbase_ca.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtbase_cs.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtbase_da.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtbase_de.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtbase_en.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtbase_es.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtbase_fi.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtbase_fr.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtbase_gd.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtbase_he.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtbase_hu.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtbase_it.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtbase_ja.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtbase_ko.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtbase_lv.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtbase_pl.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtbase_ru.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtbase_sk.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtbase_tr.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtbase_uk.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtbase_zh_TW.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtdeclarative_bg.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtdeclarative_da.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtdeclarative_de.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtdeclarative_en.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtdeclarative_es.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtdeclarative_fi.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtdeclarative_fr.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtdeclarative_hu.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtdeclarative_ja.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtdeclarative_ko.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtdeclarative_lv.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtdeclarative_pl.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtdeclarative_ru.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtdeclarative_sk.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtdeclarative_tr.qm
    Info: copy :/home/andrei/Qt/5.15.2/gcc_64/translations/qtdeclarative_uk.qm
    Info: try deploy msvc
    Info: deploy done!
    Info: copy ::/shared/Distributions/Templates/Icon.png
    Error:  not exits
    Info: [0] Parsed arguments, ok.

    Info: [2] Collecting information about available packages...

    Info: [4] Found subdirectory "Application"

    Info: [4] - it provides the package "Application"  -  "1.0"

    Info: [4] Copying component data for "Application"

    Info: [5] Compressing data directory "bin"

    Info: [21] Compressing data directory "icons"

    Info: [31] Compressing data directory "lib"

    Info: [11111] Compressing data directory "plugins"

    Info: [11740] Compressing data directory "qml"

    Info: [11786] Compressing data directory "translations"

    Info: [12321] Compressing files found in data directory: ("/home/andrei/Qt/Examples/Qt-5.15.2/quick/animation/DistributionKit/defaultQIFWTemplate/packages/Application/data/animation.sh")

    Info: [12321] Hash is stored in "/tmp/binarycreator-yOPMPa/Application/1.0bin.7z.sha1"
    [12321] Creating hash of archive "/tmp/binarycreator-yOPMPa/Application/1.0bin.7z"

    Info: [12321] Generated sha1 hash: "0af6cb9dc074407a8857403f07d13aa7d1b3c9cb"

    Info: [12322] Hash is stored in "/tmp/binarycreator-yOPMPa/Application/1.0icons.7z.sha1"
    [12322] Creating hash of archive "/tmp/binarycreator-yOPMPa/Application/1.0icons.7z"

    Info: [12322] Generated sha1 hash: "aa938cb332bfb427a6d0280c07c204c1bb48da22"

    Info: [12322] Hash is stored in "/tmp/binarycreator-yOPMPa/Application/1.0lib.7z.sha1"
    [12322] Creating hash of archive "/tmp/binarycreator-yOPMPa/Application/1.0lib.7z"

    Info: [12347] Generated sha1 hash: "7c3a43111e694bc82a0ab0d19a6e4d0025f17313"

    Info: [12347] Hash is stored in "/tmp/binarycreator-yOPMPa/Application/1.0plugins.7z.sha1"
    [12347] Creating hash of archive "/tmp/binarycreator-yOPMPa/Application/1.0plugins.7z"

    Info: [12349] Generated sha1 hash: "4ed23c36835a9b62701478ab6897737dfe9dfc9a"

    Info: [12349] Hash is stored in "/tmp/binarycreator-yOPMPa/Application/1.0qml.7z.sha1"

    Info: [12349] Creating hash of archive "/tmp/binarycreator-yOPMPa/Application/1.0qml.7z"

    Info: [12349] Generated sha1 hash: "bd44e15d2c26c8f6cb50760032eadf0241eedf43"

    Info: [12349] Hash is stored in "/tmp/binarycreator-yOPMPa/Application/1.0translations.7z.sha1"
    [12349] Creating hash of archive "/tmp/binarycreator-yOPMPa/Application/1.0translations.7z"

    Info: [12350] Generated sha1 hash: "030e0ede0c0e058a5d5a3b6c43c6a4767119d3f1"

    Info: [12350] Hash is stored in "/tmp/binarycreator-yOPMPa/Application/1.0content.7z.sha1"

    Info: [12350] Creating hash of archive "/tmp/binarycreator-yOPMPa/Application/1.0content.7z"

    Info: [12350] Generated sha1 hash: "acd2cd63e77971acedbcda2a65f93b2cc036025f"

    Info: [12350] Copy meta data for package "Application" using "/home/andrei/Qt/Examples/Qt-5.15.2/quick/animation/DistributionKit/defaultQIFWTemplate/packages/Application/meta/package.xml"

    Info: [12350] calculate size of directory "/tmp/binarycreator-yOPMPa/Application/data"

    Info: [12359] Copying associated "script" file "/home/andrei/Qt/Examples/Qt-5.15.2/quick/animation/DistributionKit/defaultQIFWTemplate/packages/Application/meta/installscript.qs"

    Info: [12360] done.

    Info: [12360] Begin to copy configuration file and data.

    Info: [12360] Copying associated "configuration" file "/home/andrei/Qt/Examples/Qt-5.15.2/quick/animation/DistributionKit/defaultQIFWTemplate/config/config.xml"

    Info: [12360] done.

    Info: [12360] Read dom element: <Name>animation</Name>.

    Info: [12360] Read dom element: <Version>1.0</Version>.

    Info: [12360] Read dom element: <Title>animation</Title>.

    Info: [12360] Read dom element: <Publisher>Company</Publisher>.
    [12360] Read dom element: <StartMenuDir>animation</StartMenuDir>.
    [12360] Read dom element: <TargetDir>@HomeDir@/animation</TargetDir>.

    Info: [12360] Read dom element: <InstallActionColumnVisible>true</InstallActionColumnVisible>.
    [12360] Read dom element: <RemoveTargetDir>true</RemoveTargetDir>.

    Info: [12360] Read dom element: <ControlScript>controlScript.qs</ControlScript>.

    Info: [12360] Copying associated "ControlScript" file "/home/andrei/Qt/Examples/Qt-5.15.2/quick/animation/DistributionKit/defaultQIFWTemplate/config/controlScript.qs"

    Info: [12360] done.

    Info: [12360] Read dom element: <MaintenanceToolName>animationTool</MaintenanceToolName>.
    [12360] Read dom element: <ProductUrl></ProductUrl>.

    Info: [12360] done.


    Info: [12362] Creating the binary

    Info: [12407] Creating resource archive for "Application"

    Info: [12407] Appending "/tmp/binarycreator-yOPMPa/Application/1.0bin.7z" ("61.58 KB")
    [12407] Appending "/tmp/binarycreator-yOPMPa/Application/1.0bin.7z.sha1" ("40.00 bytes")
    [12407] Appending "/tmp/binarycreator-yOPMPa/Application/1.0icons.7z" ("108.00 KB")
    [12407] Appending "/tmp/binarycreator-yOPMPa/Application/1.0icons.7z.sha1" ("40.00 bytes")
    [12407] Appending "/tmp/binarycreator-yOPMPa/Application/1.0lib.7z" ("18.08 MB")

    Info: [12407] Appending "/tmp/binarycreator-yOPMPa/Application/1.0lib.7z.sha1" ("40.00 bytes")
    [12407] Appending "/tmp/binarycreator-yOPMPa/Application/1.0plugins.7z" ("1.08 MB")
    [12407] Appending "/tmp/binarycreator-yOPMPa/Application/1.0plugins.7z.sha1" ("40.00 bytes")
    [12407] Appending "/tmp/binarycreator-yOPMPa/Application/1.0qml.7z" ("53.29 KB")
    [12407] Appending "/tmp/binarycreator-yOPMPa/Application/1.0qml.7z.sha1" ("40.00 bytes")

    Info: [12407] Appending "/tmp/binarycreator-yOPMPa/Application/1.0translations.7z" ("595.71 KB")
    [12407] Appending "/tmp/binarycreator-yOPMPa/Application/1.0translations.7z.sha1" ("40.00 bytes")
    [12407] Appending "/tmp/binarycreator-yOPMPa/Application/1.0content.7z" ("439.00 bytes")

    Info: [12407] Appending "/tmp/binarycreator-yOPMPa/Application/1.0content.7z.sha1" ("40.00 bytes")

    Info: [12449] Cleaning up...
    ```

5. Готово Дистрибутив был сохранен в папку **DistributionKit** 
```bash
andrei@X570-GAMING-X:~/Qt/Examples/Qt-5.15.2/quick/animation$ ./DistributionKit/Installeranimation.run 
```
![Снимок экрана от 2021-02-22 11-25-34](https://user-images.githubusercontent.com/12465465/108681955-c629a580-7500-11eb-8087-cedfa8134aae.png)

После установки буду созданы все необходимые ярлыки.

![Снимок экрана от 2021-02-22 11-27-11](https://user-images.githubusercontent.com/12465465/108682148-012bd900-7501-11eb-9f0b-f46fd79aef25.png)

![Снимок экрана от 2021-02-22 11-29-23](https://user-images.githubusercontent.com/12465465/108682391-48b26500-7501-11eb-8d81-b430ddf97d46.png)


## Интеграция с QtCreator 

К сожалению, на данный момент нет интеграции с qtcreator.
Единственный способ - включить цель сборки для **qmake** или **cmake**

### qmake 

```qmake
QT_DIR= $$[QT_HOST_BINS]
win32:QMAKE_BIN= $$QT_DIR/qmake.exe
contains(QMAKE_HOST.os, Linux):{
    QMAKE_BIN= $$QT_DIR/qmake
}

deploy.commands= cqtdeployer -bin $$DESTDIR/$$TARGET -qmake $$QMAKE_BIN -libDir $$PWD -qmlDir $$PWD -recursiveDepth 5  qif

QMAKE_EXTRA_TARGETS += deploy

```

### cmake 
```cmake
    find_program(Q_MAKE_EXE qmake)

    find_program(CQT_EXE cqtdeployer)

    IF(NOT EXISTS ${CQT_EXE})
        message("the cqtdeployer not exits please install the cqtdeployer and run cmake again!")
        return()
    endif(NOT EXISTS ${CQT_EXE})

    ADD_CUSTOM_TARGET(
        deploy
        COMMAND cqtdeployer clear -bin ${CURENT_PROJECT} -qmake ${Q_MAKE_EXE} -libDir ${PROJECT_SOURCE_DIR} -qmlDir ${PROJECT_SOURCE_DIR} -recursiveDepth 5
        COMMENT "Deploy: cqtdeployer clear -bin ${CURENT_PROJECT} -qmake ${Q_MAKE_EXE} -libDir ${PROJECT_SOURCE_DIR} -qmlDir ${PROJECT_SOURCE_DIR} -recursiveDepth 5"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )
```

Параметры **libDir** используются для установки путей к зависимостям вашего приложения в примерах был выбран корень исходных кодов приложения.   

Параметры **qmlDir** используются для установки путей к файлам qml вашего приложения в примерах был выбран корень исходных кодов приложения.   

Параметры **recursiveDepth** используются для установки глубины поиска для libDir и qmlDir.   



## Пример интеграции 
1. Создаем тестовый проект
    ``` qmake
    QT += quick

    CONFIG += c++11

    # You can make your code fail to compile if it uses deprecated APIs.
    # In order to do so, uncomment the following line.
    #DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

    SOURCES += \
            main.cpp

    RESOURCES += qml.qrc

    # Additional import path used to resolve QML modules in Qt Creator's code model
    QML_IMPORT_PATH =

    # Additional import path used to resolve QML modules just for Qt Quick Designer
    QML_DESIGNER_IMPORT_PATH =

    # Default rules for deployment.
    qnx: target.path = /tmp/$${TARGET}/bin
    else: unix:!android: target.path = /opt/$${TARGET}/bin
    !isEmpty(target.path): INSTALLS += target


    QT_DIR= $$[QT_HOST_BINS]
    win32:QMAKE_BIN= $$QT_DIR/qmake.exe
    contains(QMAKE_HOST.os, Linux):{
        QMAKE_BIN= $$QT_DIR/qmake
    }

    DESTDIR=$$PWD/Build

    deploy.commands= cqtdeployer -bin $$DESTDIR/$$TARGET -qmake $$QMAKE_BIN -libDir $$PWD -qmlDir $$PWD -recursiveDepth 5  qif

    QMAKE_EXTRA_TARGETS += deploy

    ```

2. Открываем панель управления проектом 
    ![Снимок экрана от 2021-02-22 12-15-51](https://user-images.githubusercontent.com/12465465/108687472-d09b6d80-7507-11eb-9b56-8d09d42dac5e.png)

3. Выбираем релизную сборку.
    ![изображение](https://user-images.githubusercontent.com/12465465/108688147-a39b8a80-7508-11eb-81b9-9a24426a94a3.png)


3. Добавляем "Особый этап сборки"
    ![изображение](https://user-images.githubusercontent.com/12465465/108687640-02143900-7508-11eb-8d1c-532afd7f6ccb.png)

4. В поле "Make arguments" добавляем наш этап развертывания "Deploy"
    ![изображение](https://user-images.githubusercontent.com/12465465/108687800-35ef5e80-7508-11eb-8f69-aec090d4baa0.png)

5. Сохраняем
6. Запускаем сборку.

#### На выходе вы получите папку DistributionKit с установщиком вашего приложения.
