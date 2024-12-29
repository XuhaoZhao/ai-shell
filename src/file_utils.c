#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_utils.h"

// 从字符串中提取最后一段路径
const char *extract_last_path(const char *input) {
    if (input == NULL) {
        return NULL; // 输入为空
    }

    // 复制输入字符串，因为 strtok 会修改原字符串
    char *input_copy = strdup(input);
    if (input_copy == NULL) {
        perror("Failed to allocate memory");
        return NULL;
    }

    // 使用 strtok 分割字符串
    const char *delim = " ";
    char *token = strtok(input_copy, delim);
    const char *last_path = NULL;

    while (token != NULL) {
        last_path = token; // 更新最后一个子字符串
        token = strtok(NULL, delim);
    }

    // 复制最后一个子字符串作为结果
    const char *result = (last_path != NULL) ? strdup(last_path) : NULL;

    // 释放复制的输入字符串
    free(input_copy);

    return result;
}
// 获取文件的最后 n 行内容
int get_last_n_lines(const char *file_path, char *result, size_t result_size, int n) {
    if (file_path == NULL || result == NULL || result_size == 0 || n <= 0) {
        fprintf(stderr, "Invalid arguments\n");
        return -1; // 参数无效
    }

    FILE *fp = fopen(file_path, "r");
    if (fp == NULL) {
        perror("Failed to open file");
        return -1; // 文件打开失败
    }

    // 将文件指针移动到文件末尾
    if (fseek(fp, 0, SEEK_END) != 0) {
        perror("Failed to seek to end of file");
        fclose(fp);
        return -1;
    }

    long file_size = ftell(fp); // 获取文件大小
    if (file_size == -1) {
        perror("Failed to get file size");
        fclose(fp);
        return -1;
    }

    // 从文件末尾开始查找第 n 个换行符
    int newline_count = 0;
    long pos = file_size - 1;
    while (pos >= 0 && newline_count < n) {
        if (fseek(fp, pos, SEEK_SET) != 0) {
            perror("Failed to seek in file");
            fclose(fp);
            return -1;
        }

        char ch = fgetc(fp);
        if (ch == '\n') {
            newline_count++;
        }

        pos--;
    }

    // 如果文件行数不足 n 行，从文件开头开始读取
    if (newline_count < n) {
        fseek(fp, 0, SEEK_SET);
    } else {
        fseek(fp, pos + 2, SEEK_SET); // 移动到第 n 个换行符之后
    }

    // 读取最后 n 行内容
    size_t bytes_read = fread(result, 1, result_size - 1, fp);
    result[bytes_read] = '\0'; // 添加字符串结束符

    fclose(fp);
    return 0; // 成功
}