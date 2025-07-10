#include "main.h"

//非阻塞检测键盘输入
static int get_char() {
    fd_set rfds;
    struct timeval tv;
    int ch=0;
    FD_ZERO(&rfds);
    FD_SET(0,&rfds);
    tv.tv_sec =0;
    tv.tv_usec=1;//设置等待超时时间
    //检测键盘是否有输入
    if(select(1,&rfds,NULL,NULL,&tv)>0) {
        ch = getchar();
    }
    return ch;
}

void main() {    
    init_all();

    while(g_bRunning) {
        main_thread();
    }

    g_bRunning = g_bRunning_EXIT;
    // 等待子线程结束
    pthread_join(broadcast_thread_pid, NULL);
    pthread_join(receive_thread_pid, NULL);
    end_program(0);
}