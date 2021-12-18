#include <stdarg.h>
#include <stdio.h>
#include "logging.h"

int _logLevel = 0;

void _setLogLevel(int logLevel) {
    _logLevel = logLevel;
}

int isLogLevelSet() {
    return _logLevel != 0;
}

void LOG(char *msg) {
    LOGL(INFO, msg);
}

void LOGL(int level, char *msg) {
    if (level <= _logLevel) {
        printf("%s\n", msg);
    }
}

void LOGF(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    _LOGLF(INFO, fmt, args);
    va_end(args);
}


void LOGLF(int level, char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    _LOGLF(level, fmt, args);
    va_end(args);

}

void _LOGLF(int level, char *fmt, va_list args) {
    if (level <= _logLevel) {
        vprintf(fmt, args);
        printf("\n");
    }
    va_end(args);

}