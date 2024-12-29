#ifndef FILE_UTILS_H
#define FILE_UTILS_H

// 函数声明
int get_last_n_lines(const char *file_path, char *result, size_t result_size, int n);
const char *extract_last_path(const char *input);
#endif // FILE_UTILS_H