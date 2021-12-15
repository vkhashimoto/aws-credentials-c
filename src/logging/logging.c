#include <stdarg.h>
#include <stdio.h>
#include "logging.h"

int debug = 0;

void set_debug() {
    debug = 1;
}

int get_debug() {
    return debug;
}

void DEBUG(char *msg) {
    if (get_debug()) {
        printf("%s\n", msg);
    }
}

void DEBUGF(char *fmt, ...) {
    if (get_debug()) {
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        printf("\n");
        va_end(args);
    }
}
