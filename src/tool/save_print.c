#include "main.h"
// 线程安全的printf与日志输出

// 日志锁 兼 printf锁
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

void log_lock(bool lock, void *udata) {
    if (lock) {
        pthread_mutex_lock(&log_mutex);
    } else {
        pthread_mutex_unlock(&log_mutex);
    }
}

void save_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    pthread_mutex_lock(&log_mutex);
    vprintf(format, args);
    pthread_mutex_unlock(&log_mutex);
    va_end(args);
}