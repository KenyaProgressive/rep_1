#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <time.h>

typedef enum { INFO, ERR, WARNING } LogLevel;

void logger_launch(const char *filename);
void log_do(LogLevel Level, const char *message, ...);
void logger_close();

#endif