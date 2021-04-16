# General information on packaging your distributions in packages.

## Packaging Types
* Automatic
* Manual

## Automatic Packages
Packages with this type creates automaticly.
By default the distribution have a one main package. You can configure it using [Controll of packages options](Options.md).

#### Example (Set new name for pacakge)
```
ctdeployer ... -name "MyPackage"
```

If your distribution have a multiple executable files then you can be create a distribution with multiple package.

One package can contain from 1 to infinity of executable files.

For configure a package you must be use the targetPackage option.

Example of using:

```
cqtdeployer ... -targetPackage myPacage;target1,myPacage2;target2
```

Where myPacage is a name of package and target1 is a name of executable file.
so This example create 2 packages from 2 targets.

The Next example create a 2 packages from 3 targets.
```
cqtdeployer ... -targetPackage myPacage;target1,myPacage;target2,myPacage2;target3
```


You can also do the same in the config file

```
{
"targetPackage": [
        [
            "myPacage",
            "target1"
        ],
        [
            "myPacage2",
            "target2"
        ]
    ]
}
```

```
{
"targetPackage": [
        [
            "myPacage",
            "target1"
        ],
        [
            "myPacage",
            "target2"
        ],
        [
            "myPacage2",
            "target3"
        ]
    ]
}
```


## Manual Packages
Manual packages is packages created by user template.
For example you have a aplication that must be execute a complex script on the debian package or the installer.
So you need to create your template and add tehe path to you template for a package option.

Example

```
cqtdeployer ... -qif /path/to/my/template/installer
```

The /path/to/my/template/installer folder must be contains a folder with the package name.
for example i am deploy application with the "MyExample" name, so my custom template must be locate in the 
/path/to/my/template/installer/MyExample path.

For multi-package distributions, the same rules apply.

Your folder with the templates must be contains folders with same names as packages.

Example:

```
{
"targetPackage": [
        [
            "myPacage",
            "target1"
        ],
        [
            "myPacage",
            "target2"
        ],
        [
            "myPacage2",
            "target3"
        ]
    ]
}
```

```
~/path/to/my/template/installer$ tree 
.
├── myPacage
│   └── ...
└── myPacage2
    └── ...

```

### Note
You can also extract the standard package template in order to override it if for some reason you are not satisfied with the standard implementation. See this [page](ExtractDefaultsTemplates.md) for details on how to do this 


## Available packages
* [Qt Install Framework](QIF.md)
* [ZIP Arhive](ZIP.md)
* [DEB Package](DEB.md)
