#ifndef UI_H
#define UI_H

#include "main.h"

// 标记两个输入模式, input_model
// 命令模式
#define ORDER_MODEL 'e'
// 输入模式
#define INPUT_MODEL 'i'

// S1占用4行
#define S1_ROW 4
// S2占用16行
#define S2_ROW 16
// S3占用2行
#define S3_ROW 2

// 最多允许16个用户同时聊天
#define N 16
struct friend_info {
    char name[20];
    char ip[16];
};
typedef struct friend_info friend_info;
// 好友列表
extern friend_info g_friends[N];
// 好友列表长度
extern uchar g_friends_len;
// 好友列表锁
extern pthread_mutex_t g_friends_lock;

// 聊天记录的最大长度
#define M 8
struct chat_record {
    char name[20];
    long rxtime;
    char content[80+1];
};
typedef struct chat_record chat_record;
// 聊天记录列表
extern chat_record g_chats[M];
// 聊天记录列表长度
extern uchar g_chats_len;
// 聊天记录列表锁
extern pthread_mutex_t g_chats_lock;

// S2区域刷新时加锁
extern pthread_mutex_t refresh_S2_lock;

int push_g_friends(friend_info* new_friend);
int push_g_chats(chat_record* new_record);
int push_buf_input(char input);
int pop_buf_input(void);

void refresh_S1(void);
void refresh_S2(void);
void refresh_S3(uchar friend_now, char model, bool is_press_S);

#endif