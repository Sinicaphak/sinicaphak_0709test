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

// 处理用户键盘输入
void process_keyboard_input(void) {
    char ch = get_char();
    // char ch = getchar();
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
                    // 切换到命令模式
                    log_info("ESC pressed");
                    input_model = ORDER_MODEL;
                    break;
                }
                case '0': case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9':
                case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': {
                    // 选择好友
                    log_info("Friend selected: %c", ch);
                    if ((uchar)ch == friend_now) {
                        // 如果选择的好友已经是当前好友，则取消选择
                        friend_now = NULL_FRIEND;
                    } else {
                        friend_now = (uchar)ch;
                    }
                    break;
                }
                case 's': {
                    // 按下S键, 待发送状态, 准备发送消息
                    is_press_S = true;
                    log_info("S key pressed, ready to send message");
                    break;
                }
                case 'i' : {
                    // 切换到输入模式
                    input_model = INPUT_MODEL;
                    log_info("Switched to input mode");
                    break;
                }
                case 'q' : {
                    // 退出程序
                    g_bRunning = g_bRunning_EXIT;
                    log_info("Quit command received, exiting...");
                    break;
                }
                case '\n': {
                    // 回车键, 在待发送状态下，发送输入缓冲区内容
                    if (is_press_S) {
                        is_press_cr = true;
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
            // let it crash
            log_fatal("Unknown input model: %c", input_model);
            exit(-1);
        }
    }
}

// 发送聊天数据包
int check_chat_need_send(void) {
    // 检查是否在按下s后按下CR键
    if (!is_press_cr) {
        goto refuse_send_chat;  
    }

    if (friend_now == NULL_FRIEND) {
        // 没有选中好友, 不能发送消息
        log_info("No friend selected, cannot send message");
        goto refuse_send_chat;
    }

    // ascii字符转为数字, 计算好友代号
    int friend_now_hex = friend_now > 57 ? friend_now - 97 : friend_now - 48;
    // 检查好友是否存在
    if (g_friends_len < friend_now_hex) {
        // 选择好友序号大于好友列表长度, 说明没有这个好友
        log_warn("好友不存在: %c", friend_now);
        // 重置好友选择为空
        friend_now = NULL_FRIEND;
        goto refuse_send_chat;
    }

    // 发送聊天
    return 0;

// 不发送聊天
refuse_send_chat: 
    // 重置CR状态
    is_press_cr = false; 
    return -1;
}

// 构造并发送聊天数据包
int send_chat(void) {
    char buf_to_send[BUF_SIZE];
    // todo 如果以后g_friends会有删除的操作, 那最好是加锁、检查好友选择是否有效、读出好友数据、发送聊天在同一个函数完成
    // 计算好友代号
    int friend_now_hex = friend_now > 57 ? friend_now - 97 : friend_now - 48;
    // 构造聊天数据包
    // '昵称名\t己方IP\t好友名\t对方IP\t聊天内容'
    memset(buf_to_send, 0, sizeof(buf_to_send));
    snprintf(buf_to_send, sizeof(buf_to_send), 
        "%s\t%s\t%s\t%s\t%s",
        g_config->nickname, g_config->local_ip,
        g_friends[friend_now_hex].name,
        g_friends[friend_now_hex].ip,
        buf_input
    );

    // 发送数据包
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
        goto send_chat_failed;
    }
    log_info("Sending chat message to friend %c: %s", friend_now_hex, buf_input);

    // 把自己发的聊天记录记下来
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

    return 0;

// 发送失败
send_chat_failed:
    return -1;
}

// 收尾
void end_program(int signum) {
    g_bRunning = g_bRunning_EXIT;
    log_info("Ending program...");
    // 恢复光标
    printf("\033[?25h");
    // 设置终端为规范模式
    set_input_mode(0);
    // 释放配置文件占用内存
    if (g_config) {
        free(g_config);
        g_config = NULL;
    }
    // 关闭套接字
    close(br_tx_s);
    close(bt_rx_s);
    close(da_tx_s);
    close(da_rx_s);
    // 重置crtl c行为
    // todo 不加这一行程序会退出吗
    // todo crtl c要按2次才退出
    signal(SIGINT, SIG_DFL);
    // 清屏
    system("clear");
}

void main_thread(void) {
    // 处理键盘输入
    process_keyboard_input();
    // 更新S3: 输入文字与输入状态;
    refresh_S3(friend_now, input_model, is_press_S);
    // 检查是否需要发送聊天数据包
    if (
        check_chat_need_send() == -1
    ) {
        // 不需要发送聊天数据包, 直接返回
        return;
    }
    // 发送聊天数据包
    if (
        send_chat() == 0
    ) {
        // 发送成功
        // 更新显示S2;
        // todo 有两个线程尝试刷新S2, 应该加个锁
        refresh_S2();
    } else {
        // 发送失败
        log_error("Failed to send chat message");
    }

    // 更新S3: 输入文字与输入状态;
    // todo 这里输入栏和状态栏应该分开更新
    refresh_S3(friend_now, input_model, is_press_S);
}