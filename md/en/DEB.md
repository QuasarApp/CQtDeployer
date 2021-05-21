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
If you want create multi package destribution, you should use [Controll of packages options](Options.md)
To create a new package, use the -targetPackage [package; tar1] option

cqtdeployer create deb package for each packages.

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
"deb": true
}
```


## Custom Template for DEB

You can create a own Template fo the debian pacakges.

### The debian pacakge structure: 


```bash
└── Debian templates
    ├── debPacakge1
    │   └── DEBIAN
    │       ├── control
    │       ├── postinst
    │       └── prerm
    └── debPacakge2
        └── DEBIAN
            ├── control
            ├── postinst
            └── prerm

```

For include your templates into your destribution you need to add path of the Debian templates folder into deployConfig.json

```json
{
"targetPackage": [
    [
        "debPacakge1",
        "tar1"
    ],
    [
        "debPacakge2",
        "tar2"
    ],
],
"deb": "./Debian templates"
}
```
