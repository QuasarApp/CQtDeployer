# Base packages

## Introduction

The **base package** it is package that contains all general files betwin all compared packages. 
For example you have a Qt application and you create 3 executable files (client server and terminal) and you want to create installer when all packages can be disabled.

### Trouble 

If you cteate 3 separete  package for each target then you get a dublicate of the qt libraries because all executable files dependet of the qt. 

### Solution 

Is a create a base package with a shared libraries for separete pacakge and creating dependenses to the base packages for all dependet packages.

By default CQtDeployer try create a base papckage for all packages of your distribution. If you don't want to create base package and want to create a separated independet packages you should be use the **noBase** option.

The **NoBase** option force cqtdeployer disable generate a base package. 

## Manual control of the base packages

Some times we create a very big Distributions with the many targets and one base package may by not enough. So for this case the CQtDeployer have the **basePackage** option.

the **BasePackge** option creats a base package for selected list of packages. 

#### Example:

We want to create 2 base pacages.

We have a 5 targets:
* terminal
* server
* clientGame
* cleintMapEditor
* clientDocumentation

We make 3 packages like this :
* BackEnd (include terminal and server)
* FrontEnd (include clientGame and clientDocumentation)
* GameModer (include only cleintMapEditor)

So the *GameModer* and *FrontEnd* packages has a many general libs because its work with client libraries and *BackEnd* have only own libraries and general Qt libraries with the  *FrontEnd* and *GameModer*.

For solve this task we need to create a next dependet tree:

```
BasePackage---->BaseFrontEnd pacakge---->FrontEnd pacakge
           \                        \
            \                        --->GameModer package
             \
              --->BackEnd pacakge
```

So that we nee to create 2 base packages BaseFrontEnd and BasePackage.
The **BasePackage** create automaticly because we don't use the **noBase** option.

### CQtDeployer command :

```
cqtdeployer -targetPackage BackEnd;terminal;server,FrontEnd;clientGame;clientDocumentation,GameModer;cleintMapEditor -bin terminal,server,clientGame,cleintMapEditor -extraTarget FrontEnd;clientDocumentation -basePackage BaseFrontEnd;GameModer;FrontEnd qif
```

So This command create our tree. but is very hard command... 
Еo ease the task, I suggest using the configuration file instead of one command in the console.

### CQtdeployer.json
```json
{
    "targetPackage": [
        ["BackEnd", "terminal", "server"],
        ["FrontEnd", "clientGame", "clientDocumentation"],
        ["GameModer", "cleintMapEditor"]

    ],
    "bin": [
        "terminal",
        "server",
        "clientGame",
        "cleintMapEditor"

    ],
    "extraTarget": [
        ["FrontEnd", "clientDocumentation"]
    ],
    "basePackage": [
        ["BaseFrontEnd", "GameModer", "FrontEnd"]
    ],
    "qif": true
}
```

And run commmand with the confFile

``` bash
cqtdeployer -confFile CQtdeployer.json
```

Done.