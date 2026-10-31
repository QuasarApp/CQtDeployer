# DMG Using

General rules for using DMG


Available from version CQtDeployer 1.7

### How to use dmg option

Just add to cqtdeployer option "dmg".

### For example:

```bash
cqtdeployer ... dmg
```

Where:
* **...** - is list of other options.
* **dmg** - option for use DMG packages.

By default, cqtdeployer will create one DMG package.
If you want create multi package distribution, you should use [Controll of packages options](Options.md)
To create a new package, use the -targetPackage [package; tar1] option

cqtdeployer create DMG package for each packages.

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
"dmg": true
}
```

### Name of the output file

By default, cqtdeployer creates the DMG with the name `<name>.dmg` in the
target dir (`<targetDir>/<name>.dmg`), where `<name>` is the name of the
package. You can change the name of the output file with the `dmgOut`
option:

```bash
cqtdeployer ... dmg -dmgOut MyName.dmg
```

For the multi packages distributions use the
`-dmgOut package;nameOfOutputDmgFile` syntax (the same way as the
`zipOut` and the `debOut` options).

### How the DMG package is created

The DMG package is the [DMG (the Disk Image of the macOS)](https://en.wikipedia.org/wiki/Disk_Image)
of the deployed package:

```bash
└── <name> (the root of the DMG volume)
    ├── <name>/           - the deployed files of the package (the ready-to-use application)
    ├── <name>.png        - the icon of the application
    ├── README            - the name, the description, the version, the publisher and the homepage of the package
    └── Applications      - the symbolic link to the /Applications folder (the standard drag-to-install UX)
```

The DMG is created with the `hdiutil` tool of the macOS system
(the same way the `deb` backend uses the `dpkg-deb` tool of the Linux
system):

```bash
hdiutil create -volname <name> -srcfolder <root> -ov -format UDBZ -o <targetDir>/<name>.dmg
```

Where:
* **<name>** - is the name of the package (the name option), the name of the volume of the DMG.
* **<root>** - is the root folder of the DMG volume (the staging folder of the Dmg distribution).
* **UDBZ** - is the compressed UDIF format of the standard macOS DMG files.

### How to use the created DMG

1. Open the DMG file (double click on it in the Finder).
2. Drag the `<name>` folder into the Applications folder (the
   Applications folder is available as a symbolic link in the DMG window).
3. Launch the application from the Applications folder (the run script
   of the package starts the application with the right environment, see
   the [macOS documentation](macOS.md)).

### Limitations

* The dmg option is available only on the macOS host platforms: the
  DMG packages are created with the `hdiutil` tool of the macOS system.
  On the other host platforms the option is skipped with a warning
  (the same rule as the `deb` and the `appimage` options).
* The dmg option was an alias for the `qtifw` option before CQtDeployer
  1.7 (issue #884): the binarycreator of the Qt Installer Framework
  created the installer (the ".app" bundle of the macOS hosts). Now the
  dmg option creates the real DMG packages with the hdiutil tool
  (issue #899), the qtifw option is unchanged.
