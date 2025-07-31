# Qt Installer Framwork

General rules for using Qt Installer Framwork

### What is Qt Installer Framwork

The **Qt Installer Framework** (QIF) provides a set of tools and utilities to create installers for the supported desktop Qt platforms: Linux, Microsoft Windows, and OS X.
In **CQtDeployer** QIF availabel in qif option.

### How to use qif option

The qif option invoke with path to custo qt installer template or with default installer template.

#### For example:

```bash
    cqtdeployer ... qif
```

Where:
* **...** - is list of other options.
* **qif** - option for use qt installer framework with default template.


By default, cqtdeployer will create an installation file with one package in which your entire distribution kit will lie.
If you want to configure it, you should use [Controll of packages options](Options.md)
To create a new package, use the -targetPackage [package; tar1] option


Where:

* **package** - it is package name
* **tar1** - it is name of the target that should be included in this package.

Or you can describe packages in [configuration file](DeployConfigFile.md)


```json
{
"targetPackage": [
    [
        "package",
        "tar1"
    ]
],
}
```

# Custom Template for QIF

Starting with CQtDeployer Version 1.5 you can use your own installer template. To do this, you must pass the path to your template to the qif parameter.

```bash
cqtdeployer ... -qif /path/to/my/custom/qif/template
```

### Template

The qif template should contain 2 folders:
* packages
* config

### Attention

The name of the packages in the packages folder must match the names of the packages during deployment.

For example:

```json
"targetPackage": [
    [
        "myCustomInstaller",
        "tar1"
    ]
],
```

```bash
.
├── config
│ ├── config.xml
│ └── style.qss
└── packages
    └── myCustomInstaller
        └── meta
            ├── installscript.qs
            └── package.xml

4 directories, 4 files

```

If the folder names do not match the package names then CQtDeployer will use the standard QIF package.

If you have only one package then you can use the **name** option for set name the application.

```bash
cqtdeployer ... -qif /path/to/my/custom/qif/template -name myCustomInstaller
```

#### Do not forget to rename the your package folder to the packaga name of the name option.


```bash
.
├── config
│   ├── config.xml
│   └── style.qss
└── packages
    └── myCustomInstaller  << "This is folder of the your package"
        └── meta
            ├── installscript.qs
            └── package.xml

4 directories, 4 files

```

## Initialization of the qifw template

For initialize default templates you can use the getDefaultTemplate option.
The getDefaultTemplate option extract default template of the cqtdeployer. This is can be very convinion if you want override default behavior of the installer or anothe distribution package.

### Example

### Getting default qt installer framwork template. 

```bash
cqtdeployer getDefaultTemplate qif
```


### Note

CQtDeployer will skip create a packages directory for the template because this commmnad do not contains any deploying data.
If you want to prepare template with packages configurations then you should add deployed data to your command or your config.json file using bin or extraData options.
If you create multi packages distribution then you need to configure your packages in your deploying command.


#### Example: 

Extracting template with packages:

```bash
cqtdeployer getDefaultTemplate qif -bin myExecutable
```

Extracting template for multi packages distribution

```bash
cqtdeployer getDefaultTemplate qif -bin myExecutable1,myExecutable2 -targetPackage p1;myExecutable1,p2;myExecutable2
```

You also can use the config file for configure templates.

Config.json

```json
{
    "qif": true,
    "bin": [
        "myExecutable1",
        "myExecutable2"
    ],
    "targetPackage": [
        ["p1", "myExecutable1"],
        ["p2", "myExecutable2"]
    ]
}
```

Run CQtDeployer for generate template:

```bash
cqtdeployer -confFile Config.json getDefaultTemplate
```
