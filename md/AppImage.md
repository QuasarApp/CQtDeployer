# AppImage Using

General rules for using AppImage


Available from version CQtDeployer 1.7

### How to use appimage option

Just add to cqtdeployer option "appimage".

### For example:

```bash
cqtdeployer ... appimage
```

Where:
* **...** - is list of other options.
* **appimage** - option for use AppImage packages.

By default, cqtdeployer will create one AppImage package.
If you want create multi package distribution, you should use [Controll of packages options](Options.md)
To create a new package, use the -targetPackage [package; tar1] option

cqtdeployer create AppImage package for each packages.

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
"appimage": true
}
```

### Name of the output file

By default, cqtdeployer creates the AppImage with the name `<name>.AppImage` in the
target dir (`<targetDir>/<name>.AppImage`), where `<name>` is the name of the
package. You can change the name of the output file with the `appimageOut`
option:

```bash
cqtdeployer ... appimage -appimageOut MyName.AppImage
```

For the multi packages distributions use the
`-appimageOut package;nameOfOutputAppImageFile` syntax (the same way as the
`zipOut` and the `debOut` options).

### How the AppImage package is created

The AppImage package is the [AppImage](https://appimage.org/) of the
root layout:

```bash
└── <name> (the AppDir)
    ├── AppRun            - the launch script of the AppImage
    ├── <name>.desktop    - the desktop file of the application
    ├── <name>.png        - the icon of the application
    └── bin/...           - the deployed files of the package
```

The AppImage is created with the `appimagetool` tool.

### The appimagetool tool

All the tools required to create an AppImage are included within the
CQtDeployer package (much like the Qt Installer Framework tools), so the
AppImage packages can be created on the hosts where the AppImage tools are
not present on the system by default. The tool is staged in the `AppImage`
folder next to the cqtdeployer executable (the `appimagetool` wrapper
script and the `appimagetool.AppImage` tool).

If the appimagetool is already installed on the system (for example, as the
`appimage` package in distributions like Debian), then the system tool is
used - the system PATH is searched before the bundled tools.

The AppImage tool requires the FUSE support of the host to run. When the
host has no FUSE support (for example the minimal container images), the
wrapper script runs the tool in the `--appimage-extract-and-run` mode of
the AppImage runtime: the tool is extracted to a temporary directory and
executed from there.

When the wrapper script is missing or is not executable (for example the
executable bit was lost during the extraction of the CQtDeployer package
archive), the cqtdeployer falls back to the `appimagetool.AppImage` tool
of the `AppImage` folder directly (the FUSE support of the host is
required in this case).
