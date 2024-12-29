# ai-shell



```shell

sudo apt-get install libssl-dev
sudo apt-get install zlib1g-dev

git clone https://github.com/DaveGamble/cJSON.git
mkdir build
cd build
cmake .. -DBUILD_SHARED_LIBS=OFF
make
make install

# 可能有用，不确定
# sudo apt-get update
# sudo apt-get install libcjson-dev


# 查找 OpenSSL 静态库
find /usr/lib /usr/local/lib -name "libssl.a" -o -name "libcrypto.a"

# 查找 Zlib 静态库
find /usr/lib /usr/local/lib -name "libz.a"

sudo find /usr /lib /lib64 -name "libcjson*.a"


wget https://curl.se/download/curl-8.0.0.tar.gz
tar -xzvf curl-8.0.0.tar.gz
cd curl-8.0.0

./configure --disable-shared --enable-static --with-ssl
make
sudo make install

```



```shell
# 获取当前时间，格式化为 yyyy-mm-dd_hh-mm 
TIMESTAMP=$(date +"%Y-%m-%d_%H-%M")
# 定义日志文件路径，拼接时间戳和进程号 
LOG_FILE="/tmp/shell_log_${TIMESTAMP}_$$.log"
# 获取父进程 ID
PARENT_PID=$PPID
# 查询父进程的命令信息
PARENT_INFO=$(ps -o comm= -p "$PARENT_PID")
# 检查父进程信息是否包含 "script" 关键字

if [[ "$PARENT_INFO" == *"script"* ]]; then
    echo "This shell was started by script."
else
    echo "This shell was not started by script."
    script --flush -q -a "$LOG_FILE"
    SCRIPT_PID=$!
    trap 'rm -f $LOG_FILE; kill $SCRIPT_PID' EXIT
fi
```# ai-shell
