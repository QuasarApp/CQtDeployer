## General rules for using options
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

## Options:

### Boolean options

| Option                      | Descriptiion                                              |
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
|   allQmlDependes            | Extracts all the qml libraries.                                 |
|                             | (not recommended, as it takes great amount of computer memory)  |
|   qif                       | Create the QIF installer for deployment programm"              |
|                             | Example: cqtdeployer qif" },                              |
|                             | Starting with CQtDeployer 1.5 you can specify the path to your own installer template. |
|                             | Examples: cqtdeployer -qif path/to/myCustom/qif. More details can be found [here](QIF) |
|   deb                       | Create the deb package for deployment programm"              |
|                             | Example: cqtdeployer deb" },                              |
|                             | you can specify the path to your own DEBIAN template. |
|                             | Examples: cqtdeployer -deb path/to/myCustom/DEBIAN. More details can be found [here](DEB) |
|   deploySystem              | Deploys all libraries not recomendet because there may be conflicts with system libraries                                           |
|   deploySystem-with-libc    | Deploys all libs include libc (only linux). Do not use this option for a gui application, for gui use the deploySystem option. (on snap version you need to turn on permission)                             |
|   noQt                      | Ignors the error of initialize of a qmake. Use only if your application does not use the qt framework.                             |
|   allowEmptyPackages        | Allows configure the empty packages.                            |
|   getDefaultTemplate        | Extracts defaults deb or qif templates. For more information see the  extracting default templates [page](ExtractDefaultsTemplates.md)       |
|  noBase | The **NoBase** option force cqtdeployer disable generate a base package. For more information about base packages see official documentation [page](BasePackages.md)|

### Deploy options

| Option                      | Descriptiion                                              |
|-----------------------------|-----------------------------------------------------------|
|   -bin [list, params]       | Deployable file or folder.                | 
|                             | For example -bin ~/my/project/bin/,~/my/project/bin.exe     |
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
|                             | For example -libDir ~/myLib,~/newLibs                           |
|   -extraLibs [list,params]  | Sets the mask of the library name for forced copying.           |
|                             | Example: "-extraLib mySql" - forces to copy all libraries whose |
|                             |names contain mySql to the project folder. This option is case sensitive. |
|   -customScript [scriptCode]| Insert extra code inTo All run script.                          |
|   -extraPlugin [list,params]| Sets an additional path to extraPlugin of an app                |
|   -recursiveDepth [params]  | Sets the Depth of recursive search of libs and ignoreEnv (default 0)          |
|   -targetDir [params]       | Sets target directory(by default it is the path to the first deployable file)|
|   -runScript [list,parems]  | forces cqtdeployer swap default run script to new from the arguments of option. This option copy all content from input file and insert all code into runScript.sh or .bat. Example of use: cqtdeployer -runScript "myTargetMame;path/to/my/myCustomLaunchScript.sh,myTargetSecondMame;path/to/my/mySecondCustomLaunchScript.sh"|
|   -verbose [0-3]            | Shows debug log                                                 |

### Controll of packages options

| Option                      | Descriptiion                                              |
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
|  -icon [package;val,val]     | Sets path to icon for package                                    |
|  -publisher [package;val,val]| Sets publisher for package                                       |
|  -homePage [package;val,val] | Sets the homepage url for a package                              |
|  -prefix [package;val,val]   | Sets the prefix for the package relatively a target directory       |
|  -extraData [package;val,val]| Adds the extra files or directories like a target. The selected directory will be copy to the extraDataOut location with save own structure.|
|  -basePackage [basePkg;pkg1;pkg2,...] | Forces the cqtdeployer create a base package for the tar1 and tar2. For more information about base packages see official documentation [page](BasePackages.md) |

### Plugins Controll Options

| Option                      | Descriptiion                                              |
|-----------------------------|-----------------------------------------------------------|
|  -extraPlugin [package;val1;val2,SingeleVal]| Sets an additional path to third-party application plug-in |
|  -enablePlugins [package;val1;val2,SingeleVal] | Enables additional plugins for distribution.  |
|  -disablePlugins [package;val1;val2,SingeleVal]| Disables plugins for distribution.            |
|                    |You can disable any plugin of your Qt build, just see the yourQtFolder/plugins forlder for available plugins.|
| |Example: if you want disable qxcb plugin: -disablePlugins qxcb. Note that the name of the plugin is indicated without its extension.|

### QtInstallFramework options

| Option                      | Descriptiion                                              |
|-----------------------------|-----------------------------------------------------------|
|  -qifStyle [path/to/style.css]| Sets the path to the CSS style file or sets the default style. Available styles: quasar |
|  -qifBanner [path/to/banner.png]| Sets path to the banner png file.                                      |
|  -qifLogo [path/to/logo.png]| Sets path to the logo png file.                                |


#### Example: cqtdeployer -bin myApp -qmlDir ~/MyAppProject/qml -qmake ~/Qt/5.15.4/gcc_64/bin/qmake clear
