Необходимые packages:
Debian/Ubuntu: libglew-dev libglm-dev libglfw3-dev
Команды для установки:
$ sudo apt-get install libglew-dev
$ sudo apt-get insrall libglm-dev
$ sudo apt-get install libglfw3-dev

Сборка и запуск проекта:
$ make
$ ./main

Управление:
На кнопку 2 включается визуализация буфера глубины
На кнопку 1 включается обратно визуализация сцены

Реализовано - баллы:
База        - 10
Карты теней - 10

Сумма: 20

Примечание: ввиду не совсем корректно написанного Makefile
при каких-либо изменениях в исходном коде рекомендуется
полная пересборка проекта:
make clean
make
