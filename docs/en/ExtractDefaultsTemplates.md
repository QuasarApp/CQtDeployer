# Extracting default templates 

Sometimes you need to create your own templates for installers or deb packages.
Fot this you can use the getDefaultTemplate option. This option extracts your distribution's default template to the target directory. This option will generate a template according to your settings. If you use the targetPackage option then a template will be generated for each package you configured.

### Example:

``` bash 
cqtdeployer -bin myExecutable getDefaultTemplate qif
```
This command generate template for qt installer framework into DistributionKit folder for your "myExecutable" programm.

If you want change target location just add the targetDir option 
``` bash
cqtdeployer -bin myExecutable getDefaultTemplate qif -targetDir "/distanation/Dir"

``` 

## Work With multipackage distribution.

This option support the configFile.json 

### Example :

#### configFile.json 
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
}
```

```bash
cqtdeployer getDefaultTemplate qif -confFile "path/to/configFile.json"
```


## Note 
This option support work with all packages except of zip.
If you want get more information about available packages see the packing [page](Package.md).
