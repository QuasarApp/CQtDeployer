# Отключение ярлыков для многоцелевого дистрибутива в структуре Qt Installer.

Иногда нам нужно отключить автоматическое создание ярлыков для некоторых приложений.
Для решения этой проблемы я рекомендую переопределить шаблон фреймворка установщика qt по умолчанию.

Для этого вам нужно извлечь шаблон по умолчанию для вашего случая. Вы можете получить дополнительную информацию на этой [странице](ExtractDefaultsTemplates.md).

## Кейс

У меня есть дистрибутив, развернутый с помощью следующей команды:

``` bash
cqtdeployer -bin myExecutableFile1,myExecutableFile2 -qmake /qmake/path qif
```
### где:
* **-bin myExecutableFile1, myExecutableFile2** - устанавливает исполняемые файлы.
* **-qmake /qmake/path** - устанавливает путь к вашему qmake.
* **qif** - опция для включения дистрибутивов Qt Installer FrameWork.
* **-targetPackage MyDistribution** - устанавливает новое имя для пакета распространения. 

Чтобы добавить myExecutableFile2 в список исключений создания ярлыка, вам необходимо проделать следующие шаги:



1. Извлеките шаблон по умолчанию.
    Для извлечения шаблона по умолчанию вам необходимо добавить в свою команду параметр getDefaultTemplate. Также вам следует добавить опцию targetDir для установки пути к выходным файлам.

    ``` bash
    cqtdeployer -bin myExecutableFile1,myExecutableFile2 -qmake /qmake/path qif getDefaultTemplate -targetDir qifTemplate
    ```

    ### где:
    * **- targetDir qifTemplate** - устанавливает целевой каталог для tempalte.
    * **getDefaultTemplate** - опция для извлечения шаблона по умолчанию. 

    Итак, после этого вы получите шаблон qif по умолчанию в targetDir.
    ``` bash 
    ~/qifTemplate$ tree
    .
    └── defaultQIFWTemplate
        ├── config
        │   ├── config.xml
        │   └── controlScript.qs
        └── packages
            └── Application
                └── meta
                    ├── installscript.qs
                    └── package.xml

    5 directories, 4 files

    ```

2. Измените шаблон по умолчанию.
    Для отключения ярлыков вам необходимо изменить метод generateShortCutCmd вашего пакета. В моем случае это **defaultQIFWTemplate/packages/Application/meta/installscript.qs**

    Это реализация сценария установки по умолчанию:
    ``` js
    function Component() {

    }

    function extractFileName(path) {
        const fullName = path.substring(path.lastIndexOf('/') + 1);

        const index = fullName.lastIndexOf('.');
        if (index >= 0) {
            return fullName.substring(0, index)
        }

        return fullName;
    }

    function generateShortCutCmd(cmd) {
        if (systemInfo.kernelType === "winnt") {

            console.log("create icons!!! on Windows");

            component.addOperation(
                "CreateShortcut",
                "@TargetDir@/" + cmd + ".bat",
                "@DesktopDir@/" + extractFileName(cmd) + ".lnk",
                "iconPath=@TargetDir@/icons/Icon.png",
                "iconId=0");

        }


        if (systemInfo.kernelType === "linux") {
            console.log("create icons!!! on LINUX");
            const name = extractFileName(cmd);
            component.addOperation("CreateDesktopEntry",
                                "@HomeDir@/.local/share/applications/" + name + ".desktop",
                                "Version=@Version@\n
                                    Type=Application\n
                                    Terminal=false\n
                                    Exec=\"@TargetDir@/" + cmd + ".sh\"\n
                                    Name=" + name + "\n
                                    Icon=@TargetDir@/icons/Icon.png\n
                                    Name[en_US]=" + name);

            console.log("create icons!!! on LINUX done");
        }
    }

    Component.prototype.createOperations = function() {
        // call default implementation to actually install README.txt!
        component.createOperations();

        const cmdArray = []; // will be changed in cqtdeployer

        cmdArray.forEach( function (item){
            generateShortCutCmd(item);
        });

    }
    ```

    Добавьте в начало метода generateShortCutCmd следующие строки для отключения генерации ярлыков приложения *"myExecutableFile2"*:

    ``` js
    function generateShortCutCmd(cmd) {
        if (cmd.include("myExecutableFile2"))
            return;
        ...
    }
    ```

3. Добавьте измененный шаблон в команду развертывания.
    ``` bash
    cqtdeployer -bin myExecutableFile1,myExecutableFile2 -qmake /qmake/path qif qifTemplate/defaultQIFWTemplate
    ```

Все сделано. Теперь у вас есть дистрибутив с отключенным ярлыком для приложения myExecutableFile2. 

## Примечание. Если вы используете файл конфигурации или другую команду развертывания, ваша конфигурация развертывания должна соответствовать конфигурации генерации шаблона. 