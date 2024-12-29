# 设置最低 CMake 版本要求
cmake_minimum_required(VERSION 3.10)

# 项目名称和版本号
project(call_gpt VERSION 1.0 LANGUAGES C)

# 设置 C 标准为 C99
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# 查找 libcurl 库
set(CURL_LIBRARY /usr/local/lib/libcurl.a)
find_package(CURL REQUIRED)
set(OPENSSL_CRYPTO_LIBRARY /usr/lib/x86_64-linux-gnu/libcrypto.a)
set(OPENSSL_SSL_LIBRARY /usr/lib/x86_64-linux-gnu/libssl.a)
set(CJSON_LIBRARY /usr/local/lib/libcjson.a)
set(ZLIB_LIBRARY /usr/lib/x86_64-linux-gnu/libz.a)
# 添加源文件
# 自动收集 src 目录下的所有 .c 文件
file(GLOB SRC_FILES CONFIGURE_DEPENDS "src/*.c")

# 添加可执行文件
add_executable(call_gpt ${SRC_FILES})

# 链接静态库
target_link_libraries(call_gpt
    PRIVATE
        CURL::libcurl
        ${OPENSSL_SSL_LIBRARY}
        ${OPENSSL_CRYPTO_LIBRARY}
        ${ZLIB_LIBRARY}
        ${CJSON_LIBRARY}
        pthread  # 静态链接需要手动添加 pthread
)


# 设置静态链接标志
set_target_properties(call_gpt PROPERTIES LINK_FLAGS "-static")
# 设置编译选项（可选）
target_compile_options(call_gpt PRIVATE -Wall -Wextra -pedantic)

# 添加 include 目录（如果有头文件）
target_include_directories(call_gpt PRIVATE ${CMAKE_SOURCE_DIR}/include)

# 安装配置（可选）
install(TARGETS call_gpt DESTINATION bin)

# 项目信息
message(STATUS "Project: ${PROJECT_NAME} Version: ${PROJECT_VERSION}")
message(STATUS "CURL library found: ${CURL_INCLUDE_DIR}")
