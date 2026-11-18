# Packaging distributions

General information on packaging your distributions in packages.

## Packaging Types

* Automatic
* Manual

## Automatic Packages

Packages with this type creates automaticly.
By default the distribution have a one main package. You can configure it using [Controll of packages options](Options.md).

#### Example (Set new name for package)

```
cqtdeployer ... -name "MyPackage"
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


### Target selectors

The target name of the `-targetPackage`, `-icon`, `-runScript`,
`-extraDepends`, `-disableShortCut` and the `-disableRunScript` options is
a target selector: every argument of the tool that selects a target uses
this single selector (issue #311). A selector that does not contain the
`*` character selects the one target that has this name exactly (the
comparison is case-insensitive). A selector that contains `*` is a glob
pattern: every `*` matches any sequence of characters, so one selector
can select several targets.

Example: the distribution contains the `win32`, `win64` and `linux`
targets, the option

```
cqtdeployer ... -targetPackage winPackage;win*,linuxPackage;linux
```

creates the `winPackage` package from the `win32` and the `win64`
targets and the `linuxPackage` package from the `linux` target. Without
the `*` the `win` selector would not match any target, because a plain
selector must equal the target name exactly.


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


### Package selectors

The package name of the package options (`-name`, `-description`,
`-deployVersion`, `-releaseDate`, `-publisher`, `-homePage`, `-prefix`,
`-extraData`, `-tr`, `-binOut`, `-qmlOut`, `-libOut`, `-trOut`,
`-pluginOut`, `-recOut`, `-extraDataOut`, `-installDirDeb`, `-debOut`,
`-zipOut`, `-appimageOut`, `-dmgOut`, `-qmlDir`, `-extraPlugin`,
`-enablePlugins` and the `-disablePlugins` options) is a package
selector: every argument of the tool that selects a package uses this
single selector (the same exact/glob semantics of the [target
selectors](#target-selectors), issue #311). A selector that does not
contain the `*` character selects the one package that has this name
exactly (the comparison is case-insensitive). A selector that contains
`*` is a glob pattern: every `*` matches any sequence of characters, so
one selector can select several packages. A value without the selector
(the single level value) is set to the default package.

Example: the distribution contains the `win32Package`, `win64Package`
and the `linuxPackage` packages, the option

```
cqtdeployer ... -name win*Package;Windows Package
```

sets the `Windows Package` name to the `win32Package` and the
`win64Package` packages and the `linuxPackage` package keeps its name.
Without the `*` the `winPackage` selector would not match any package,
because a plain selector must equal the package name exactly.

### Command line globs

The values of the path options (for example the `-bin` option) support
the `*` glob pattern (issue #311): the value that contains the `*`
character is expanded to the files and the folders that match the
pattern, every `*` matches any sequence of characters and all the other
characters are matched literally (the pattern is matched against the
name of the last path component in the directory of the pattern, in the
current working directory when the pattern is a plain file name).

```
cqtdeployer ... -bin 'bin/*.exe'
```

deploys all the executable files of the `bin` folder.

The shell (bash, zsh, ...) expands the unquoted glob of the command line
into the separate file arguments before the tool starts, so the tool
merges the expanded file list back into the single value of the option
(the files are joined with the `,` separator of the list options), the
unquoted pattern works the same way as the quoted one:

```
cqtdeployer ... -bin bin/*.exe qtifw
```

is the same command as the quoted example above (the shell passes the
`bin/app1.exe bin/app2.exe` file list to the tool and the tool sets the
`bin` option to the `bin/app1.exe,bin/app2.exe` value).

The command line of the tool is parsed with the Qt `QCommandLineParser`
(the official Qt command line parser, the review of the PR #928): the
`-option value` and the `-option=value` forms of the options and the
double-dash form of the option (the `--option value` form) are accepted
by the parser, and the bare option names of the command line (the
`qtifw`, `deb`, ... flags of the command line) are normalized by the tool
to the dash form of the parser (the `DeployCore::normalizeCommandLine`
function), so every form of the command line works with the globs the
same way (see the [Options general rules](Options.md#options-general-rules)
documentation).

## Manual Packages

Manual packages is packages created by user template.
For example you have a aplication that must be execute a complex script on the debian package or the installer.
So you need to create your template and add tehe path to you template for a package option.

Example

```
cqtdeployer ... -qtifw /path/to/my/template/installer
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
* [AppImage Package](AppImage.md)
* [DMG Package](DMG.md)
