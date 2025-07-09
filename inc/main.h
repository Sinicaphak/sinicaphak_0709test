#ifndef MIAN_H
#define MIAN_H

#define _GNU_SOURCE
// lib
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <libgen.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <termios.h>
// tool
#include "cqueue.h"
#include "cJSON.h"
#include "log.h"
#include "save_print.h"
#include "read_config.h"
// self
#ifndef uchar
    #define uchar unsigned char
#endif
#include "udp_socket.h"
#include "broadcast_thread.h"
#include "receive_thread.h"
#include "read_config.h"
#include "ui.h"

#define BROADCAST_PORT 10000
#define CHAT_PORT 10001

// 创建广播发送br_tx_s;
// 创建广播接收bt_rx_s,
// 创建数据发送da_tx_s;
// 创建数据接收da_rx_s,
extern int br_tx_s;
extern int bt_rx_s;
extern int da_tx_s;
extern int da_rx_s;

extern bool g_bRunning;

#endif