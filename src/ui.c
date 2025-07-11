#include "main.h"

friend_info g_friends[N];
chat_record g_chats[M];
uchar g_friends_len;
uchar g_chats_len;
pthread_mutex_t g_friends_lock;
pthread_mutex_t g_chats_lock;
pthread_mutex_t refresh_S2_lock;

// a) 第1部分有S1=4行, 用于显示好友列表, 每行显示4位, 显示格式为'[0..9A..F]. friendname'
//       第S1+1行显示分隔行
void refresh_S1(void) {
    int start_row = 0;
    // 清除S1部分的内容
    for (int i = 0; i < S1_ROW; i++) {
        printf("\033[%d;1H\033[K", start_row + i);
        // printf("\033[41;33m");
    }
    // 显示S1部分内容
    uchar len = 0;
    // S1=4行, 每行显示4位好友, 格式: '[0..9A..F]. friendname'
    int per_line = 4;
    for (uchar i = 0; i < g_friends_len; i++) {
        printf("[%hX]. %s", i, g_friends[i].name);
        if ((i + 1) % per_line == 0 || i == g_friends_len - 1) {
            printf("\n");
        } else {
            printf("  "); // 每个名字之间加两个空格
        }
    }
    start_row = S1_ROW + 1;
    printf("\033[%d;1H///////////分隔行s1//////////\n", start_row);
    // printf(" \033[0m");
}
// b) 第2部分有S2=16行, 可显示8条最新的聊天记录, 每条聊天记录的格式为
//         好友名 时:分:秒
//         聊天内容(<=80字符)
void refresh_S2(void) {
    pthread_mutex_lock(&refresh_S2_lock);
    int start_row = S1_ROW + 2;
    // 清除S2部分的内容
    for (int i = 0; i < S2_ROW; i++) {
        printf("\033[%d;1H\033[K", start_row + i);
    }
    // 显示S2部分内容
    int records_to_show = g_chats_len;
    for (int i = 0; i < g_chats_len; i++) {
        int idx = g_chats_len - records_to_show + i;
        
        time_t t = (time_t)g_chats[idx].rxtime;
        struct tm *tm_info = localtime(&t);
        char time_str[9];
        strftime(time_str, sizeof(time_str), "%H:%M:%S", tm_info);

        printf("\033[%d;1H%s %s \n%s\n", 
            start_row + i*2, g_chats[idx].name,
            time_str,
            g_chats[idx].content
        );
    }
    start_row = S1_ROW + 1 + S2_ROW + 1;
    printf("\033[%d;1H///////////分隔行s2//////////\n", start_row);
    pthread_mutex_unlock(&refresh_S2_lock);
}

// c) 第3部分有S3=2行, 分别显示
// 输入的聊天内容(<=80字符)
// 好友代号列表[0..9A..F]
void refresh_S3(uchar friend_now, char model, bool is_press_S) {
    // 清除S3部分的内容
    int start_row = S1_ROW + 1 + S2_ROW + 2;
    for (int i = 0; i < S3_ROW; i++) {
        printf("\033[%d;1H\033[K", start_row + i);
    }

    uchar friend_state = friend_now == NULL_FRIEND ? 'N': friend_now;
    // 显示S3部分内容
    printf("\033[%d;1H %s", start_row, buf_input);
    start_row++;
    printf("\033[%d;1H [%c] 模式:[%c] 按S:[%d]", start_row, friend_state, model, (int)is_press_S);
}

int push_g_friends(friend_info* new_friend) {
    pthread_mutex_lock(&g_friends_lock);
    if (g_friends_len >= N) {
        // 超过最大好友数量
        log_error("好友列表已满，无法添加新好友");
        return -1; 
    }

    for (int i = 0; i < g_friends_len; i++) {
        if (strcmp(g_friends[i].name, new_friend->name) == 0) {
            // 好友已存在
            return 0;
        }
    }
    memcpy(&g_friends[g_friends_len], new_friend, sizeof(friend_info));
    g_friends_len++;
    pthread_mutex_unlock(&g_friends_lock);
    return 0;
}

// g_chats视为循环队列
int push_g_chats(chat_record* new_record) {
    pthread_mutex_lock(&g_chats_lock);
    if (g_chats_len < M) {
        memcpy(&g_chats[g_chats_len], new_record, sizeof(chat_record));
        g_chats_len++;
    } else {
        // 循环覆盖最旧的记录
        memmove(&g_chats[0], &g_chats[1], sizeof(chat_record) * (M - 1));
        memcpy(&g_chats[M - 1], new_record, sizeof(chat_record));
    }
    pthread_mutex_unlock(&g_chats_lock);
    return 0;
}

int push_buf_input(char input) {
    size_t len = strlen(buf_input);
    if (len < CHAT_INPUT_LEN) {
        buf_input[len] = input;
        buf_input[len + 1] = '\0'; // 确保字符串以'\0'结尾
    } else {
        log_warn("输入内容超过最大长度，无法添加新字符");
    }
    return 0;
}

int pop_buf_input(void) {
    size_t len = strlen(buf_input);
    if (len > 0) {
        buf_input[len - 1] = '\0'; // 删除最后一个字符
    } else {
        log_info("输入内容为空，无法删除字符");
    }
    return 0;
}