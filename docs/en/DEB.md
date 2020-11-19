## General rules for using DEB


#### Available from version CQtDeployer 1.5

### How to use deb option

Just add to cqtdeployer option "deb".

#### For example:

``` bash
    cqtdeployer ... deb
```

Where:
* **...** - is list of other options.
* **deb** - option for create the deb package.


By default, cqtdeployer will create one deb package.
If you want create multi package destribution, you should use [Controll of packages options](https://github.com/QuasarApp/CQtDeployer/wiki/Options)
To create a new package, use the -targetPackage [package; tar1] option

cqtdeployer create deb package for each packages.

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
    ],
],
"deb": true
}
```
