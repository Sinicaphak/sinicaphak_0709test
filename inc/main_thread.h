#ifndef MAIN_THREAD_H
#define MAIN_THREAD_H

// 广播socket端口
#define BROADCAST_PORT 10000
// 聊天socket端口
#define CHAT_PORT 10001

// 标记程序结束, 准备退出
#define g_bRunning_EXIT 0
// 标记程序正常运行中
#define g_bRunning_RUNNING 1

// 标记未选中好友
#define NULL_FRIEND 255

// 聊天输入内容的最大长度为80, buf_input
#define CHAT_INPUT_LEN 80

// 创建广播发送br_tx_s;
// 创建广播接收bt_rx_s,
// 创建数据发送da_tx_s;
// 创建数据接收da_rx_s,
extern int br_tx_s;
extern int bt_rx_s;
extern int da_tx_s;
extern int da_rx_s;

// 广播线程
extern pthread_t broadcast_thread_pid;
// 接收线程
extern pthread_t receive_thread_pid;

// 标记S3状态
// 当前好友代号
extern uchar friend_now; 
// 输入模式
extern uchar input_model; 
// 是否按下S键, S发送聊天记录(按CR键后才生效)
extern bool is_press_S; 
// 是否按下CR键, CR发送聊天记录(按CR键后才生效)
extern bool is_press_cr; 

// 标记程序运行状态
extern bool g_bRunning;

// 输入的聊天内容, 多一位保证字符串以'\0'结尾
extern char buf_input[CHAT_INPUT_LEN + 1]; 

void end_program(int signum);
void main_thread(void);

#endif