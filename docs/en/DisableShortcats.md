# Disabling shortcuts for multi targets distribution Kit in the Qt Installer framework.

Some times we need to disable creating automaticly shortcuts for some applications.
For solve this problem i recomendet override the default qt installer framework template.

For this you need to extract default template for your case. You can get more infrmation in this [page](ExtractDefaultsTemplates.md). 

## Case

I have a distribution that deployed using next command:

``` bash
cqtdeployer -bin myExecutableFile1,myExecutableFile2 -qmake /qmake/path qif
```
To add myExecutableFile2 to the shortcut creation exception list you need to do the following steps:

1. Extract default template.
    For extraction default template you need to add into your commnad the getDefaultTemplate option. Alsu you should add targetDir option to sets path of output files.

    ``` bash
    cqtdeployer -bin myExecutableFile1,myExecutableFile2 -qmake /qmake/path qif getDefaultTemplate -targetDir qifTemplate
    ```

    So after this you get a default qif template in the targetDir.

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

2. Change you default template.
    For disabling shortcuts you need to change the generateShortCutCmd method of the your package. In the my case it is **defaultQIFWTemplate/packages/Application/meta/installscript.qs**

    This is default implementation of install script:
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

    Add into begin of the generateShortCutCmd method next lines for disable generate shorcuts of the *"myExecutableFile2"* application:

    ``` js
    function generateShortCutCmd(cmd) {
        if (cmd.include("myExecutableFile2"))
            return;
        ...
    }
    ```

3. Add your overriden template into your deploy command.
    ``` bash
    cqtdeployer -bin myExecutableFile1,myExecutableFile2 -qmake /qmake/path qif qifTemplate/defaultQIFWTemplate
    ```

All done. Now you have a distribution with disabled shortcut for the myExecutableFile2 application.

