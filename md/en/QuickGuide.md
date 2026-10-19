# Quick Guide

How to deploy project with cqtdeployer.

1. First one you need to install cqtdeployer into your platform

    * Snap (14 more linux destributions):
    ```
    sudo snap install cqtdeployer
    ```
    Or 

    Install from snap store and add all requirement permissions.

    [![Download from Snap Store](https://snapcraft.io/static/images/badges/ru/snap-store-black.svg)](https://snapcraft.io/cqtdeployer)

    * Other linux [LinuxInstaller.run][1].
    ```
    chmod +x LinuxInstaller.run
    ./LinuxInstaller.run
    ```

    * Windows [installer][1]

    * macOS [DMG][1]: open the DMG file and move the `CQtDeployer.app` bundle into the `Applications` folder.
    ``` bash
    open CQtDeployer_*.dmg
    cp -R /Volumes/CQtDeployer/CQtDeployer.app /Applications/
    ```

2. Build your project on release mode.
3. Deploy yor executable file.

    * For Linux systems:
    ``` bash
    cqtdeployer -bin myexecutable
    ```

    * For Windows systems:
    ``` bash
    cqtdeployer -bin myexecutable.exe -qmake C:/Qt/5.15.0/min_gw/bin/qmake.exe
    ```

    * For macOS systems:
    ``` bash
    cqtdeployer -bin myexecutable -qmake /Users/<user>/Qt/6.10.3/macos/bin/qmake
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

If you want create simple installer for your application just add qif option.
```
cqtdeployer -bin myexecutable -libDir /PathToMyExtraLibs -recursiveDepth 5 -qmlDir /path/to/my/qml/sources qif
```

> **Note:** the Qt Installer Framework tools (binarycreator) are not published
> for macOS, so the `qif` option is skipped gracefully on macOS hosts. To
> create a portable archive of your deployed macOS application use the `zip`
> packager. See the [CQtDeployer for macOS](macOS.md) page.

If you use system libraries from system PATH, just add deploySystem option. 

#### Warning: snap version do not have permision to system files. To deploy system libraries use the cqtdeployer version from the [installer][1]

```
cqtdeployer -bin myexecutable -libDir /PathToMyExtraLibs -recursiveDepth 5 -qmlDir /path/to/my/qml/sources qif deploySystem
```


### If you have any problems with use cqtdeployer then ask you questions on the official github issues [page][2].

### For more options of CQtDeploye see oficial [documentation][3].

[1]: https://github.com/QuasarApp/CQtDeployer/releases
[2]: https://github.com/QuasarApp/CQtDeployer/issues
[3]: https://github.com/QuasarApp/CQtDeployer/blob/main/md/en/Options.md

