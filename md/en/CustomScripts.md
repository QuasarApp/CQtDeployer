# Custom Scripts

After version 1.5.1 cqtdeployer support self variables in the custom scripts.
For using custom scripts use the **runScript option**

### Exmaples :

```bash
cqtdeployer -bin MyExecutable.exe -runScript MyExecutable,path/to/my/custom/script.bat 
```

```bash
cqtdeployer -bin MyExecutable -runScript MyExecutable,path/to/my/custom/script.sh 
```

## Suported variables list:

* CQT_LIB_PATH - are releative path to libraryes of a deployed distribution.
* CQT_QML_PATH - are releative path to qml libraryes of a deployed distribution.
* CQT_PLUGIN_PATH - are releative path to qt plugins of a deployed distribution.
* CQT_BIN_PATH - are releative path to targets of a deployed distribution.

* CQT_SYSTEM_LIB_PATH - are releative path to system libraryes of a deployed distribution.
* CQT_BASE_NAME - are base name of the executable that will be launched after run this script.
* CQT_CUSTOM_SCRIPT_BLOCK - This is code from the customScript option
* CQT_RUN_COMMAND - This is command for run application. Requred BASE_DIR variable.
