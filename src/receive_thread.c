#include "main.h"

//         '昵称名\tIP\t当前时间'  # 以秒为单位
int process_broadcast_data(char* data, friend_info* friend_info_local) {
    // 解析数据
    char* token = strtok(data, "\t");
    if (token == NULL) {
        return -1;
    } 
    strncpy(friend_info_local->name, token, sizeof(friend_info_local->name) - 1);
    friend_info_local->name[sizeof(friend_info_local->name) - 1] = '\0';

    token = strtok(NULL, "\t");
    if (token == NULL) {
        return -1;
    }
    strncpy(friend_info_local->ip, token, sizeof(friend_info_local->ip) - 1);

    token = strtok(NULL, "\t");
    if (token == NULL) {
        return -1;
    }

    long timestamp = strtol(token, NULL, 10);

    log_info(
        "Received broadcast from %s at %s with timestamp %ld", 
        friend_info_local->name, friend_info_local->ip, timestamp
    );
    return 0;
}

// '昵称名\t己方IP\t好友名\t对方IP\t聊天内容'
int process_chat_data(char* data, chat_record* recv_data) {
    // 解析数据
    char* token = strtok(data, "\t");
    if (token == NULL) {
        return -1;
    }
    strncpy(recv_data->name, token, sizeof(recv_data->name) - 1);
    recv_data->name[sizeof(recv_data->name) - 1] = '\0';

    // token = strtok(data, "\t");
    // if (token == NULL) {
    //     return -1;
    // }

    // // 己方IP: 本机发出的, 把昵称名设置为本机昵称
    // if (strcmp(token, g_config->local_ip) == 0) {
    //     strncpy(recv_data->name, g_config->nickname, sizeof(recv_data->name) - 1);
    //     recv_data->name[sizeof(recv_data->name) - 1] = '\0';
    // }

    // 己方IP\t好友名\t对方IP\t 好像用不到, 先丢掉
    for (int i = 0; i < 3; i++) {
        token = strtok(NULL, "\t");
        if (token == NULL) {
            return -1;
        }
    }

    token = strtok(NULL, "\t");
    if (token == NULL) {
        return -1;
    }
    strncpy(recv_data->content, token, sizeof(recv_data->content) - 1);
    recv_data->content[sizeof(recv_data->content) - 1] = '\0';

    recv_data->rxtime = (long)time(NULL);

    log_info(
        "Received chat from %s: %s || now timestamp %ld", 
        recv_data->name, recv_data->content, recv_data->rxtime
    );
    return 0;
}

// 处理br_sock
void recv_broadcast_data(void) {
    // 接收广播数据;
    char buffer[BUF_SIZE] = {0};
    friend_info friend_info_local;

    // 非阻塞等待广播数据
    int recv_len = recv(
        bt_rx_s, buffer, sizeof(buffer), MSG_DONTWAIT
    );

    if (recv_len == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // 没收到消息
        } else {
            perror("read");
            log_error("Failed to receive broadcast data: %s", strerror(errno));
        }
        return;
    } else if (recv_len > 0) {
        // 解析数据
        process_broadcast_data(buffer, &friend_info_local);
        // 更新g_friends[N];
        if (push_g_friends(&friend_info_local) != 0) {
            log_warn("g_friends full");
        } else {
            // 更新显示S1;
            refresh_S1();
        }
        return;
    } else {
        log_warn("recv_len <= 0, recv_len: %d", recv_len);
        return;
    }
}

// 处理da_sock
void recv_chat_data(void) {
    char buffer[BUF_SIZE] = {0};
    chat_record chat_record_local;

    int recv_len = recv(
        da_rx_s, buffer, sizeof(buffer), MSG_DONTWAIT
    );

    if (recv_len == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // 没收到消息
        } else {
            perror("read");
            log_error("Failed to receive broadcast data: %s", strerror(errno));
        }
        return;
    } else if (recv_len > 0) {
        // 收到消息
        process_chat_data(buffer, &chat_record_local);
        //  更新g_chats[M];
        if (push_g_chats(&chat_record_local) != 0) {
            log_warn("g_chats push error");
        } else {
            //  更新显示S2;
            refresh_S2();
        }
        return;
    } else {
        log_warn("recv_len <= 0, recv_len: %d", recv_len);
        return;
    }
}

void* receive_thread(void* arg) {
    
    while(g_bRunning) {
        recv_broadcast_data();
        recv_chat_data();
    }

    pthread_exit(NULL);
}