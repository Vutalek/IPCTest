# IPCTest
Создана во время стажировки в СХД Baum.

С помощью данной утилиты можно сравнить скорости межпроцессного взаимодействия разными методами. На данный момент реализовано 4 метода:
- fifo - именованный канал;
- socket - сокеты;
- pipe - неименованный канал;
- shm - общая память.

Утилита может работать как в качестве сервера, так и в качестве клиента.

Клиент подключается к серверу и осуществляет передачу указанного клиенту файла на сервер. Сервер же принимает данные и сохраняет их в файл, указанный серверу. Если сервер не находит файл, то он создаёт новый.
По завершение передачи, клиент, вычислив размер переданного файла в байтах и затраченного времени в секундах, считает среднюю пропускную способность в Мбайт/с. Затем он выводит эту информацию на экран.

## Запуск

После компиляции, исполняемый файл находится в папке `bin`.

`./ipc <роль> <метод> <путь до файла>`

Роли:
- server;
- client.
 
Методы:
- fifo;
- socket;
- pipe;
- shm.

Рекомендуется перед передачей файла, первым запускать сервер.

## Серверная часть

После завершения передачи клиентом файла, на экране сервера будет примерно следующее

```
Server opened.
Transmission with client 1234 ended.
Close server?[y/n]: 
```

Сервер спросит, завершить ли его работу. При отправке символа `y`, сервер благополучно завершит свою работу, закрыв все соединения и файловые дескрипторы.

При отправке `n`, сервер переоткроет(!!!) указанный файл, стерев то, что было записано ранее и будет ждать сообщения от клиента.

Сервер может обслуживать до 20 разных клиентов.

На данный момент функция `n` сервера некорректно работает с общей памятью.

## Клиентская часть

Примерный вывод работы клиента:

```
Connection established.
Client: 1234
File size: 21648 Bytes
General time: 0.000196 sec
Bandwidth: 110.449 MBytes/sec
Connection closed.
```

## Замечание о неименованном канале

Так как неименованный канал может работать только между родительским и дочерним процессами, то запуск происходит только в одном терминале, неважно с какой ролью `server` или `client`.
Клиент передаёт указанный файл, а сервер записывает в файл `<имя файла>copy`, причём copy вставляется просто в конец, так что если файл содержит расширение, то оно будет испорчено, впрочем данная проблема решается переименовыванием файла на сервере.

В таком случае вывод программы будет выглядеть следующим образом:

```
Server opened.
Connection established.
Client 1234.
File size: 221986 Bytes
General time: 0.002691
Bandwidth: 82.492 Mbytes/sec
Connection closed.
Transmission with client 1234 ended.
Server closed.
```

## Зависимости

Утилитой можно пользоваться исключительно на unix-like системах.

Необходимые зависимости:
- make.

## Установка

Для компиляции проекта необходимо открыть терминал в папке репозитория и написать команду

`make`

Для удаления результатов компиляции в терминал необходимо написать команду

`make clean`

## Опциональные настройки

### Имя компилируемой утилиты

Для изменения имени утилиты после компиляции, необходимо изменить `Makefile`. Желаемое имя необходимо указать в переменную `TARGET`.

### Размер блока данных

Клиент отправляет серверу файл поблочно. По умолчанию это 512 байт. Чтобы изменить размер блока данных, необходимо поменять константу `DATA_BLOCK` в файле `code/IPCTest.cpp`.

### Название сервера

Во многих методах фигурирует название сервера. По умолчанию это `ipc_server`. Чтобы это поменять, необходимо изменить константу `SERVER_NAME` в файле `code/IPCTest.cpp`

### Режим отладки

В файле `code/IPCTest.cpp` есть константа `DEBUG_MODE`.

Если `DEBUG_MODE` выставлен в 0, то никаких дополнительных сообщений не возникает.

Если же `DEBUG_MODE` выставлен в 1, то клиент будет выводить дополнительную информацию.
А именно:
- сколько байт он отправил;
- ответ сервера.

Выглядит это примерно так:

```
Send 512 bytes.
Server received 512 bytes from 1234.

Send 512 bytes.
Server received 512 bytes from 1234.
```
