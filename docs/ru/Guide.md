### Введение
В данной статье мы рассмотрим, как правильно собрать все зависимости qt для вашего приложения, которое было собрано динамически. 

### Для начало немного теории

### Зачем это нужно?

Существует несколько способов сборки приложений, основные из них это:
 * Статическая сборка 
Статическая сборка подразумевает создание бинарника, в котором будут слинковоны все необходимые ему компоненты. Другими словами все что нужно для его работы будет лежать в нем. Такой подход удобен для маленьких консольных приложений, у которых мало зависимостей, в противном случае размер конечного бинарника будет крайне велик.

* Динамическая сборка 
Отличается от статической тем, что в бинарнике будут лежать только исходные тексты вашего приложения (размер бинарника будет минимальный), но при выполнении такого приложения ему понадобятся сторонние библиотеки, которые использовались при его написании. 

### Теперь немного описания

CQtDeployer — это простая утилита для извлечения всех зависимых библиотек исполняемых файлов и создания сценария запуска для вашего приложения.

### Давайте рассмотрим пример

Для примера я написал простое qt приложение с использованием qml — MyApp.
### MyApp (main.cpp)
``` C++
#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}

``` 
### MyApp (main.qml)
``` qml
import QtQuick 2.9
import QtQuick.Controls 2.2

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Scroll")

    ScrollView {
        anchors.fill: parent

        ListView {
            width: parent.width
            model: 20
            delegate: ItemDelegate {
                text: "Item " + (index + 1)
                width: parent.width
            }
        }
    }
}

```

MyApp слинкована динамически, то есть для работы ему требуются библиотеки qt.
Если попробовать запустить приложение, сразу после сборки мы получим ошибку:
``` bash
~/MyApp$ ./MyApp 
./MyApp: /usr/lib/x86_64-linux-gnu/libQt5Qml.so.5: version `Qt_5' not found (required by ./MyApp)
./MyApp: /usr/lib/x86_64-linux-gnu/libQt5Gui.so.5: version `Qt_5' not found (required by ./MyApp)
./MyApp: /usr/lib/x86_64-linux-gnu/libQt5Core.so.5: version `Qt_5.11' not found (required by ./MyApp)
./MyApp: /usr/lib/x86_64-linux-gnu/libQt5Core.so.5: version `Qt_5' not found (required by ./MyApp)
```

Из сходных текстов мы видим, что приложение зависит от GUI библиотек qt и библиотек qml. Поиск и сборка всех ресурсов (библиотек и плагинов) займет очень много времени.
Чтобы сэкономить время и силы, мы воспользуемся утилитой CQtDeployer (скачать можно [здесь](https://github.com/QuasarApp/CQtDeployer/releases))
или установить в Snap Store

[![Загрузите из Snap Store](https://snapcraft.io/static/images/badges/ru/snap-store-black.svg)](https://snapcraft.io/cqtdeployer)
``` bash
cqtdeployer -bin myApp -qmake /media/D/Qt/5.12.3/gcc_64/bin/qmake -qmlDir ./
```

После выполнения данной команды, вы получите полностью готовое для работы приложение с готовым лаунчером, который настроит все необходимое окружение для работы вашего приложения на всех машинах под управлением Linux.

Давайте подробнее рассмотрим некоторые из параметров CQtDeployer:


| Опция                      | Описание
|-----------------------------|-----------------------------------------------------------|
|   help / h                  | Показывает справку                                          |
|   always-overwrite          | Копирует файлы с заменой уже существующих                 |
|   -bin    [list, params]    | Развертываемый файл или папка. пример -bin ~/my/project/bin/,~/my/project/bin.exe|
|   -binDir [params]          | Папка с развертываемыми файлами (с рекурсивным поиском). ВНИМАНИЕ! Этот флаг поддерживает только файлы 'so', 'dll' и 'exe'. Если вы хотите развернуть бинарный файл Linux, используйте флаг '-bin'  |
|   -qmlDir [params]          | Папка qml. пример -qmlDir ~/my/project/qml                |
|   deploySystem              | Копирует все библиотеки                                 |
|   -qmake  [params]          | Путь к qmake. пример                                      |
|                             | -qmake ~/Qt/5.11.1/gcc_64/bin/qmake                       |
|   -ignore [list,params]     | Список библиотек для игнорирования                        |
|                             | Пример -ignore libicudata.so.56,libicudata2.so.56         |
|   -ignoreEnv [list,params]  | Список путей для игнорирования.                          |
|                             | Пример -ignoreEnv /bad/dir,/my/bad/Dir                                |
|   clear                     | Удаляет все старые файлы (с прошлого запуска)              |
|                             | пример -runScript myApp.sh                                |
|  allQmlDependes             | Извлекает все библиотеки qml.          |
|                             | (не рекомендуется, так как занимает много памяти)         |
|  -libDir [list,params]      | Устанавливает дополнительные пути к библиотекам               |
|                             | Пример -libDir ~/myLib,~/newLibs                          |
|  -extraPlugin [list,params] | Устанавливает дополнительный путь для extraPlugin приложения |
|  -recursiveDepth [params]   | Устанавливает глубину поиска библиотек (по умолчанию 0)       |
|  -targetDir [params]        | Устанавливает целевой каталог (по умолчанию это путь к первому развертываемому файлу)|
|  noStrip                    | Пропускает шаг strip                                      |
|  noTranslations             | Пропускает файлы переводов                                |
|  qmlExtern                  | Использует внешний сканер qml (qmlimportscaner)         |
|                             | не работает без qmake и в snap                            |
|  -verbose [0-3]             | Показывает дебаг лога                                          |

### Итог 
После выполнения cqtdeployer у вас появится папка Distro c уже готовым приложением со всеми его зависимостями  содержимое этой папки должно выглядеть примерно следующим образом:
``` bash

drwxr-xr-x 7 andrei andrei 4096 May 24 12:22 ./
drwxrwxr-x 3 andrei andrei 4096 May 24 12:22 ../
drwxr-xr-x 2 andrei andrei 4096 May 24 12:22 bin/
drwxr-xr-x 2 andrei andrei 4096 May 24 12:22 lib/
-rwx---rwx 1 andrei andrei  433 May 24 12:22 myApp.sh*
drwxr-xr-x 6 andrei andrei 4096 May 24 12:22 plugins/
drwxr-xr-x 5 andrei andrei 4096 May 24 12:22 qml/
drwxr-xr-x 2 andrei andrei 4096 May 24 12:22 translations/


```
![cqtdeployer result](https://user-images.githubusercontent.com/12465465/58318590-bdcea300-7e20-11e9-8b45-37a13aeef222.png)
*  myApp.sh  — скрипт запуска вашего приложения
*  bin — папка с вашим бинарником
*  lib — папка со всеми необходимыми зависимости вашего приложения
*  plugins — qt плагины, необходимые для работы приложения
*  qml — зависимости qml.
*  translations — стандартные переводы qt.

Таким образом можно подготовить ваше приложения для упаковки в deb или snap пакет, после чего можно приступить к его распространению. Учитывайте, что после выполнения cqtdeployer, ваше приложение необходимо запускать с помощью sh скрипта, который настроит для вашего приложения необходимое окружение.
