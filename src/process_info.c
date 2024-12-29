#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "process_info.h"

// 从 /proc/[pid]/status 文件中获取进程信息
ProcessInfo get_process_info(int pid) {
    ProcessInfo info = {0};
    char path[256];
    FILE *fp;

    // 构造 /proc/[pid]/status 文件路径
    snprintf(path, sizeof(path), "/proc/%d/status", pid);

    // 打开文件
    fp = fopen(path, "r");
    if (fp == NULL) {
        perror("Failed to open status file");
        return info;
    }

    // 读取进程名称
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "Name:", 5) == 0) {
            sscanf(line, "Name:\t%s", info.name);
        } else if (strncmp(line, "PPid:", 5) == 0) {
            sscanf(line, "PPid:\t%d", &info.pid);
        }
    }

    fclose(fp);

    // 构造 /proc/[pid]/cmdline 文件路径
    snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);

    // 打开文件
    fp = fopen(path, "r");
    if (fp == NULL) {
        perror("Failed to open cmdline file");
        return info;
    }

    // 读取进程启动参数
    char *ptr = info.cmdline;
    int ch;
    while ((ch = fgetc(fp)) != EOF) {
        if (ch == '\0') {
            *ptr++ = ' '; // 将空字符替换为空格
        } else {
            *ptr++ = ch;
        }
    }
    *ptr = '\0'; // 添加字符串结束符

    fclose(fp);
    return info;
}

// 获取祖父进程信息
ProcessInfo get_grandparent_process_info() {
    ProcessInfo grandparent_info = {0};

    // 获取当前进程的父进程ID
    int parent_pid = getppid();

    // 获取父进程的信息
    ProcessInfo parent_info = get_process_info(parent_pid);
    if (parent_info.pid == 0) {
        fprintf(stderr, "Failed to get parent process info\n");
        return grandparent_info;
    }

    // 获取祖父进程的信息
    grandparent_info = get_process_info(parent_info.pid);
    if (grandparent_info.pid == 0) {
        fprintf(stderr, "Failed to get grandparent process info\n");
    }

    return grandparent_info;
}