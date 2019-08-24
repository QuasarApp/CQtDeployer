# QuassarApp operation rules
## General rules
### QuasarApp internal developers
* If there is no indication, separate from the master branch.
* We realize the task
* Upon completion of work, issue a Pull Request to the master.

### External QuasarApp Developers
* Making Fork repository.
* If there is no indication, separate from the master branch.
* We realize the task
* Upon completion of work, issue a Pull Request to the master.
* Before starting work on the Task, drag it to [inProgress](https://github.com/orgs/QuasarApp/projects) in projects

### Prohibited from working in QuassarApp

* Use push force. All problems need to be solved by a new committee.
* Push code directly to master.
* Break the code design rules. If some aspect is not described, you need to write in the same style as it was written before you.
* Move the code for no particular reason.


## Making Pull Request

The content of the pull request must include:
* The number of the task that solves (if performed according to the task)
* A complete description of everything that was done in the task.
* In the case when a pull creation is created, the Task is still running, marking the Pull Request header with a WIP tag (example [WIP] MyTask)
* Pull Request must always be assigned to the branch from which you separated.

## Making Tasks
If necessary, assign a task to someone
You must:
* Create relevant discussion on github, selected repository.
* Completely describe the problem or task.
* If you have a solution to the problem fully describe what and how to do.

## Code Registration

When writing code follows the following rules: (inscribed written in order of importance)
1. If in order to achieve high performance gains (over 10%) you need to sacrifice any of the rules, donate them.
2. In no case do not use the C-style Cast.
3. All connected headers should be stored to the maximum in cpp files.
4. If the class uses pointers, then initialize the prototypes of these classes to the place where the header is connected: class a; a * value = nullptr;
5. In headings it is necessary to null the signs.
6. Template functions are described in cpp files.
7. If possible, think through your code so that it does not have a cast.
8. Write class access specifiers in the following order: private, private slots, protected, protected slots, public, public slots, signals
9. Carefully check and arrange spaces between operators in the code.
10. Moving the bracket to the next line is prohibited.
11. When transferring the shift should be equal to 4 spaces.
12. Before pushing the code, be sure to run the tests.

# Правила работы QuassarApp
## Общие правила
### Внутренние разработчики QuasarApp
* Если нет никаких указаний отделяемся от ветки master
* Реализуем поставленную задачу 
* По завершению работы оформить  Pull Request в master.

### Внешние разработчики QuasarApp
* Делаем Fork репозитория.
* Если нет никаких указаний отделяемся от ветки master
* Реализуем поставленную задачу 
* По завершению работы оформить Pull Request в master.
* Перед началом работы над Задачей, перетащить ее в [inProgress](https://github.com/orgs/QuasarApp/projects) в проектах

### При работе в QuassarApp запрещается

* Использовать push force. Все проблемы нужно решать новым комитом.
* Пушить код напрямую в master.
* Нарушать правила оформления кода. Если какой то аспект не описан, писать нужно в том-же стиле как было написано до вас.
* Перемещать код без особой на это причины. 


## Оформление  Pull Request

Содержимое Pull Request должно обязательно включать:
* Номер задания которую решает (если выполнялось по поставленной задаче)
* Полное описание всего что было сделано в задаче.
* В случае когда создается пулреквест а Задача еще выполняется помечать заголовок Pull Request тегом WIP (пример [WIP] MyTask)
* Pull Request всегда должен быть назначен именно в ту ветку от которой вы отделялись.

## Оформление Задачи
В случае необходимости назначить на кого-то задачу
Вы должны:
* Создать соответствующее обсуждение на github, выбранного репазитория.
* Полностью описать проблему или задачу.
* Если у вас есть решение задачи полностью описать что и как делать.

## Оформление кода

При написании кода придерживается следующих правил: (привила записаны в порядке важности)
1. Если ради достижения высокого прироста производительности (более 10%) вам нужно пожертвовать каким либо из правил, жертвуйте ими.
2. Не в коем случае не использовать C-style Cast.
3. Все подключаемые заголовки по максимуму должны хранится в cpp файлах.
4. Если в классе используются указатели то инициализировать прототипы этих классов в место подключения заголовка: сlass a; a *value = nullptr;
5. В заголовках обязательно занулять указатели.
6. Шаблонные функции описывать в cpp файлах.
7. По возможности продумывать ваш код так что бы в нем не было cast.
8. Спецификаторы доступа класса писать в следующем порядке private, private slots, protected, protected slots, public, public slots, signals
9. Внимательно проверять и расставлять пробелы между операторами в коде.
10. Перенос скобки на следующую строку запрещается. 
11. При переносе сдвиг должен быть равен 4 пробела.
12. Перед пушем кода обязательно прогнать тесты.
