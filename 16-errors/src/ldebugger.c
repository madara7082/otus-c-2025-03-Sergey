#include "ldebugger.h"

static struct debug debugSession;

const char* logLevelToString (logLevel level)
{
    switch(level)
    {
        case DEBUG:     return "DEBUG";
        case INFO:      return "INFO";
        case WARNING:   return "WARNING";
        case ERROR:     return "ERROR";
        default:        return "UNKNOWN";
    }
}

void timeStr(char* buffer, size_t size) 
{
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

int debugInit(char* filename, logLevel level)
{
    debugSession.logFile = fopen(filename, "a");
    if (!debugSession.logFile)
    {
        return EXIT_FAILURE;
    }
    debugSession.globalLogLevel = level;
    char timeBuf[20];
    timeStr(timeBuf, sizeof(timeBuf));
    fprintf(debugSession.logFile, "\n--- [%s] Hello! This is the beginning of the debugging session! ---\n\n", timeBuf);
    return EXIT_SUCCESS;
}

void debugClose()
{
    if (debugSession.logFile)
    {
        char timeBuf[20];
        timeStr(timeBuf, sizeof(timeBuf));
        fprintf(debugSession.logFile, "\n--- [%s] End of debug session! ---\n", timeBuf);
    }
    fclose(debugSession.logFile);
}

void printBacktrace() 
{
    void* buffer[100];
    int frames = backtrace(buffer, 100);
    char** symbols = backtrace_symbols(buffer, frames);
    
    if (symbols) 
    {
        fprintf(debugSession.logFile, "Backtrace:\n");
        // Пропускаем первые 2 фрейма (эта функция и log)
        for (int i = 2; i < frames; i++) {
            fprintf(debugSession.logFile, "  #%d %s\n", i - 2, symbols[i]);
        }
        free(symbols);
    }
}

void logMessage(logLevel level, const char* file, int line, const char* func, const char* format, ...)
{
    if (level < debugSession.globalLogLevel
        || !debugSession.logFile)
    {
        return;
    }
    char timeBuf[20];
    timeStr(timeBuf, sizeof(timeBuf));

    char msg_buf[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(msg_buf, sizeof(msg_buf), format, args);
    va_end(args);

    fprintf(debugSession.logFile, "[%s] [%s] %s:%d (%s) - %s\n",
            timeBuf, logLevelToString(level), file, line, func, msg_buf);

    if (level == ERROR)
    {
        printBacktrace();
    }

    fflush(debugSession.logFile);
}
