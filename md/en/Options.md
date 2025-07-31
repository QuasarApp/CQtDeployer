# Options general rules 

General rules for using options

```
cqtdeployer -option1 value1 -option2 list, of, values ​​flag1 flag2 flag3
```

* All options must begin with a '-' sign.
* After specifying the desired option, you must specify the value.
* Some options support enumerations (arrays of values). When initializing such options, specify the list of values through ',' without spaces. If you use any programming language and call the cqtdeployer process, then the array of values must be passed as 1 parameter.
* Some options support redistributions with the ability to select the target for which the flag will be applied. Target and values are separated using the ';' sign. Example -libOut target1;value1,target2;value2,defaultValue.
* To include a flag, simply include the flag name in the list of options.

## Description of values in the table

* **-binDir [params]** - option without array support.
* **-bin [params,list]** - option with support for arrays. The delimiter used is ','.
* **-libOut [package;path,path]** - parameter with support for selecting the package for which the flag value is set. As a separator, use ';'. Please note that the rule described above is used to enumerate parameter values. The last parameter does not have a package value, this means that this value will be set for all non-configured packages by default.
* **clear** - boolean option

## Definitions in description

**System environments** - paths of directories containing the system libraries.
 * Windows:
   * Paths defined in the PATH variable.
   * C:/Windows/System32
   * C:/Windows/SysWOW64
 * Linux:
   * Paths defined in the LD_LIBRARY_PATH and PATH variables.
   * /usr/lib
   * /lib

**Deployable files** - all file that will be copied into distribution (include executable files)

**Targets** - all executable files marked by bin option.

**Executable files** - all files with PE, ELF and Mach-O formats.

## Options:

### Boolean options

| Option                      | Description                                              |
|-----------------------------|-----------------------------------------------------------|
|   init                      | will initialize cqtdeployer.json file (configuration file)|
|                             | For example: "cqtdeployer init" - for initialize base package configuration|
|                             | "cqtdeployer -init multi" - for initialize multi package configuration|
|                             | "cqtdeployer -init single" - for initialize single package configuration |
|   help / h                  | Shows help.                                               |
|   clear                     | Deletes deployable files of the previous session.
|   force-clear               | Deletes the destination directory before deployment.      |
|   noStrip                   | Skips strip step                                          |
|   noTranslations            | Skips the translations files.                             |
|                             | It doesn't work without qmake and inside a snap package   |
|   noOverwrite               | Prevents replacing existing files.                              |
|   noCheckRPATH              | Disables automatic search of paths to qmake in executable files.|
|   noCheckPATH               | Disables automatic search of paths to qmake in system PATH.     |
|   noRecursiveiIgnoreEnv     | Disables recursive ignore for ignoreEnv option.                 |
|   v / version               | Shows compiled version                                          |
|   qif                       | Create the QIF installer for deployment program"              |
|                             | Example: cqtdeployer qif" },                              |
|                             | Starting with CQtDeployer 1.5 you can specify the path to your own installer template. |
|                             | Examples: cqtdeployer -qif path/to/myCustom/qif. More details can be found [here](QIF.md) |
|   deb                       | Create the deb package for deployment programm"              |
|                             | Example: cqtdeployer deb" },                              |
|                             | you can specify the path to your own DEBIAN template. |
|                             | Examples: cqtdeployer -deb path/to/myCustom/DEBIAN. More details can be found [here](DEB.md) |
|   deploySystem              | Deploys all libraries not recommended because there may be conflicts with system libraries  |
|   noQt                      | Ignores the error of initialize of a qmake. Use only if your application does not use the qt framework. |
|   allowEmptyPackages        | Allows configure the empty packages.                            |
|   getDefaultTemplate        | Extracts defaults deb or qif templates. For more information see the  extracting default templates [page](ExtractDefaultsTemplates.md)       |
|   noHashSum                 | Disable computation of a packages hash sum               |


### Deploy options

| Option                      | Description                                              |
|-----------------------------|-----------------------------------------------------------|
|   -bin [list,params]        | Files to deploy or folders that contain files to deploy. For example -bin ~/my/project/bin/,~/my/project/bin.exe,~/my/project/runtimeLinking/lib.dll. For files: These files will be unconditional copied to the destination directory, regardless of their format or suffix. For folders: CQtDeployer will enter these folders and non-recursively copy all executable files to the destination directory. Then, CQtDeployer will extract all dependencies of the copied files and search dependencies in system environments and **libDir** paths. **Note**: If CQtDeployer can't find required file then CQtDeployer try find required file in the system PATH enviroment.|
|   -binPrefix [prefixPath]   | Sets prefix path for bin option. Example: **-bin path/MyExecutable** is some as **-bin MyExecutable -binPrefix path** |
|   -confFile [params]        | The path to the json file with all deployment configurations.|
|                             | Using this file, you can add the necessary options, |
|                             | thereby simplifying  in the command invocation the console. |
|                             | However, the parameters in Kansol have a higher priority than in the file.|
|                             | For more info about this flag see [wiki](DeployConfigFile)                           |
|   -qmlDir [params]          | Qml data dir. For example -qmlDir ~/my/project/qml              |
|   -qmake  [params]          | Qmake path. For example                                         |
|                             | -qmake ~/Qt/5.14.0/gcc_64/bin/qmake                             |
|   -ignore [list,params]     | The list of libs to ignore.                                     |
|                             | For example -ignore libicudata.so.56,libicudata2.so.56          |
|   -ignoreEnv [list,params]  | The list of the environment to ignore.                          |
|                             | For example -ignoreEnv /bad/dir,/my/bad/Dir                     |
|   -libDir [list,params]     | Sets additional paths for extra libs of an app.                 |
|                             | For example -libDir ~/myLib,~/newLibs.  **Attention** the libDir option should not be children of the targetDir directory. The targetDir option is forbidden for library searches, as it may contain outdated project libraries, which in turn may lead to unwanted crashes.                         |
|   -extraLibs [list,params]  | Sets the mask of the library name for forced copying.           |
|                             | Example: "-extraLib mySql" - forces to copy all libraries whose names contain mySql to the project folder. This option is case-insensitive on Windows and case-sensitive on other platforms. This option will only search libraries in system environments similar to **deploySystem**.|
|   -customScript [scriptCode]| Insert extra code inTo All run script.                          |
|   -extraPlugin [list,params]| Sets an additional path to extraPlugin of an app                |
|   -recursiveDepth [params]  | Sets the Depth of recursive search of libs and ignoreEnv (default 0)          |
|   -targetDir [params]       | Sets target directory(by default it is the path to the first deployable file)|
|   -installDirDeb [params]   | Sets install target directory fordebian package (by default it is /opt path) |
|   -installDirQIFW [params]  | Sets install target directory for installers (by default it is /home path) |
|   -verbose [0-3]            | Shows debug log. By default it is 2                             |
|   -platform [list]          | If this option is enabled then CQtDeployer will deploy only binaries of a selected platform. Supported values: [win_x86 win_x86_64 win_arm win_arm64 linux_x86 linux_x86_64 linux_ARM linux_ARM64]  |

### Controll of packages options

| Option                      | Description                                              |
|-----------------------------|-----------------------------------------------------------|
|  -targetPackage [package;tar1,package;tar2]| Creates a new package and adds 'tar1 and tar2' to it |
|  -qmlOut [package;path,path] | Sets path to qml out directory                                  |
|  -libOut [package;path,path] | Sets path to libraries out directory                            |
|  -trOut [package;path,path]  | Sets path to translations out directory                         |
|  -pluginOut [package;path,path]| Sets path to plugins out directory                            |
|  -binOut [package;path,path] | Sets path to binary out directory                               |
|  -recOut [package;path,path] | Sets path to recurses out directory                             |
|  -extraDataOut [package;path,path] | Sets path to extra data files out directory. By Default it is root dir of the distribution.         |
|  -name [package;val,val]     | Sets name for package. If this if you do not specify a package, the value will be assigned to the default package ("")|
|  -description [package;val,val] | Sets description for package                                  |
|  -deployVersion [package;val,val] | Sets version for package                                    |
|  -releaseDate [package;val,val] | Sets release date for package                                 |
|  -publisher [package;val,val]| Sets publisher for package                                       |
|  -homePage [package;val,val] | Sets the homepage url for a package                              |
|  -prefix [package;val,val]   | Sets the prefix for the package relatively a target directory       |
|  -extraData [package;val,val]| Adds the extra files or directories like a target. The selected directory will be copy to the extraDataOut location with save own structure.|
|  -tr [package;val,val]       | Adds qm files into the translations folder.                     |

### Controll of targets options

| Option                      | Description                                              |
|-----------------------------|-----------------------------------------------------------|
|  -icon [target;val,val]    | Sets path to icon for a targets. This option support only png (Linux) and ico (Windows) files.  |
|  -disableRunScript [target,target2,target3]  | Disables the generation of run script for selected targets|
|  -disableShortCut [target,target2,target3]   | Disables the generation of shortcut for selected targets |
|  -runScript [target;val,val]  | forces cqtdeployer swap default run script to new from the arguments of option. This option copy all content from input file and insert all code into runScript.sh or .bat. Example of use: cqtdeployer -runScript "myTargetMame;path/to/my/myCustomLaunchScript.sh,myTargetSecondMame;path/to/my/mySecondCustomLaunchScript.sh" For get more information about customScript see the documentation [page](CustomScripts.md)|
|  -extraDepends [target;val,val]  | Adds extra dependencies for target, if package is skiped then for all targets. Example -extraDepends libssl.so or -extraDepends myExecutable;libssl.so           |

### Plugins Control Options
 
| Option                      | Description                                              |
|-----------------------------|-----------------------------------------------------------|
|  -extraPlugin [package;val1;val2,SingeleVal]| Sets an additional path to third-party application plug-in |
|  -enablePlugins [package;val1;val2,SingeleVal] | Enables additional plugins for distribution.  |
|  -disablePlugins [package;val1;val2,SingeleVal]| Disables plugins for distribution.            |
|                    |You can disable any plugin of your Qt build, just see the yourQtFolder/plugins folder for available plugins.|
| |Example: if you want disable qxcb plugin: -disablePlugins qxcb. Note that the name of the plugin is indicated without its extension.|

### QtInstallFramework options

| Option                      | Description                                              |
|-----------------------------|-----------------------------------------------------------|
|  -qifStyle [path/to/style.css]| Sets the path to the CSS style file or sets the default style. Available styles: quasar |
|  -qifBanner [path/to/banner.png]| Sets path to the banner png file.                                      |
|  -qifLogo [path/to/logo.png]| Sets path to the logo png file.                                |
|  -qifOut [nameOfOutputInstallerFile] | Sets name of output qifw installer. Note: on Windows, the exe suffix will be added to the installer automatically.                |
|  -qifConfig [path/to/config.xml] | Sets a custom path to the configure file of the qt ifw installer. By default it is qif/config/config.xml. Note This path sets relative target folder (sets by TargetDir option). |
|  -qifPackages [path/to/packagesFodoler] | Sets a custom path to the packages directories. By default it is qif/packages. Note This path sets relative target folder (sets by TargetDir option). |
|  -qifResources [path/to/resources1.qrc,path/to/resources2.qrc] | Sets a custom path to the resources files. By default this option is skipped. Note This path sets relative target folder (sets by TargetDir option). |
|  -qifArchiveFormat [7z,zip,tar,tar.gz,tar.bz2,tar.xz] | Sets the format used when packaging new component data archives. If you omit this option, the 7z format will be used as a default. |
|  -binarycreator [binarycreator command] | Sets new binarycreator command. Example : cqtdeployer -bin my.exe qifw -binarycreator 'wine path/to/binarycreator.exe'|

### Deb package options

| Option                      | Description                                              |
|-----------------------------|-----------------------------------------------------------|
|  -debOut [package;nameOfOutputDebFile,nameOfOutputDebFile]| Sets name of the output debian file. This option can be work with multiple packages |

### Zip package options

| Option                      | Description                                              |
|-----------------------------|-----------------------------------------------------------|
|  -zipOut [package;nameOfOutputZipFile,nameOfOutputZipFile]| Sets name of the output zip archive. This option can be work with multiple packages |

#### Example: cqtdeployer -bin myApp -qmlDir ~/MyAppProject/qml -qmake ~/Qt/5.15.4/gcc_64/bin/qmake clear
