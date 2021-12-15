#ifndef FLAGS_H
#define FLAGS_H
#define FLAGS_SIZE 2

typedef enum {
    profile,
    credential,

    COUNT
} e_flags;

void handle_flags(int argc, char *argv[], char *flags[]);
void handle_debug_flag(int argc, char *argv[]);

#endif