#include "main.h"

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