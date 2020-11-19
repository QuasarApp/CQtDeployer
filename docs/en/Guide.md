### Introduction
In this article we will look at how to properly collect all qt dependencies for your application, which was collected dynamically.

### For a start, a little theory.

### Why is this needed?

There are several ways to build applications, the main ones are:
* Static build
Static assembly involves the creation of a binary, which will be all the necessary links to it. In other words, all that is needed for his work will lie in it. This approach is suitable for small console applications that have few dependencies, otherwise the size of the final binary will be extremely large.

* Dynamic build
It differs from static in that only the source code of your application will be in the binary (the size of the binary will be minimal), but when you run such an application it will need third-party libraries that were used when writing it.

### Now a little description.

The CQtDeployer is application for extract all depends library of executable and create launch script for your application.

### Let's take an example.

For example, I wrote a simple qt application using qml - MyApp.
### MyApp (main.cpp)
```cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main (int argc, char * argv [])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app (argc, argv);

    QQmlApplicationEngine engine;
    engine.load (QUrl (QStringLiteral("qrc: /main.qml")));
    if (engine.rootObjects ().isEmpty )
        return -1;

    return app.exec ();
}

```
### MyApp (main.qml)
```qml
import QtQuick 2.9
import QtQuick.Controls 2.2

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr ("Scroll")

    ScrollView {
        anchors.fill: parent

        ListView {
            width: parent.width
            model: 20
            delegate: ItemDelegate {
                text: "Item" + (index + 1)
                width: parent.width
            }
        }
    }
}

```

MyApp is linked dynamically, that is, it needs qt libraries to work.
If we try to start the application, right after the build we get an error:
``` bash
~/build-MyApp-Desktop_Qt_5_11_1_GCC_64bit4-Release $ ./MyApp ./MyApp: /usr/lib/x86_64-linux-gnu/libQt5Qml.so.5: version `Qt_5 'not found (required by ./MyApp)
./MyApp: /usr/lib/x86_64-linux-gnu/libQt5Gui.so.5: version `Qt_5 'not found (required by ./MyApp)
./MyApp: /usr/lib/x86_64-linux-gnu/libQt5Core.so.5: version `Qt_5.11 'not found (required by ./MyApp)
./MyApp: /usr/lib/x86_64-linux-gnu/libQt5Core.so.5: version `Qt_5 'not found (required by ./MyApp)
```

From similar texts, we see that the application depends on the GUI qt libraries and qml libraries. Search and build all resources (libraries and plugins) will take a lot of time.
To save time and effort, we will use the CQtDeployer utility (you can download it [here](https://github.com/QuasarApp/CQtDeployer/releases))

or install in Snap Store

[![Download from the Snap Store](https://snapcraft.io/static/images/badges/ru/snap-store-black.svg)](https://snapcraft.io/cqtdeployer)
``` bash
cqtdeployer -bin myApp -qmake /media/D/Qt/5.12.3/gcc_64/bin/qmake -qmlDir ./
```

After executing this command, you will receive a fully ready application for work with a ready launcher, which will set up all the necessary environments for your application to work on all machines running Linux.

### Total
After running the QtDeployer, the contents of the folder with your application should look like this:
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
* myApp.sh - your application launch script
* bin - the folder with your binary
* lib - the folder with all necessary dependencies of your application.
* plugins - the qt plugins needed for an application to work
* qml -  the folder with qml dependencies.
* translations - the folder with standard qt translations.

So, you can prepare your application for packaging in a deb or snap package, after which you can begin to distribute it. Note that after running cqtdeployer, your application must be launched using the sh script, which will set up the necessary environment for your application.
