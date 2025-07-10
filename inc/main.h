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
#include "init.h"
#include "main_thread.h"
#include "udp_socket.h"
#include "broadcast_thread.h"
#include "receive_thread.h"
#include "read_config.h"
#include "ui.h"

#endif