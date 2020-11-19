# Общая информация об упаковке ваших дистрибутивов в пакеты.

## Типы упаковки
* Автоматически
* Вручную

## Автоматические пакеты
Пакеты с этим типом создаются автоматически.
По умолчанию в дистрибутиве есть один основной пакет. Вы можете настроить его, используя [Параметры управления пакетами](Options.md).

#### Пример (Установить новое имя для pacakge)
```
ctdeployer ... -name "MyPackage"
```

Если в вашем дистрибутиве есть несколько исполняемых файлов, вы можете создать дистрибутив с несколькими пакетами.

Один пакет может содержать от 1 до бесконечности исполняемых файлов.

Для настройки пакета необходимо использовать параметр targetPackage.

Пример использования:

```
cqtdeployer ... -targetPackage myPacage; target1, myPacage2; target2
```

Где myPacage - это имя пакета, а target1 - имя исполняемого файла.
Итак, в этом примере создается 2 пакета из 2 целей.

В следующем примере создается 2 пакета из 3 целей.
```
cqtdeployer ... -targetPackage myPacage; target1, myPacage; target2, myPacage2; target3
```


Вы также можете сделать то же самое в файле конфигурации

```
{
"targetPackage": [
        [
            "myPacage",
            "target1"
        ],
        [
            "myPacage2",
            "target2"
        ]
    ]
}
```

```
{
"targetPackage": [
        [
            "myPacage",
            "target1"
        ],
        [
            "myPacage",
            "target2"
        ],
        [
            "myPacage2",
            "target3"
        ]
    ]
}
```


## Ручные пакеты
Ручные пакеты - это пакеты, созданные по шаблону пользователя.
Например, у вас есть приложение, которое должно выполнять сложный сценарий в пакете debian или программе установки.
Итак, вам нужно создать свой шаблон и добавить путь к вашему шаблону для варианта пакета.

пример

```
cqtdeployer ... -qif /path/to/my/template/installer
```

Папка  /path/to/my/template/installer должна содержать папку с именем пакета.
например, я развертываю приложение с именем "MyExample", поэтому мой настраиваемый шаблон должен быть расположен в
 /path/to/my/template/installer/MyExample путь.

Для многопакетных дистрибутивов применяются те же правила.

Ваша папка с шаблонами должна содержать папки с такими же названиями, что и пакеты.

Пример:

```
{
"targetPackage": [
        [
            "myPacage",
            "target1"
        ],
        [
            "myPacage",
            "target2"
        ],
        [
            "myPacage2",
            "target3"
        ]
    ]
}
```

```
~/path/to/my/template/installer $ tree
.
├── myPacage
│ └── ...
└── myPacage2
    └── ...

```

## Доступные пакеты
* [Qt Install Framework](https://github.com/QuasarApp/CQtDeployer/wiki/QIF(en))
* [ZIP-архив](https://github.com/QuasarApp/CQtDeployer/wiki/ZIP(en))
* [Пакет DEB](https://github.com/QuasarApp/CQtDeployer/wiki/DEB(en))
