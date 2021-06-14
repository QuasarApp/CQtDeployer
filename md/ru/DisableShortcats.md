# Отключение ярлыков для Qt Installer.

 Отключение ярлыков для многоцелевого дистрибутива в структуре Qt Installer.

Иногда нам нужно отключить автоматическое создание ярлыков для некоторых приложений.
Для решения этой проблемы я рекомендую переопределить шаблон фреймворка установщика qt по умолчанию.

# Note 
From CQtDeployer 1.5.1 you can add option -disableShortcuts for disable automation create shortcuts.

### Example: 

```bash 
cqtdeployer -disableShortCut targetName,target2Name
```

```bash
cqtdeployer -bin myProgramm.exe -disableShortCut myProgramm
```
