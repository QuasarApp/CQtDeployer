# Базовые пакеты

## Введение

**Базовый пакет** - это пакет, который содержит все общие файлы между всеми сравниваемыми пакетами.
Например, у вас есть приложение Qt, и вы создаете 3 исполняемых файла (клиентский сервер и терминал), и вы хотите создать установщик, когда все пакеты можно отключить.

### Проблема

Если вы создаете 3 отдельных пакета для каждой цели, вы получите дубликат библиотек qt, потому что все исполняемые файлы зависят от qt.

### Решение

Это создание базового пакета с общими библиотеками для отдельных пакетов и создание зависимостей от базовых пакетов для всех зависимых пакетов.

По умолчанию CQtDeployer пытается создать базовый пакет для всех пакетов вашего дистрибутива. Если вы не хотите создавать базовый пакет и хотите создать отдельные независимые пакеты, вам следует использовать параметр **noBase**.

Параметр **NoBase** заставляет cqtdeployer отключить создание базового пакета.

## Ручное управление базовыми пакетами

Иногда мы создаем очень большие дистрибутивы с множеством целей, и одного базового пакета может быть недостаточно. Так что для этого случая у CQtDeployer есть опция **basePackage**.

опция **BasePackge** создает базовый пакет для выбранного списка пакетов.

#### Пример:

Мы хотим создать 2 базовых пакета.

У нас есть 5 целей:
* terminal
* server
* clientGame
* cleintMapEditor
* clientDocumentation

Делаем 3 таких пакета:
* BackEnd (включая terminal и server)
* FrontEnd (включая clientGame и clientDocumentation)
* GameModer (включает только cleintMapEditor)

Таким образом, пакеты *GameModer* и *FrontEnd* имеют много общих библиотек, потому что они работают с клиентскими библиотеками, а *BackEnd* имеет только собственные библиотеки и общие библиотеки Qt с *FrontEnd* и *GameModer*.

Для решения этой задачи нам необходимо создать следующее дерево зависимостей:

```
BasePackage---->BaseFrontEnd pacakge---->FrontEnd pacakge
           \                        \
            \                        --->GameModer package
             \
              --->BackEnd pacakge
```

Так что нам нужно создать 2 базовых пакета BaseFrontEnd и BasePackage.
**BasePackage** создается автоматически, потому что мы не используем параметр **noBase**.

### Команда CQtDeployer:

```
cqtdeployer -targetPackage BackEnd;terminal;server,FrontEnd;clientGame;clientDocumentation,GameModer;cleintMapEditor -bin terminal,server,clientGame,cleintMapEditor -extraTarget FrontEnd;clientDocumentation -basePackage BaseFrontEnd;GameModer;FrontEnd qif
```

Итак, эта команда создаст наше дерево. но это очень жесткая команда ...
Для облегчения задачи предлагаю использовать конфигурационный файл вместо одной команды в консоли.

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

И запустите команду с confFile

``` bash
cqtdeployer -confFile CQtdeployer.json
```

Выполнено. 