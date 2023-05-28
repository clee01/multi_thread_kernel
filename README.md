# multi_thread_kernel(多线程系统内核)

## 学习路线
### 准备
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

### `Hello World Bootloader`
```
# 编译
nasm -f bin ./boot.asm -o ./boot.bin
# 反汇编
ndisasm ./boot.bin
# 运行
qemu-system-x86_64 -hda ./boot.bin
```
