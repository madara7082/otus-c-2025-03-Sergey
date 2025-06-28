Компилятор gcc

Для сборки и запуска:
1. make
2. ./solution "файл конфигурации"

Убить демон:
sudo kill $(cat /var/run/solution.pid)

Связь:
nc -U /tmp/solution.sock
