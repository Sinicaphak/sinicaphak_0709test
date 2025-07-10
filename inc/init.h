#ifndef INIT_H
#define INIT_H

// 默认日志级别
#define LOG_LEVEL_LOCAL LOG_WARN

// 程序初始化
void init_all(void);
// 结束程序时清理
void end_program(int signum);
// 设置终端规范模式
// 无需回车直接捕捉输入字符
void set_input_mode(int enable);

#endif