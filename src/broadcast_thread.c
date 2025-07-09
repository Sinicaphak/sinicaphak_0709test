#include "main.h"

void* broadcast_thread(void* package) {
    char pack[BUF_SIZE];
    struct sockaddr_in des_addr;
    bzero(&des_addr, sizeof(des_addr));
	des_addr.sin_family = AF_INET;
    //广播地址
	des_addr.sin_addr.s_addr = inet_addr(BROADCAST_IP); 
	des_addr.sin_port = htons(BROADCAST_PORT);

    while(g_bRunning) {
        //  '昵称名\tIP\t当前时间'  # 以秒为单位 
        snprintf(
            pack, sizeof(pack),
            "%s\t%s\t%ld",
            g_config->nickname,
            g_config->local_ip,
            time(NULL)
        );
        if (
            sendto(
                br_tx_s, pack, strlen(pack), 0, 
                (struct sockaddr*)&des_addr, sizeof(des_addr)
            ) < 0
        ) {
            log_error("Broadcast send failed: %s", strerror(errno));
        }
        sleep(5);
    }

    pthread_exit(NULL);
}