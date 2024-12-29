#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include "process_info.h"
#include "file_utils.h"

// 响应数据结构
struct ResponseData {
    char *data;
    size_t size;
};
void test_get_grandparent_process_info() {

    
    printf("Running tests for get_grandparent_process_info...\n");
    // 测试获取祖父进程信息
    ProcessInfo grandparent_info = get_grandparent_process_info();
    if (grandparent_info.pid != 0) {
        printf("Test Passed:\n");
        printf("  Grandparent Process ID: %d\n", grandparent_info.pid);
        printf("  Grandparent Process Name: %s\n", grandparent_info.name);
        printf("  Grandparent Process Cmdline: %s\n", grandparent_info.cmdline);
    } else {
        printf("Test Failed: Unable to get grandparent process info.\n");
    }
    // 分配结果缓冲区
    char result[1024]; // 假设缓冲区足够大
    const char * cmd_line = grandparent_info.cmdline;
    const char * file_path = extract_last_path(cmd_line);
    printf("file path:%s\n",file_path);
    // 获取最后 5 行
    if (get_last_n_lines(file_path, result, sizeof(result), 5) == 0) {
        printf("hello");
        printf("Last 5 lines:\n%s", result);
    } else {
        fprintf(stderr, "Failed to get last 5 lines\n");
    }

}



// 回调函数，用于处理从服务器收到的数据
size_t write_callback(void *ptr, size_t size, size_t nmemb, void *data) {
    size_t total_size = size * nmemb;

    // 直接处理接收到的每个数据块
    char *received_data = malloc(total_size + 1);
    if (!received_data) {
        fprintf(stderr, "Memory allocation failed!\n");
        return 0;  // 返回 0 会中断 curl 请求
    }

    // 复制接收到的数据
    memcpy(received_data, ptr, total_size);
    received_data[total_size] = '\0';  // 确保字符串结尾

    // 打印每次收到的数据块（可选）
    // printf("Received chunk: %s\n", received_data);

    // 解析当前接收到的 JSON 数据
    parse_json(received_data);
    free(received_data);  // 解析后释放内

    return total_size;
}



void parse_json(const char *json_data) {
    // 检查并移除前缀 "data "
    const char *json_start = json_data;
    if (strncmp(json_data, "data: ", 6) == 0) {
        json_start = json_data + 6; // 跳过 "data: " 前缀
    }

    // 解析 JSON
    cJSON *root = cJSON_Parse(json_start);
    if (!root) {
        fprintf(stderr, "Error parsing JSON: %s\n", cJSON_GetErrorPtr());
        fprintf(stderr, "Invalid JSON data: %s\n", json_start); // 输出未能解析的内容
        return;
    }
    // 查找 choices 数组
    cJSON *choices = cJSON_GetObjectItemCaseSensitive(root, "choices");
    if (cJSON_IsArray(choices)) {
        if (cJSON_GetArraySize(choices) == 0) {
            // 如果 choices 数组为空，打印换行符并返回
            printf("\n");
            cJSON_Delete(root); // 释放 JSON 对象
            return;
        }

        // 遍历 choices 数组并提取 content
        cJSON *choice = NULL;
        cJSON_ArrayForEach(choice, choices) {
            cJSON *delta = cJSON_GetObjectItemCaseSensitive(choice, "delta");
            if (cJSON_IsObject(delta)) {
                cJSON *content = cJSON_GetObjectItemCaseSensitive(delta, "content");
                if (cJSON_IsString(content) && (content->valuestring != NULL)) {
                    printf("%s", content->valuestring); // 打印 content 字段
                }
            }
        }
    }

    cJSON_Delete(root);
}

void call_api(const char *input_text) {
    CURL *curl;
    CURLcode res;

    // API 地址和密钥（需要替换为实际地址和密钥）
    const char *api_url = "http://170.120.150.111:8099/v1/chat/completions";
    const char *api_key = "YOUR_API_KEY";

    // 构造 JSON 数据
    char json_data[1024];
    snprintf(json_data, sizeof(json_data),
             "{\"messages\":[{\"role\":\"user\",\"content\":\"%s\"}],\"model\":\"Qwen2.5-72B-Instruct\",\"temperature\":0,\"max_tokens\":2048,\"stream\":true}", input_text);

    printf("Request JSON: %s\n", json_data); // 检查 JSON 数据


    // 初始化 CURL
    curl = curl_easy_init();
    if (curl) {
        struct curl_slist *headers = NULL;

        // 设置请求头
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "Accept: application/json");
        char auth_header[256];
        snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", api_key);
        headers = curl_slist_append(headers, auth_header);

        // 设置 CURL 选项
        curl_easy_setopt(curl, CURLOPT_URL, api_url);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // 禁用 SSL 验证（仅在测试环境下）
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);  // 不需要额外的响应数据结构
        // 执行请求
        printf("Sending request...\n");
        res = curl_easy_perform(curl);

        // 检查结果
        // 检查结果
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // 清理资源
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

    } else {
        fprintf(stderr, "Failed to initialize CURL\n");
    }
}


int main(int argc, char *argv[]) {
    // 检查用户输入
    if (argc < 2) {
        printf("Usage: %s \"your prompt here\"\n", argv[0]);
        return 1;
    }
     test_get_grandparent_process_info();

    // 获取用户输入的文本（合并参数为完整的提示）
    char input_text[1024] = {0};
    for (int i = 1; i < argc; i++) {
        strcat(input_text, argv[i]);
        if (i < argc - 1) strcat(input_text, " ");
    }

    printf("Input prompt: %s\n", input_text);

    // 调用 API
    call_api(input_text);


    return 0;
}
