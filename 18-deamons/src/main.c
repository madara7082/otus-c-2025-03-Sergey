#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <syslog.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

struct config
{
    char file_path[256];
    char socket_path[256];
};

int sockfd;
struct config config;

// Обработчик сигналов для корректного завершения
void signal_handler(int sig) {
    if (sig == SIGTERM || sig == SIGINT) {
        syslog(LOG_INFO, "Демон завершает работу");
        close(sockfd);
        unlink(config.socket_path);
        closelog();
        unlink("/var/run/solution.pid"); // Удалить PID-файл
        exit(EXIT_SUCCESS);
    }
}

void trim_trailing_newline(char *str) 
{
    size_t len = strlen(str);
    while (len > 0 && (str[len-1] == '\n' || str[len-1] == '\r' || str[len-1] == ' ')) 
    {
        str[len-1] = '\0';
        len--;
    }
}

long int size_of_file(char *filepath)
{
    struct stat file_info;

    if (stat(filepath, &file_info) == -1) 
    {
        syslog(LOG_ERR, "Ошибка stat для файла %s: %s", filepath, strerror(errno));
        return -1;
    }

    return (long int)file_info.st_size;
}

int main(int argc, char *argv[]) {
    pid_t pid;
    struct sigaction sa;

    if(argc != 2)
    {
        printf("USAGE: %s <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *cmd = argv[0];      // Имя программы
    const char *cfg_file = argv[1];

    char buffer[256];
    FILE *fp = fopen(cfg_file, "r");
    if(fp)
    {
        for (int line=0; line<2; line++)
        {
            fgets(buffer, 256, fp);
            if (line == 0)
                strncpy(config.file_path, buffer, sizeof(config.file_path));
            if (line == 1)
                strncpy(config.socket_path, buffer, sizeof(config.socket_path));
        }
        trim_trailing_newline(config.file_path);
        trim_trailing_newline(config.socket_path);
        printf("%s \n", config.file_path);
        printf("%s \n", config.socket_path);
        fclose(fp);
    }
    else
    {
        printf("Ошибка открытия конфигурационного файла\n");
        return EXIT_FAILURE; 
    }

    // fork
    if ((pid = fork()) < 0) {
        perror("Ошибка fork");
        return EXIT_FAILURE;
    } else if (pid > 0) {
        exit(EXIT_SUCCESS); // Завершение родителя
    }

    // Создать новую сессию
    if (setsid() < 0) {
        perror("Ошибка setsid");
        return EXIT_FAILURE;
    }

    // Игнорировать SIGHUP
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0) {
        perror("Ошибка sigaction");
    }

    // Настройка демона
    umask(0);

    // Инициализация syslog
    openlog(cmd, LOG_CONS | LOG_PID, LOG_DAEMON);

    // Перенаправление стандартных дескрипторов
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    int null_fd = open("/dev/null", O_RDWR);
    if (null_fd != STDIN_FILENO) {
        syslog(LOG_CRIT, "Ошибка перенаправления stdin");
        exit(EXIT_FAILURE);
    }
    dup2(STDIN_FILENO, STDOUT_FILENO);
    dup2(STDIN_FILENO, STDERR_FILENO);

    // Запись PID-файла
    FILE *pidfile = fopen("/var/run/solution.pid", "w");
    if (pidfile) {
        fprintf(pidfile, "%d\n", getpid());
        fclose(pidfile);
    } else {
        syslog(LOG_ERR, "Не удалось создать PID-файл");
    }

    // Настройка обработчиков сигналов
    sa.sa_handler = signal_handler;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);

    syslog(LOG_INFO, "Демон %s успешно запущен", cmd);

        // Создание сокета
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        syslog(LOG_ERR, "Ошибка создания сокета: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, config.socket_path, sizeof(addr.sun_path) - 1);

    unlink(config.socket_path); // Удалить старый сокет если существует

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        syslog(LOG_ERR, "Ошибка привязки сокета: %s", strerror(errno));
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 5) < 0) {
        syslog(LOG_ERR, "Ошибка прослушивания: %s", strerror(errno));
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Главный цикл демона
    while (1) 
    {
        int client_fd = accept(sockfd, NULL, NULL);

        long int size = size_of_file(&config.file_path[0]);

        if (size >= 0)
        {
            char response[64];
            snprintf(response, sizeof(response), "%ld\n", size);
            write(client_fd, response, strlen(response));
        }
        else 
        {
            write(client_fd, "ERROR\n", sizeof("ERROR\n"));
        }
        close(client_fd);
    }

    return EXIT_SUCCESS; // Сюда никогда не попадём
}