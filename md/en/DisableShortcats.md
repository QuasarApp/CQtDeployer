# Disabling shortcuts

Disabling shortcuts for multi targets distribution Kit in the Qt Installer framework.

Some times we need to disable creating automaticly shortcuts for some applications.
For solve this problem i recomendet override the default qt installer framework template.

# Note 
Starting from CQtDeployer 1.5.1, you can use the -disableShortcuts option to disable the automatic creation of shortcuts.

### Example: 

```bash 
cqtdeployer -disableShortCut targetName,target2Name
```

```bash
cqtdeployer -bin myProgramm.exe -disableShortCut myProgramm
```
