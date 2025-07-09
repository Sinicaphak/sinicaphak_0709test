#ifndef UDP_SOCKET_H
#define UDP_SOCKET_H

int create_udp_send_socket(int* server_socket, bool is_broadcast);
int create_udp_recv_socket(int* client_socket, bool is_broadcast);

#endif