# Change log 

## CQtDeployer 1.5.3

## New options 
* installDirDeb - Sets install target directory for debian  package (by default it is /opt path) #640 
* installDirQIFW - Sets install target directory for installers (by default it is /home path) #640 

## Bug fixes :
* Fix warning on installer fix #628
* Added support validation input options of the command line fix #421 
* Added Error message for conflict between libDir and targetDir options #629 

## CQtDeployer 1.5.2

### New parameters

* Added **qifOut** option for set name of the output installer
* Added **disableRunScript** option for disabling the generation of run script for selected targets
* Added **disableShortCut** option for disabling the generation of shortcut for selected targets

### Fixes

* Fix documentation
* Fix deploy openssl libraries

## CQtDeployer 1.5.1 

### Fixes

* Fix deploy multi target Debian packages
* Fix deploy qt6 qml applications


Change log for all versions of the CQtDeployer.

## CQtDeployer 1.5.0

### New features

- Added new theme of qif installer "quasarDark".
- Added support of work with custom template for qif option.
- Added support of debian packages.
- Added support icons for targets. now the icon option work with targets but not packages.
- Added support of the control custom translation files. 
- Added support the deploy nonexecutable data. (extraData option)
- Added envirement variables for deployed applications.
- Added support of the generate deb packages.
- Added support of the generate zip arrhives.
- Added support create a qif installer using custom template
- Added command of init default qif of deb template (getDefaulttemplate)
- Added support of import custom launch scripts
- Added support of md5 packages hashes
- Added support of Qt6.1 

### Fixes

- Fixed stability for the deploySystem option on windows
- Fixed verbose log.
- Fixed general output log.
- Fixed typo errors in log.
- Fixed crossdeploy of the arm distributions. 
- Fixed deploy Qt6 qml 
- Fixed powershell commands on Windows
- bug fixes and improvements.


### New Envirement Variables

- CQT_RUN_FILE - This variable contains path to the run script of the using application.
- CQT_PKG_ROOT - This variable contains path to root of the current package.

### New parameters

- zip - create a ZIP archive for deployable programs
- deb - Create the deb package for deployment programm
- homepage - Sets the homepage url for a package
- noQt - Ignors the error of initialize of a qmake. Use only if your application does not use the qt framework
- allowEmptyPackages -  Allows configure the empty packages.  
- getDefaultTemplate - Extracts defaults deb or qif templates.
- tr -  Adds qm files into the translations folder.
- extraData - Adds the extra files or directories like a target. The selected directory will be copy to the extraDataOut location with save own structure.
- extraDataOut - Sets path to extra data files out directory.
- prefix - Sets the prefix for the package relatively a target directory
- homePage - Sets the homepage url for a package
- binPrefix - Sets prefix for bin option.
- noHashSum - Disables campute hashes of packages

### Deprecated and removed options

- deploySystem-with-libc - instead of this option use the deploySystem option.
- allQmlDependes - instead of this option use the qmlDir option.

## CQtDeployer 1.5.0 Alpha

### Corrections

- bug fixes and improvements.

### New Features

- Added a new Theme for qif installer "quasarDark".

### New options

- zip - create the ZIP arhive for deployement programm


## CQtDeployer 1.4.7

### Corrections

- Fixed deploy the qxcb plugin. #438
- Fixed deploy the qtGui module. #446

### New Features

- Added support of Qt6 #437
- Added support of Qt installer Framework 4 (with improved command line interface experience) #436

## CQtDeployer 1.4.6

### Corrections

- Fixed deploy sql drivers #367
- Fixed algorithm of deploy libraryes. #423 #371
- Fixed working with a qt from the linux system repository. (classic instaler only) #422
- Fixed snap version of cqtdeployer, added support for qmake search by RPATH #424 #420
- Fixed help message.
- Minor bug fixes and improvements.
- Removed deprecated option extractPlugins. #371

### New options

- noQt - Ignore the error of initialize of a qmake. Use only if your application does not use the qt framework.

## CQtDeployer 1.4.5

### Corrections

- Optimized project deployment performance. 60% faster.
- Optimized output size of distributions. Size reduced by 25%.
- Fixed display of shortcuts after installing deployed applications.
- Fixed launching applications with spaces in the name. Thanks ahndee (#384)
- Fixed deployment of the Renderer plugin.
- Minor bug fixes and improvements.

### New Features

- Added a new deployment plugin management system.

### New options

- extraPlugin - sets an additional path to third-party application plug-in
- enablePlugins - enables additional plugins for distribution.
- disablePlugins - disables plugins for distribution.

## CQtDeployer 1.4.4

### New options
- qifFromSystem - force use system binarycreator tool of qif from path or qt.


## CQtDeployer 1.4.3
### Fixes
- Fixed display of the application downloaded from the snap store and installers in the ubuntu 20.04 application settings menu

### New features
- added support recursive ignore for ignoreEnvirement #356

### New options
- noRecursiveiIgnoreEnv - Disables recursive ignore for ignoreEnv option.



## CQtDeployer 1.4.2
### Fixes

- fixed #337 (error generate installer with custom name)
- fixed #338 (windows version collect pdb files)
- fixed #339 (error of show all help messages)
- fixed #340 (cqdeployer don't added a targets dirs into "ignore")
- fixed #341 (The clear option don't delete old installer)


## CQtDeployer 1.4.1

### Fixes
- Fixed Not deploying platformthemes (#324)
- Fixed create shortcuts on windows (#322)
- Minor improvements and fixes


## CQtDeployer 1.4.0
### Fixes
- Fixed The help output in the console, now the actual size of the console is recounted before the output, which allows you to correctly transfer text.
- Fixed work with the deployment of Qt plugins. Now plug-ins do not extract all system dependencies, but only qt. Extract system dependencies caused applications to crash due to incompatible plugin libraries.
- Minor bug fixes and improvements.

### New features
- Added support for qmake search from the system environment.
- Added the ability to initialize the repository for further packaging, similar to git init.
- Added support for Qt Install Framework packages. Now you can pack the distribution into the installer.
- Added the ability to split the final distribution into several packages.
- Added the ability to unify the creation of packages for the final distribution.
- Added support for adding custom scripts to application launch scripts.
- Added support for extracting system dependencies for Windows.
- Added support for RPATH for Linux. Now cqtdeployer can independently determine the necessary qmake to deploy the application.
- Added support for finding the required dependency by library name.
- Added support for Qt libraries from Linux distributions repositories.
- Added new alias for the run command (cqt and cqtdeployer.cqt) for fast deploy of applications.
- Added support the native name of command for windows. Now you can run a cqtdeployer from cqtdeployer commnad in cmd and powershell.

### New options
- init - will initialize cqtdeployer.json file (configuration file). For example: "cqtdeployer init" - to initialize the configuration of a base package. "cqtdeployer -init multi" - to initialize the configuration of several packages.
- noCheckRPATH - disables the automatic search for paths to qmake in executable files (Linux only).
- noCheckPATH - disables the automatic search for paths to qmake in the system environment.
- extractPlugins - forces to extract all plug-in dependencies.
- qif - creates an installer at the end of the deployment.
- extraLibs - adds a template for an additional library, which should be included in the distribution.
- customScript - adds a custom script to the startup script of the application.
- -targetPackage [package; tar1, package; tar2] - used to form packages, denotes lists of target files for specific packages.
- recOut - indicates in which folder the resources will be added after deployment.
- name - sets the name of the package.
- description - sets the package description
- deployVersion - sets the package version
- releaseDate -  sets the release date of the package.
- icon -  sets the package icon.
- publisher - sets the publisher of the package.
- qifStyle - Sets the path to the CSS style file or sets the default style. Available styles: quasar
- qifBanner - Sets path to the banner png file.
- qifLogo - Sets path to the logo png file.

## CQtDeployer 1.3.2
- Fixed #218 (The CQtDeployer adding sh scripts for target libraries.)
- Fixed #217 (The Qmake option did not work with the configuration file.)
---

## CQtDeployer 1.3.1
- Fixed a task #191 (ignore command for plugins)
- Fixed a task #192  (invalid paths when creating) 
- Fixed absalute pathes of deployement file (see [confFile](DeployConfigFile))
---

## CQtDeployer 1.3.0
- Added support of qt.conf for Linux
- Added support of config file for project (deploy.json) with all rules of deployment, for create deploy.json use flag -confFile [./path/to/file.json],
- Added support of output dirs for all deployement files #145,
- Added support of deploy WebEngine #146
- Added support multi package in cqtdeployer installer 
- Renamed project from Console-QtDeployer to CQtDeployer #139
- Fixed deployment of system libs in linux #172
- Added new tests of application
- Removed flag noLibC and added flag deploySystem-with-libc. If you need libc in the program, use the deploySystem-with-libc flag, and if not, use deploySystem.
- The always-overwrite flag has been replaced with noOverwrite, now all files are overwritten by default, use noOverwrite to disable overwriting.
---

## CQtDeployer 1.2.3
- Added support of qt.conf for Windows builds  #130 
- Fixed bug #129 now the 'clear' flag  remove only copied files.
- Fixed ignoreEnv flag 
- Fixed deploySystem flag
---

## CQtDeployer 1.2.2
- Added new flag noLibc for ignore libc and li-linux libs 
- Fixed bug #125 Deployed system lib (working with simple projects, for big projects no recommended)
- Fixed ignoreEnv flag 
---

## CQtDeployer 1.2.1
- Added new ignore flag (ignoreEnv). This flag disables unnecessary environment 
- Added support QIFW (online installer for windows and Other Linux distributives)
- Added support update for windows and Other Linux distributives (not snap)
- Fixed bug #116 (copy files from removable media)
- Fixed bug #115 (create target in folder with russean name) 
- Fixed big size of msvc version 
- FIxed crossdeploy Linux >> Windows
- Removeв old build scripts
---

## CQtDeployer 1.2.0
- Added own qml dependecies scaner.
- Moved to used a new [PE-Lib scaner](https://github.com/QuasarApp/pe-parse/tree/7138bb218a05c81d825728ba8a0a6f6a56c4fdbc).
- Moved to used a new [ELF-lib scaner](https://github.com/qt/qttools/blob/b0147a5d79ce7a7e77a24f36aaddfacb1a1517c8/src/shared/winutils/elfreader.h).
- Added a new unit tests.
- Added  support of snap version (home dir only).
- Added new flag qmlExtern - for use qt qml import scaner.
- Added support a levels of verbose log (0-3).
- Fixed log of deploy app.
- Fixed deploy the app on snap version.
---

## CQtDeployer 1.1.2
- Added support of translations
- Added a flag for disable deploy translations
- Added the strip flag on linux version
- Added unit tests
- Fixed deploy with shared version
- Fixed bug deploy on current dir with snap and static versions 
---

## CQtDeployer 1.1.1
- Fixed qml build in dinomisc version
- Fixed -recursiveDepth flag
- Fixed -libDir flag operation
- The output of the application has been cleaned, a part of the log has been moved to verbose
---

## CQtDeployer 1.1.0
- Added windows support (dynamic linking only)
- Added support for snap (now available in the snapstore)
- Added snap build scripts
- Added flag binDir allowing recursively to capture exe dll and so
- Changed the logic of the bin flag now, if you select a folder in the file location, then all its contents will be captured
- Changed and corrected the logic of the clear flag now it cleans all the expanded content on a specific target.
- Added flag targetDir which defines the place where the expanded project will lie
- The structure of the deployed project has been changed, now all the launch shells are in the root and the binaries are in the bin folder, a launch script of the same name is created for each binary.
- Removed old runScript flag due to change of deployment logic
- Added verbose flag for debugging by
- Fixed performance bugs
---

## CQtDeployer 1.0.2
- Added a script to build (dynamic and static release)
- Added flag -libDir to install additional path for additional libraries of the application
- Added flag -extraPlugin to install an additional path for additional application plugins
- Added build for C libraries only
- Fixed deployment
- Fixed startup script
- Fixed static build
---

## CQtDeployer 1.0.1
- fix qml deploy (redundancy)
- fix run script
---

## CQtDeployer 1.0.0
- Added a flag runScript
- Added a flag allQmlDependes
- Fixed bug of deploy qml
- Fixed bug of deploy plugins
- Added support static build 

