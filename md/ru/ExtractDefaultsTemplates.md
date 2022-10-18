# Извлечение шаблонов

Извлечение шаблонов по умолчанию

Иногда вам нужно создать свои собственные шаблоны для установщиков или пакетов deb.
Для этого вы можете использовать опцию getDefaultTemplate.

Эта опция извлекает шаблон вашего дистрибутива по умолчанию в целевой каталог.

Эта опция создаст шаблон в соответствии с вашими настройками. Если вы используете опцию targetPackage, то для каждого настроенного вами пакета будет создан шаблон.

### Пример:

```bash
cqtdeployer -bin myExecutable getDefaultTemplate qif
```
Эта команда создает шаблон для среды установки qt в папке DistributionKit для вашей программы "myExecutable".

Если вы хотите изменить целевое местоположение, просто добавьте параметр targetDir
```bash
cqtdeployer -bin myExecutable getDefaultTemplate qif -targetDir "/distanation/Dir"

```

## Работа с мультипакетной раздачей.

Эта опция поддерживает configFile.json

### Пример:

#### configFile.json

```json
{

    "корзина": [
        "myClientExecutable",
        "myServerExecutable"
    ],

    "targetPackage": [
        ["Клиент", "myClientExecutable"],
        ["Сервер", "myServerExecutable"]
    ],
}
```

```bash
cqtdeployer getDefaultTemplate qif -confFile "path/to/configFile.json"
```


## Запись

Эта опция поддерживает работу со всеми пакетами, кроме zip.
Если вы хотите получить больше информации о доступных пакетах, смотрите упаковку [страница](Packing.md).