# How to deploy C++ Application 

In this article i describe deploy processes of c++/qt application with cqtdeployer.

## Recommendation 
If you want maximum stability of your distribution and you want to run your application on different Distributions with a different versions then you need to use Qt from official [build](https://www.qt.io/download-open-source?hsCtaTracking=9f6a2170-a938-42df-a8e2-a9f0b1d6cdce%7C6cb0de4f-9bb5-4778-ab02-bfb62735f3e5).

## Case 
In this case we deploy Qt GUI Application for Ubuntu 16.04,Ubuntu 18.04 and Ubuntu 20.04 ++.

**If you want support all distributions begin from Ubuntu 16.04 you need build and deploy you application on the older distribution (in my case it is 16.04)**

## Install CQtDeployer 
If you want use system qt from your OS then install [classic](https://github.com/QuasarApp/CQtDeployer/releases/download/1.4.7/LinuxInstaller.run) version of the cqtdeployer.

```bash
wget https://github.com/QuasarApp/CQtDeployer/releases/download/1.4.7/LinuxInstaller.run
```

```
chmod +x LinuxInstaller.run
./LinuxInstaller.run
```
![CQtDeployer Setup](https://user-images.githubusercontent.com/12465465/100077575-9280a600-2e53-11eb-8d9d-d184341cfa7c.png)

Select latest stable version (1.4)

![Screenshot from 2020-11-24 12-50-54](https://user-images.githubusercontent.com/12465465/100077707-bb08a000-2e53-11eb-9838-6f896af33105.png)

Done.

If You use official qt build then you can use snap version of the cqtdeployer.

```
sudo snap install cqtdeployer
```

After install add requirement permissions.

```
sudo snap connect cqtdeployer:process-control
sudo snap connect cqtdeployer:removable-media
sudo snap connect cqtdeployer:system-backup
```

Done!!!

## Deploy Qt GUI Application

### Deploy yor executable file.

* For Linux systems:
``` bash
cqtdeployer -bin myexecutable
```

* For Windows systems:
``` bash
cqtdeployer -bin myexecutable.exe -qmake C:/Qt/5.15.0/min_gw/bin/qmake.exe
```

* For crossplatform build (Linux -> Windows)
``` bash
cqtdeployer -bin myexecutable.exe -qmake ~/crossbuildet-Qt/5.15.0/min_gw/bin/qmake
```

## Extra options

If you use extra libraries, just add path for cqtdeployer to used libs.
``` bash
cqtdeployer -bin myexecutable -libDir /PathToMyExtraLibs 
```

If you want find libraries recursively from libDir path, just add recursive Depth option.
``` bash
cqtdeployer -bin myexecutable -libDir /PathToMyExtraLibs -recursiveDepth 5
```

If you application use qml, just add qmlDir option
``` bash
cqtdeployer -bin myexecutable -libDir /PathToMyExtraLibs -recursiveDepth 5 -qmlDir /path/to/my/qml/sources
```

If you want create simple installer for your application just add gif option.
```
cqtdeployer -bin myexecutable -libDir /PathToMyExtraLibs -recursiveDepth 5 -qmlDir /path/to/my/qml/sources qif
```

If cqtdeployer not found qmake then add the -qmake option to an invoke command.

```
cqtdeployer -bin myexecutable -libDir /PathToMyExtraLibs -recursiveDepth 5 -qmlDir /path/to/my/qml/sources qif -qmake /path/to/my/qmake

```


