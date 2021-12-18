#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flags.h"
#include "logging/logging.h"


void handleLogFlags(int argc, char *argv[]) {
    int index;
    for (index = 1; index < argc; index++) {
        if (strcmp(argv[index], "--log") == 0 || strcmp(argv[index], "-l") == 0) {
            long logLevel = 1;
            char *ptr;
            if (argv[index +1] != NULL) {
                logLevel = strtol(argv[index + 1], &ptr, 10);
            }
            _setLogLevel(logLevel);
            LOGF("Logging activated with level: %d", logLevel);
        }
    }
}

void handleFlags(int argc, char *argv[], char *flags[]) {
    LOGLF(TRACE, "Executing handle_flags");
    int index;
    int has_profile = 0;
    int subtract_argc = 1;

    if (isLogLevelSet()) {
        subtract_argc = 3;
    }

    if ((argc - subtract_argc) % 2 != 0 || (argc - 1) == 0) {
        LOGLF(DEBUG, "%d arguments provided", argc);
        printf("Wrong number of arguments provided\n");
        printf("USAGE: ");
        /* TODO: Show usage */
        exit(EXIT_FAILURE);
    }
    
    for (index = 1; index < argc; index++) {
        LOGLF(DEBUG, "argv[%d] is %s", index, argv[index]);
        if (strcmp(argv[index], "-p") == 0) {
            LOG("Parsing profile");
            LOGLF(DEBUG, "Profile is: %s", argv[index + 1]);
            flags[profile] = argv[index+1];
            has_profile = 1;
        }
        if (strcmp(argv[index], "-c") == 0) {
            LOG("Parsing credentials");
            LOGLF(DEBUG, "Credentials is: %s", argv[index + 1]);
            flags[credential] = argv[index+1];
        }

        
    }

    if (!has_profile) {
        LOG("Profile was not provided. Using 'default'");
        flags[profile] = "default";
    }
}