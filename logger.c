#include "logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static FILE *log_file = NULL;

void logger_launch(const char *filename)
{
    if (log_file != NULL)
    {
        logger_close();
    }
    log_file = fopen(filename, "a");
    if (log_file == NULL)
    {
        fprintf(stderr, "Failed to create log file: %s\n", filename);
    }
    else
    {
        log_do(INFO, "Logger initialized. Log file: %s", filename);
    }
}

void log_do(LogLevel level, const char *message, ...)
{
    if (log_file == NULL)
        return;

    const char *level_str[] = {"INFO", "ERROR", "WARNING"};
    time_t now = time(NULL);
    struct tm *local_time = localtime(&now);

    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%T", local_time);

    va_list args;
    va_start(args, message);
    fprintf(log_file, "[%s] [%s] ", timestamp, level_str[level]);
    vfprintf(log_file, message, args);
    fprintf(log_file, "\n");
    va_end(args);

    fflush(log_file);
}

void logger_close()
{
    if (log_file != NULL)
    {
        log_do(INFO, "Logger closed.");
        fclose(log_file);
        log_file = NULL;
    }
}
