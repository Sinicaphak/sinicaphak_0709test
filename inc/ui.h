#ifndef UI_H
#define UI_H

#include "main.h"

#define ORDER_MODEL 'e'
#define INPUT_MODEL 'i'

#define NULL_FRIEND 255

#define S1_ROW 4
#define S2_ROW 16
#define S3_ROW 2

#define N 16
struct friend_info {
    char name[20];
    char ip[16];
};
typedef struct friend_info friend_info;
extern friend_info g_friends[N];
extern uchar g_friends_len;
extern pthread_mutex_t g_friends_lock;

#define M 8
struct chat_record {
    char name[20];
    long rxtime;
    char content[80+1];
};
typedef struct chat_record chat_record;
extern chat_record g_chats[M];
extern uchar g_chats_len;
extern pthread_mutex_t g_chats_lock;

// 输入的聊天内容
extern char buf_input[80+1]; 

int push_g_friends(friend_info* new_friend);
int push_g_chats(chat_record* new_record);
int push_buf_input(char input);
int pop_buf_input(void);

void refresh_S1(void);
void refresh_S2(void);
void refresh_S3(uchar friend_now, char model, bool is_press_S);

#endif