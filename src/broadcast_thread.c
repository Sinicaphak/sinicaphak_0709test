#include "main.h"

void* broadcast_thread(void* nothing) {
    char pack[BUF_SIZE];
    struct sockaddr_in des_addr;
    bzero(&des_addr, sizeof(des_addr));
	des_addr.sin_family = AF_INET;
    //广播地址
	des_addr.sin_addr.s_addr = inet_addr(BROADCAST_IP); 
	des_addr.sin_port = htons(BROADCAST_PORT);

    while(g_bRunning) {
        // 构造广播数据包
        //  '昵称名\tIP\t当前时间'  # 以秒为单位 
        snprintf(
            pack, sizeof(pack),
            "%s\t%s\t%ld",
            g_config->nickname,
            g_config->local_ip,
            time(NULL)
        );
        // 发送广播数据包
        if (
            sendto(
                br_tx_s, pack, strlen(pack), 0, 
                (struct sockaddr*)&des_addr, sizeof(des_addr)
            ) < 0
        ) {
            log_error("Broadcast send failed: %s", strerror(errno));
        }

        // 等待一段时间再发送下一个广播包
        sleep(BROADCAST_SLEEP_SEC);
    }

    pthread_exit(NULL);
}