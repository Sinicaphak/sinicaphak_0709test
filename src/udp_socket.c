#include "main.h"

int create_udp_send_socket(int* server_socket, bool is_broadcast) {
    // 服务器地址
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    
    //服务器ip端口
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(g_config->local_ip);
    server_addr.sin_port = 0;

    // 创建socket
    *server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (*server_socket == -1) {
        perror("socket creation failed"); 
        log_fatal("socket creation failed");
        return -1;
    }

    char* ifname = "ens39";
    setsockopt(*server_socket, SOL_SOCKET, SO_BINDTODEVICE, ifname, strlen(ifname));

    if (is_broadcast) {
        // 允许广播
        int broadcastEnable = 1;
        setsockopt(*server_socket, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
    }

    if ( 
        bind(*server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0
    ) {
        perror("server udp bind failed");
        log_fatal("bind failed");
    }

    return 0;
}

int create_udp_recv_socket(int* client_socket, bool is_broadcast) {
    // 地址
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));

    //ip端口
    client_addr.sin_family = AF_INET; 
    if (is_broadcast) {
        client_addr.sin_addr.s_addr = INADDR_ANY;
        client_addr.sin_port = htons(BROADCAST_PORT);
    } else {
        client_addr.sin_addr.s_addr = inet_addr(g_config->local_ip);
        client_addr.sin_port = htons(CHAT_PORT);
    }

    // 创建socket
    *client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (*client_socket == -1) {
        perror("socket creation failed"); 
        log_fatal("socket creation failed");
        return -1;
    }

    // todo 在一台设备上共享广播监听端口
    if (is_broadcast) {
        int opt = 1;
        setsockopt(*client_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    }

    if ( 
        bind(*client_socket, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0
    ) {
        perror("server udp bind failed");
        log_fatal("bind failed");
        close(*client_socket);
        return -1;
    }

    return 0;

}