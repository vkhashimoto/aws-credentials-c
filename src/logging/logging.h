#ifndef LOGGING_H
#define LOGGING_H
#include <stdarg.h>

typedef enum {
    INFO = 1,
    WARN,
    ERROR,
    DEBUG,
    TRACE
} e_levels;

void LOG(char *msg);
void LOGL(int level, char *msg);
void LOGF(char *fmt, ...);
void LOGLF(int level, char *fmt, ...);
void _LOGLF(int level, char *fmt, va_list args);
#endif