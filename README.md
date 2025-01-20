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






前向模式自动微分（Forward Mode AD）适用于 **输入维度小、输出维度大的情况**，这是由于其计算过程和复杂度与输入变量数量的关系。下面是原因的详细解释：

### 1. **前向模式的工作机制**
- 前向模式通过引入 **双数（dual numbers）** 来计算函数的值和导数。  
  对于一个函数 \( f: \mathbb{R}^n \to \mathbb{R}^m \)，每个输入变量 \( x_i \) 都被表示为：
  \[
  x_i = (x_i, 1) \quad \text{（种子变量，表示 } \frac{\partial x_i}{\partial x_i} = 1\text{）}
  \]
  其他变量 \( x_j \) 对 \( x_i \) 的导数初始为 0。

- 当计算一个函数时，前向模式逐步计算每个输入对输出的导数。这意味着：
  - **每个输入变量单独计算其对应的输出导数**。

### 2. **复杂度分析**
对于一个函数 \( f: \mathbb{R}^n \to \mathbb{R}^m \)，自动微分的复杂度由输入和输出的数量决定。

#### **前向模式**
- 计算每个输入变量的导数。每增加一个输入变量，必须从头开始计算一遍所有输出导数。
- **复杂度为 \( O(n) \) 对于每个输出**。

#### **反向模式**
- 反向模式使用反向传播技术，先计算输出值，然后反向传播计算每个输出相对于所有输入的导数。
- 复杂度取决于 **输出的数量**，即 **对于每个输出复杂度为 \( O(m) \)**。

### 3. **适用场景**
- **前向模式的强项**：当输入变量 **数量小** 时，计算每个输入变量的导数仍然是可行的。适合输入维度较小但需要对**多个输出**求导的场景。
  - 例如：标量到向量映射，计算多个输出值。
- **反向模式的强项**：当输出变量数量为 1 时，它能一次性计算所有输入的梯度，适合输出为**标量**的情况，如损失函数。

### 4. **例子对比**
#### 输入维度小、输出维度大
计算一个函数 \( f: \mathbb{R}^2 \to \mathbb{R}^5 \)：
- 前向模式：计算每个输入 \( x_1, x_2 \) 对所有 5 个输出的导数，一共需要 2 次计算。
- 反向模式：需要为每个输出单独反向传播，共 5 次。

### 5. **总结**
- **前向模式** 随输入维度线性增长。输入越多，计算成本越高，但能同时得到所有输出对单个输入的导数。因此适合**少量输入**，但每个输入有多个导数输出的情况。
- **反向模式** 随输出维度线性增长，适合输出为**标量**的场景，比如神经网络的损失函数计算。
