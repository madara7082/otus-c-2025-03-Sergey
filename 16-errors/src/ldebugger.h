#ifndef __LDEBUGGER_H
#define __LDEBUGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <time.h>
#include <execinfo.h>
#include <stdarg.h>

typedef enum 
{
    DEBUG,
    INFO,
    WARNING,
    ERROR
} logLevel;

struct debug
{
    FILE*       logFile;
    logLevel    globalLogLevel;
};

/**
 *  @brief      Функция преобразует уровень отладки в строку
 *  @param      level enum уровня debug
 *  @return     Возвращает строку уровня логирования
 */
const char* logLevelToString (logLevel level);

/**
 *  @brief      Функция создает файл для записи логов и пишет приветствие
 *  @param      filename название файла логов
 *  @param      level уровень логирования сообщений
 *  @return     Указатель на созданный файл или NULL в случае ошибки
 */
int debugInit(char* filename, logLevel level);

/**
 *  @brief      Функция пишет конечное сообщение и закрывает файл
 */
void debugClose();

/**
 *  @brief      Функция пишет в файл стек вызовов
 *  @param      file указатель на открытый файл
 */
void printBacktrace();

/**
 *  @brief      Функия логирования
 */
void logMessage(logLevel level, const char* file, int line, const char* func, const char* format, ...);

#define LOG_DEBUG(...) logMessage(DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define LOG_INFO(...)  logMessage(INFO,  __FILE__, __LINE__, __func__, __VA_ARGS__)
#define LOG_WARNING(...)  logMessage(WARNING, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define LOG_ERROR(...) logMessage(ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__)

#endif /* __LDEBUGGER_H */