#include "main.h"

#define ORDER_MODEL 'e'
#define INPUT_MODEL 'i'

const enum LOG_LEVEL LOG_LEVEL = LOG_WARN;
bool g_bRunning = true;

// 创建广播发送br_tx_s;
// 创建广播接收bt_rx_s,
// 创建数据发送da_tx_s;
// 创建数据接收da_rx_s,
int br_tx_s = 0;
int bt_rx_s = 0;
int da_tx_s = 0;
int da_rx_s = 0;

pthread_t broadcast_thread_pid;
pthread_t receive_thread_pid;

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

// 收尾
void end_program(int signum) {
    g_bRunning = false;
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

void main() {    
    // 设置日志锁
    log_set_lock(log_lock, NULL);
    log_info("Starting server...");
    log_set_level(LOG_LEVEL);

    // 隐藏光标, 不然会乱闪
    printf("\033[?25l");
    
    // 加载本地配置
    g_config = malloc(sizeof(Config*));
    read_out_config(g_config);

    if (
        create_udp_send_socket(&br_tx_s, true)  < 0 ||
        create_udp_recv_socket(&bt_rx_s, true)  < 0 ||
        create_udp_send_socket(&da_tx_s, false) < 0 ||
        create_udp_recv_socket(&da_rx_s, false) < 0
    ) {
        log_fatal("Failed to create UDP sockets");
        g_bRunning = false;
    }

    // 初始化g_chats和g_friends的锁
    pthread_mutex_init(&g_friends_lock, NULL);
    pthread_mutex_init(&g_chats_lock, NULL);

    // 创建线程
    CreateThread(&broadcast_thread_pid, broadcast_thread);
    CreateThread(&receive_thread_pid, receive_thread);

    // 标记S3状态
    static uchar friend_now = NULL_FRIEND; // 当前好友代号
    static char input_model = INPUT_MODEL; // 输入模式
    static bool is_press_S = false; // 是否按下S键, S发送聊天记录(按CR键后才生效)
    static bool is_press_cr = false; // 是否按下CR键, CR发送聊天记录(按CR键后才生效)

    // 注册 crtl c 时收尾函数
    signal(SIGINT, end_program);
    // 设置终端为非规范模式
    set_input_mode(1);

    // 清屏
    system("clear");
    refresh_S1();
    refresh_S2();
    refresh_S3(friend_now, input_model, is_press_S);

    while(g_bRunning) {
        // 接收输入;
        // char ch = get_char();
        char ch = getchar();
        //检测缓冲区中是否有数据
        if (ch != 0) {  
            if (input_model == INPUT_MODEL) {
                switch (ch) {
                    case 27: { 
                        // ESC key ASCII code is 27
                        log_info("ESC pressed");
                        input_model = ORDER_MODEL;
                        break;
                    }
                    case 127: {
                        // 退格键ASCII码为127
                        // 从输入缓冲区删除最后一个字符
                        pop_buf_input(); 
                        break;
                    }
                    default: {
                        push_buf_input(ch); // 其他字符直接添加到输入缓冲区
                        break;
                    }
                }
            } else if (input_model == ORDER_MODEL) {
                switch (ch) {
                    case 27: { 
                        // ESC key ASCII code is 27
                        log_info("ESC pressed");
                        input_model = ORDER_MODEL;
                        break;
                    }
                    case '0': case '1': case '2': case '3': case '4':
                    case '5': case '6': case '7': case '8': case '9':
                    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': {
                        log_info("Friend selected: %c", ch);
                        if ((uchar)ch == friend_now) {
                            friend_now = NULL_FRIEND;
                        } else {
                            friend_now = (uchar)ch;
                        }
                        break;
                    }
                    case 's': {
                        is_press_S = true; // 按下S键
                        log_info("S key pressed, ready to send message");
                        break;
                    }
                    case 'i' : {
                        input_model = INPUT_MODEL; // 切换到输入模式
                        log_info("Switched to input mode");
                        break;
                    }
                    case 'q' : {
                        g_bRunning = false; // 退出程序
                        log_info("Quit command received, exiting...");
                        break;
                    }
                    case '\n': {
                        // 回车键在输入模式下，发送输入缓冲区内容
                        if (is_press_S) {
                            is_press_cr = true; // 按下CR键
                            is_press_S = false;
                            log_info("press cr in order mode, S pressed");
                        } else {
                            log_info("Enter pressed in order mode, but S not pressed");
                        }
                        break;
                    }
                    default: {
                        log_info("Unrecognized command in order mode: %c", ch);
                        break;
                    }
                }
            } else {
                log_fatal("Unknown input model: %c", input_model);
                exit(-1);
            }
        }
       
        // 更新S3;
        refresh_S3(friend_now, input_model, is_press_S);
        // 执行命令:
        char buf_to_send[BUF_SIZE];
        // ascii字符转为数字
        int friend_now_hex = friend_now > 57 ? friend_now - 97 : friend_now - 48;
        if (is_press_cr) {
            // 发送聊天记录
            if (friend_now == NULL_FRIEND) {
                // 没有选中好友, 不能发送消息
                log_info("No friend selected, cannot send message");
            } else {            
                // '昵称名\t己方IP\t好友名\t对方IP\t聊天内容'
                memset(buf_to_send, 0, sizeof(buf_to_send));
                snprintf(buf_to_send, sizeof(buf_to_send), 
                    "%s\t%s\t%s\t%s\t%s",
                    g_config->nickname, g_config->local_ip,
                    g_friends[friend_now_hex].name,
                    g_friends[friend_now_hex].ip,
                    buf_input
                );

                struct sockaddr_in friend_addr;
                memset(&friend_addr, 0, sizeof(friend_addr));
                friend_addr.sin_family = AF_INET;
                friend_addr.sin_addr.s_addr = inet_addr(g_friends[friend_now_hex].ip);
                friend_addr.sin_port = htons(CHAT_PORT);
                if (
                    sendto(
                        da_tx_s, buf_to_send, strlen(buf_to_send), 0,
                        (struct sockaddr*)&friend_addr, sizeof(friend_addr)
                    ) < 0
                ) {
                    perror("sendto failed");
                    log_error("Failed to send chat message to friend %c", friend_now_hex);
                }
                log_info("Sending chat message to friend %c: %s", friend_now_hex, buf_input);
                // 更新g_chats[M];
                chat_record new_record;
                memset(&new_record, 0, sizeof(new_record));
                new_record.rxtime = time(NULL);
                // todo g_config->name应该是定长数组, 长度要跟new_record.name一样是20
                strncpy(new_record.name, g_config->nickname, sizeof(new_record.name) - 1);
                new_record.name[sizeof(new_record.name) - 1] = '\0';
                strncpy(new_record.content, buf_input, sizeof(new_record.content) - 1);
                new_record.content[sizeof(new_record.content) - 1] = '\0';
                push_g_chats(&new_record);
                // 清空输入内容
                // todo 测试完记得放出来
                // memset(buf_input, 0, sizeof(buf_input));
            }
            // 更新s3
            refresh_S3(friend_now, input_model, is_press_S);
            // 重置CR状态
            is_press_cr = false;
        }
        // 更新显示S2;
        refresh_S2();
    }

    g_bRunning = false;
    // 等待子线程结束
    pthread_join(broadcast_thread_pid, NULL);
    pthread_join(receive_thread_pid, NULL);
    end_program(0);
}