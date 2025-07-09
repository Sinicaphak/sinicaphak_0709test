/********************************************************************************
 * cqueue.h
 ********************************************************************************/
#ifndef CQUEUE_H
#define CQUEUE_H

#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct CQueueItem {
    int priority;
    void* next;
    void* value;
} CQueueItem;

typedef struct CQueueHead {
    CQueueItem* head;
    CQueueItem* tail;
    uint64_t len;
    pthread_mutex_t* lock;
} CQueueHead;

#ifdef PRIORITY
    // 宏PRIORITY允许cqueue通过优先级将元素插入队首
    // 插入队首
    void cqueue_insert(CQueueHead* handle, void* value, int priority);
    void cqueue_push(CQueueHead* handle, void* value, int priority);
#else
    void cqueue_push(CQueueHead* handle, void* value);
#endif
CQueueHead* cqueue_create();
void cqueue_destroy(CQueueHead* handle);
void* cqueue_pop(CQueueHead* handle);
uint64_t cqueue_len(CQueueHead* handle);

#endif /* CQUEUE_H */