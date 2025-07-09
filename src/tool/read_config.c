#include "main.h"

enum LOG_LEVEL get_log_level(const char *level_str) {
    if (strcmp(level_str, "trace") == 0) return LOG_TRACE;
    if (strcmp(level_str, "debug") == 0) return LOG_DEBUG;
    if (strcmp(level_str, "info") == 0) return LOG_INFO;
    if (strcmp(level_str, "warning") == 0) return LOG_WARN;
    if (strcmp(level_str, "error") == 0) return LOG_ERROR;
    if (strcmp(level_str, "fatal") == 0) return LOG_FATAL;
    log_warn("unknown log level: %s, defaulting to info", level_str);
    return LOG_INFO; // 默认日志等级
}

/** 
 * @brief 读取json配置文件
 * @param input config_path 配置文件路径
 * @param output config_json 解析后的json对象
 * @return 0 成功, -1 失败
 */
int read_config(const char* config_path, cJSON** config_json) {
    // 找到程序文件现在的绝对路径, 和配置文件文件的相对路径一起拼成配置文件的绝对路径
    char exe_path[2048];
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len != -1) {
        exe_path[len] = '\0';
        char exe_dir[1024];
        strcpy(exe_dir, exe_path);
        // exe_dir 现在是可执行文件所在目录
        dirname(exe_dir); 
        // 拼接配置文件路径
        snprintf(exe_path, sizeof(exe_path), "%s/%s", exe_dir, config_path);
    }
    log_info(exe_path);
    // 打开配置文件
    FILE *file = fopen(exe_path, "r");
    if (!file) {
        perror("Failed to open config file");
        return -1;
    }
    // 加载文件内容
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char json_data[file_size + 1];

    fread(json_data, 1, file_size, file);
    json_data[file_size] = '\0';

    // 解析JSON数据
    *config_json = cJSON_Parse(json_data);
    if(*config_json == NULL) {
        log_fatal("client config json parse error");
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

Config* g_config;

/** 
 * @brief 读取配置文件
 * @param output config 配置文件信息
 * @return 0 成功, -1 失败
 */
int read_out_config(Config* config) {
    cJSON* cjson_test = NULL;
    if ( read_config(TASK_CONFIG_FILE, &cjson_test) == -1 ) {
        perror("config json not exist");
        log_fatal("config json parse error");
        return -1;
    }
    
    cJSON* nickname  = cJSON_GetObjectItem(cjson_test, "nickname");
    cJSON* local_ip  = cJSON_GetObjectItem(cjson_test, "local_ip");
    if (nickname == NULL || local_ip == NULL) {
        log_fatal("config json parse error");
        cJSON_Delete(cjson_test);
        return -1;
    }
    // 分配空间
    config->nickname = malloc(strlen(nickname->valuestring) + 1);
    if (config->nickname == NULL) {
        log_fatal("Failed to allocate memory for config->nickname");
        cJSON_Delete(cjson_test);
        return -1;
    }
    strcpy(config->nickname, nickname->valuestring);
    strncpy(config->local_ip, local_ip->valuestring, sizeof(config->local_ip) - 1);
    config->local_ip[sizeof(config->local_ip) - 1] = '\0';

    cJSON_Delete(cjson_test);
    return 0;
}