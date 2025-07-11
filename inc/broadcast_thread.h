#ifndef BROADCAST_THREAD_H
#define BROADCAST_THREAD_H

// 广播的网段
#define BROADCAST_IP "192.168.206.255"
// 广播时间间隔
#define BROADCAST_SLEEP_SEC 5

void* broadcast_thread(void* nothing);

#endif