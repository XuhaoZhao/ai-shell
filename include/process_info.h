#ifndef PROCESS_INFO_H
#define PROCESS_INFO_H

// 定义进程信息结构体
typedef struct {
    int pid;          // 进程ID
    char name[256];   // 进程名称
    char cmdline[1024]; // 进程启动参数
} ProcessInfo;

// 函数声明
ProcessInfo get_grandparent_process_info();

#endif // PROCESS_INFO_H