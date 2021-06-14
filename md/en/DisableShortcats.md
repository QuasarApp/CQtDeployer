# Disabling shortcuts

Disabling shortcuts for multi targets distribution Kit in the Qt Installer framework.

Some times we need to disable creating automaticly shortcuts for some applications.
For solve this problem i recomendet override the default qt installer framework template.

# Примечание
В CQtDeployer 1.5.1 вы можете добавить опцию -disableShortcuts для отключения автоматизации создания ярлыков. 

### Пример: 

```bash 
cqtdeployer -disableShortCut targetName,target2Name
```

```bash
cqtdeployer -bin myProgramm.exe -disableShortCut myProgramm
```
