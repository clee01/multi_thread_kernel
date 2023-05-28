# multi_thread_kernel(多线程系统内核)

## 学习路线
### 准备
* 操作系统发行版本
```
cat /etc/issue

Ubuntu 18.04.1 LTS \n \l
```
* 安装`nasm`汇编语言编译程序
```
# 安装
sudo apt install nasm

# 编译命令如下
nasm -f <format> <filename> [-o <output>]
```

* 安装`qemu-system-x86`虚拟机软件
```
# 安装
sudo apt install qemu-system-x86

# 常见使用命令
qemu-system-x86_64 -hda <filename>  # 为客户机指定块存储设备
```

### Hello World Bootloader
```
# 编译
nasm -f bin ./boot_hello_world.asm -o ./boot_hello_world.bin
# 反汇编（可选的）
ndisasm ./boot_hello_world.bin
# 运行
qemu-system-x86_64 -hda ./boot_hello_world.bin
```
![image](https://github.com/clee01/multi_thread_kernel/blob/master/img/hello_world.jpg)

### 分段内存模型
* 8086段寄存器
  * `CS` - Code Segment
  * `SS` - Stack Segment
  * `DS` - Data Segment
  * `ES` - Extra Segment

* 计算物理地址
  * 物理地址 = 段寄存器 * 16 + 偏移地址
```
# 编译
nasm -f bin ./boot_segment_memory.asm -o ./boot_segment_memory.bin
# 反汇编（可选的）
ndisasm ./boot_segment_memory.bin
# 运行
qemu-system-x86_64 -hda ./boot_segment_memory.bin
```
![image](https://github.com/clee01/multi_thread_kernel/blob/master/img/hello_world.jpg)