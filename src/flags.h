#ifndef FLAGS_H
#define FLAGS_H
#define FLAGS_SIZE 2

typedef enum {
    profile,
    credential,

    COUNT
} e_flags;

void handleLogFlags(int argc, char *argv[]);
void handleFlags(int argc, char *argv[], char *flags[]);

#endif