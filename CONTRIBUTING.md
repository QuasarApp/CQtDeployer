# QuassarApp operation rules
## General rules
### QuasarApp internal developers
* if there is no instruction to complete the task, separate you new branch from the version branch (for example v1.x).
* Realize the task
* At the end of the work, create a pull request to the branch from which you created the branch.
Example:
I made corrections for version 1.5 and created a new branch from branch v1.5, which means that I should create a pull request to branch v1.5,

### External QuasarApp Developers
* Making Fork repository.
* Switch to the branch of the version in which you need to perform the correction (example of the names of the branch with the version: v1.x).
* Realize the task
* Upon completion, create a Pull Request in the branch from which they are separated.
   Example:
I made corrections for version 1.5 and created a new branch from branch v1.5, which means that I should create a pull request to branch v1.5,
* ( not necessary ) Before starting work on the Task, drag it to [inProgress](https://github.com/orgs/QuasarApp/projects) in projects

### Forbidden

* Using 'push force'. All conflicts need to be solved by a new commite.
* Push code directly to master.
* Break the code design rules. If some aspect is not described, you need to write in the same style as it was written before you.
* Move the code for no particular reason.


## Making Pull Request

The content of the pull request must include:
* The number of the task that solves (if performed according to the task)
* A complete description of everything that was done in the task.
* In the case when a pull creation is created and you still work on the task, change the header of the pool request by adding a WIP tag to the beginning (example [WIP] MyTask)
* Pull Request must always be assigned to the branch from which you separated.

## Making Tasks
If necessary, assign a task to someone
You must:
* Create relevant discussion on github, selected repository.
* Completely describe the problem or task.
* If you have a solution to the problem fully describe what and how to do.

## Code Guideline

When writing code follows the following rules: (inscribed written in order of importance)
1. If in order to achieve high performance gains (over 10%) you need to sacrifice any of the rules, donate them.
2. In no case do not use the C-style Cast.
3. All connected headers should be stored to the maximum in cpp files.
4. Use fast forward declaration. If pointers are used in the class, then initialize the prototypes of these classes as follows: class a; a* value = nullptr; without connecting the headers of the types of indicators.
5. In headings it is necessary to null the signs.
6. Template functions are described in cpp files.
7. If possible, think through your code so that it does not have a cast.
8. Write class access specifiers in the following order: public, public slots, siganls, protected, protected slots, private, private slots. 
9. Carefully check and arrange spaces between operators in the code.
10. Moving the bracket to the next line is prohibited.
11. When transferring the shift should be equal to 4 spaces.
12. Before pushing the code, be sure to run the tests.

# QuassarApp правила работы
## Основные правила
### QuasarApp внутренние разработчики
* Если нет инструкции для выполнения задачи, отделите новую ветку от ветви версии (например, v1.5).
* Реализовать задачу
* В конце работы создайте пул-запрос к ветке, из которой вы создали ветку.
Пример:
Я внес исправления для версии 1.5 и создал ветвление из ветви v1.5, что означает, что я должен создать запрос на извлечение в ветке v1.5,

### Внешние разработчики QuasarApp
* Создание Fork репозитория.
* Переключитесь на ветку версии, в которой нужно выполнить исправление (пример названий ветки с версией: v1.x).
* Реализовать задачу
* По завершении создайте Запрос на извлечение в ветке, от которой они отделены.
   Пример:
   Я внес исправления для версии 1.5 и создал ветвление из ветви v1.5, что означает, что я должен создать запрос на извлечение в ветке v1.5,
* (не обязательно) Перед началом работы над задачей перетащите ее на [inProgress] (https://github.com/orgs/QuasarApp/projects) в проектах

### Запрещено работать в QuassarApp

* Использовать push force. Все проблемы должны быть решены новым комитом.
* Пушить код на прямую в мастер или другую ветку релиза.
* Нарушать правила оформления кода. Если какой-то аспект не описан, вам нужно писать в том же стиле, в котором он был написан до вас.
* Переместить код без особой причины.


## Выполнение запроса на слияние

Содержание pool request должно включать:
* Номер задачи, которая решается (если выполняется согласно задаче)
* Полное описание всего, что было сделано в задании.
* В случае создания Poll request и задача все еще выполняется, изменить заголовок запроса на извлечение, добавив в начало тег WIP (пример [WIP] MyTask)
* Запрос на извлечение всегда должен быть назначен ветви, от которой вы отделены.

## Выполнение задач
При необходимости назначьте кому-нибудь задачу
Вы должны:
* Создайте соответствующую дискуссию на GitHub, выбранном хранилище.
* Полностью опишите проблему или задачу.
* Если у вас есть решение проблемы, полностью опишите, что и как делать.

## Регистрация кода

При написании кода соблюдайте следующие правила: (надписи пишутся в порядке важности)
1. Если для достижения более высокой производительности (более 10%) вам необходимо пожертвовать каким-либо из правил, пожертвуйте ими.
2. Ни в коем случае не используйте C-стиль Cast.
3. Все подключенные заголовки должны быть максимально сохранены в файлах cpp.
4. Используйте fast forward declaration. Если в классе используются указатели, то инициализируйте прототипы этих классов следующим образом: class a; a* значение = nullptr; без подключения заголовков типа созданного обьекта.
5. В заголовках необходимо обнулить знаки.
6. Функции шаблона описаны в файлах cpp.
7. Если возможно, продумайте свой код так, чтобы в нем не было приведений.
8. Напишите спецификаторы доступа к классам в следующем порядке: общедоступные, общедоступные, защищенные, защищенные, частные, частные.
9. Тщательно проверьте и расставьте пробелы между операторами в коде.
10. Перемещение скобки на следующую строку запрещено.
11. При переносе смена должна быть равна 4 пробелам.
12. Прежде чем пушить код, обязательно запустите тесты.
