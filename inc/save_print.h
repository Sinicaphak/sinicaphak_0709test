#ifndef SAVE_PRINT_H
#define SAVE_PRINT_H

#include <pthread.h>

extern pthread_mutex_t log_mutex;

void save_printf(const char *format, ...);
void log_lock(bool lock, void *udata);

#endif