# Adding extraData into distributive.

CQtDeployer has support to work with extra data files. 

For this you should use the -extraData and -extraDataOut options. 

## Examples:

### Add custom file into root directory of the your distribution.

```
cqtdeployer -extraData path/to/my/customFile
```

Or in configFile.json

```json
{
    ...
    "extraData": "./path/to/my/customFile"
    ...
}
```

This command support list of arguments.


``` bash
cqtdeployer -extraData path/to/my/customFile,path/to/my/customFile2,path/to/my/customFile3
```

Or in configFile.json

```json
{
    ...
    "extraData": [
        "./path/to/my/customFile",
        "./path/to/my/customFile2",
        "./path/to/my/customFile3"
    ]
    ...
}
```

### Changing location of extra data files.

For change location you should use  the extraDataOut options. This option change destination path based on the root of the distributive.

### Example. Add custom scripts into scripts folder in the Distribution.

``` bash
cqtdeployer -extraData /path/to/myScripts.sh,/another/path/to/myScripts.sh -extraDataOut scripts
```

Or in configFile.json

```json
{
    ...
    "extraData": [
        "./path/to/myScripts.sh",
        "./another/path/to/myScripts.sh"
    ],
    "extraDataOut": "scripts"
    ...
}
```

## Packages
extraData and extraDataOut options have support to work with packages. See the packaging [page](Pacakges.md) to get more information about packages.

Sometimes we need to create a multipackages application. For example create a installer for client-server application with the client and server applications. The client and server have a own default configuration files. We add config files using the extraData option.

For create a big distribution i recommend use the [confFile](DeployConfigFile.md).

## Example multipacking application.


```json
{

    "bin": [
        "myClientExecutable",
        "myServerExecutable"
    ],

    "targetPackage" : [
        ["Client", "myClientExecutable"],
        ["Server", "myServerExecutable"]
    ],

    "extraData": [
        ["Client", "./path/to/clientConfig.conf"],
        ["Server", "./path/to/serverConfig.conf"]
    ],
    "extraDataOut": [
        ["Client", "defaultConfigurations"],
        ["Server", "defaultConfigurations"]
    ]
}
```

This config create a distribution with two appications (client and server). Each application have a own configuration file in the defaultConfigurations folder.
