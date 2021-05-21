## General rules for using ZIP


#### Available from version CQtDeployer 1.5

### How to use zip option

Just add to cqtdeployer option "zip".

#### For example:

``` bash
    cqtdeployer ... zip
```

Where:
* **...** - is list of other options.
* **zip** - option for use zip arhives.


By default, cqtdeployer will create one zip arhive.
If you want create multi package destribution, you should use [Controll of packages options](Options.md)
To create a new package, use the -targetPackage [package; tar1] option

cqtdeployer create zip arhive for each packages.

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
    ],
],
"zip": true
}
```
