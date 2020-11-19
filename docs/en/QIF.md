## General rules for using Qt Installer Framwork

### What is Qt Installer Framwork

The **Qt Installer Framework** (QIF) provides a set of tools and utilities to create installers for the supported desktop Qt platforms: Linux, Microsoft Windows, and OS X.
In **CQtDeployer** QIF availabel in qif option.

### How to use qif option

The qif option invoke with path to custo qt installer template or with default installer template.

#### For example:

``` bash
    cqtdeployer ... qif
```

Where:
* **...** - is list of other options.
* **qif** - option for use qt installer framework with default template.


By default, cqtdeployer will create an installation file with one package in which your entire distribution kit will lie.
If you want to configure it, you should use [Controll of packages options](https://github.com/QuasarApp/CQtDeployer/wiki/Options)
To create a new package, use the -targetPackage [package; tar1] option


Where:

* **package** - it is package name
* **tar1** - it is name of the target that should be included in this package.

Or you can describe packages in [configuration file](https://github.com/QuasarApp/CQtDeployer/wiki/DeployConfigFileEn)


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
### Template
The qif template should contain 2 folders:
* packages
* config

### Attention
The name of the packages in the packages folder must match the names of the packages during deployment.

For example:
``` json
"targetPackage": [
    [
        "myCustomInstaller",
        "tar1"
    ]
],
```

``` bash
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
