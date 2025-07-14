#ifndef READ_CONFIG_H
#define READ_CONFIG_H


#ifdef __cplusplus
extern "C" {
#endif


#define TASK_CONFIG_FILE "config.json"

typedef struct Config {
    // 昵称
    char* nickname;  
    // 本机ip
    char local_ip[16];
} Config;

extern Config* g_config;

int read_out_config(Config *config);

#ifdef __cplusplus
}
#endif

#endif