#ifndef LOGGING_H
#define LOGGING_H

void DEBUG(char *msg);
void DEBUGF(char *fmt, ...);
void set_debug();
int get_debug();

#endif