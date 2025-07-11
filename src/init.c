#include "main.h"

// 创建线程
int CreateThread(pthread_t* thread, void *(*thread_func)(void *)) {
    if (pthread_create(thread, NULL, thread_func, NULL) != 0) {
        perror("Failed to create thread");
        log_fatal("Failed to create thread");
        return -1;
    }
    
    return 0;
}

// 设置终端为非规范模式
// 无需回车直接捕捉输入字符
void set_input_mode(int enable) {
    static struct termios oldt;
    static struct termios newt;
    if (enable) {
        tcgetattr(STDIN_FILENO, &oldt); // 保存原设置
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO); // 关闭规范模式和回显
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    } else {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // 恢复原设置
    }
}

void init_lock(void) {
    // 设置日志锁
    log_set_lock(log_lock, NULL);
    // 初始化g_chats和g_friends的锁
    pthread_mutex_init(&g_friends_lock, NULL);
    pthread_mutex_init(&g_chats_lock, NULL);
    // 初始化S2区域刷新锁
    pthread_mutex_init(&refresh_S2_lock, NULL);
}

// 程序初始化
void init_all(void) {
    log_info("Starting server...");
    // 初始化锁
    init_lock();
    // 日志等级
    log_set_level(LOG_LEVEL_LOCAL);
    
    // 加载本地配置
    g_config = malloc(sizeof(Config*));
    read_out_config(g_config);

    // 创建UDP socket
    if (
        create_udp_send_socket(&br_tx_s, true)  < 0 ||
        create_udp_recv_socket(&bt_rx_s, true)  < 0 ||
        create_udp_send_socket(&da_tx_s, false) < 0 ||
        create_udp_recv_socket(&da_rx_s, false) < 0
    ) {
        log_fatal("Failed to create UDP sockets");
        g_bRunning = g_bRunning_EXIT;
    }

    // 标记程序正常运行
    g_bRunning = g_bRunning_RUNNING;

    // 注册 crtl c 时收尾函数
    signal(SIGINT, end_program);
    // 无需回车直接捕捉输入字符
    set_input_mode(1);

    // 创建线程
    CreateThread(&broadcast_thread_pid, broadcast_thread);
    CreateThread(&receive_thread_pid, receive_thread);

    // S3状态初始化
    // 当前好友代号
    friend_now = NULL_FRIEND; 
    // 输入模式
    input_model = INPUT_MODEL; 
    // 是否按下S键, S发送聊天记录(按CR键后才生效)
    is_press_S = false; 
    // 是否按下CR键, CR发送聊天记录(按CR键后才生效)
    is_press_cr = false; 

    // 清屏
    system("clear");
    refresh_S1();
    refresh_S2();
    refresh_S3(friend_now, input_model, is_press_S);

    // 隐藏光标, 不然会乱闪
    printf("\033[?25l");
}

