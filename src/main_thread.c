#include "main.h"

// 广播线程
pthread_t broadcast_thread_pid;
// 接收线程
pthread_t receive_thread_pid;

// 标记S3状态
// 当前好友代号
uchar friend_now = NULL_FRIEND; 
// 输入模式
uchar input_model = INPUT_MODEL; 
// 是否按下S键, S发送聊天记录(按CR键后才生效)
bool is_press_S = false; 
// 是否按下CR键, CR发送聊天记录(按CR键后才生效)
bool is_press_cr = false; 

// 创建广播发送br_tx_s;
// 创建广播接收bt_rx_s,
// 创建数据发送da_tx_s;
// 创建数据接收da_rx_s,
int br_tx_s = 0;
int bt_rx_s = 0;
int da_tx_s = 0;
int da_rx_s = 0;

// 标记程序运行状态
bool g_bRunning = g_bRunning_EXIT;

// 收尾
void end_program(int signum) {
    g_bRunning = g_bRunning_EXIT;
    log_info("Ending program...");
    // 恢复光标
    printf("\033[?25h");
    // 设置终端为规范模式
    set_input_mode(0);
    // 释放资源
    if (g_config) {
        free(g_config);
        g_config = NULL;
    }
    // 关闭套接字
    close(br_tx_s);
    close(bt_rx_s);
    close(da_tx_s);
    close(da_rx_s);
    // todo 不加这一行程序会退出吗
    // todo crtl c要按2次才退出
    signal(SIGINT, SIG_DFL);
    // 清屏
    system("clear");
}

void main_thread(void) {
    
}