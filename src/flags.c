#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flags.h"
#include "logging/logging.h"


void handle_debug_flag(int argc, char *argv[]) {
    int index;
    for (index = 1; index < argc; index++) {
        if (strcmp(argv[index], "--debug") == 0) {
            set_debug();
            DEBUG("Debug is activated");
        }
    }
}

void handle_flags(int argc, char *argv[], char *flags[]) {
    int index;
    int has_profile = 0;
    int subtract_argc = 1;

    if (get_debug()) {
        subtract_argc = 2;
    }

    if ((argc - subtract_argc) % 2 != 0 || (argc - 1) == 0) {
        /* TODO: Show usage */
        printf("Wrong number of arguments\n");
        DEBUGF("argc: %d", argc);
        DEBUGF("It has to be an even number of arguments");
        exit(EXIT_FAILURE);
    }
    
    for (index = 1; index < argc; index++) {
        DEBUGF("argv[%d] is %s", index, argv[index]);
        if (strcmp(argv[index], "-p") == 0) {
            DEBUG("Adding the profile to the flags.");
            flags[profile] = argv[index+1];
            has_profile = 1;
        }
        if (strcmp(argv[index], "-c") == 0) {
            DEBUG("Adding credential to the flags.");
            flags[credential] = argv[index+1];
        }

        
    }

    if (!has_profile) {
        /* printf("Using default profile\n"); */
        DEBUG("Using default profile");
        flags[profile] = "default";
    }
}