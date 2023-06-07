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

### `BPB`应用
![image](https://github.com/clee01/multi_thread_kernel/blob/master/img/mbr.jpg)
* `MBR`：主引导记录，446字节
* 主分区（只有四个，16bytes * 4）
  * 活动分区，用来启动操作系统
  * 扩展分区，可以建立逻辑分区
* 魔数（2bytes）
  * 0x55AA，如果这里被破坏，系统就废了

`BIOS`+`MBR`启动过程：
* 在打开电源时，计算机开始自检过程，从`BIOS`中载入必要的指令，然后进行一系列的自检操作（进行硬件的初始化检查，包括内存、硬盘、键盘等），同时在屏幕上显示信息。自检完成后，根据`CMOS`的设置，`BIOS`加载启动盘，将主引导记录（`MBR`，`Master Boot Record`）中的引导代码载入内存，接着，启动过程由`MBR`来执行。启动代码搜索`MBR`中的分区表（`DPT`），找出活动分区，将活动分区的第一个扇区（`VBR`，`Volumn Boot Record`，卷引导记录，活动分区的`VBR`也叫`DBR`）中的引导代码载入内存`0x07C00`处
* `MBR`：计算机启动后从可启动介质上首先装入内存并且执行的代码
* `DBR`：`Dos Boot Record`通常包括一个引导程序和一个被称为`BPB`（`BIOS Parameter Block`）的本分区参数记录表。引导程序的主要任务是当`MBR`将系统控制权交给它时，判断本分区根目录前两个文件是不是操作系统的引导文件。如果确定存在，就把它读入内存并把控制权交给该文件。`BPB`参数块记录着本分区的起始扇区、结束扇区、文件存储格式、硬件介质描述符、根目录大小、`FAT`个数和分配单元的大小等重要参数
* `VBR`：每个非扩展分区以及逻辑分区的第一个扇区（`VBR`包括`DBR`（非扩展分区）和`EBR`），可存放小段程序，用于启动该分区上某程序或者操作系统（`DBR`）
* 扩展分区中的每个逻辑驱动器都存在一个类似于`MBR`的扩展引导记录（`EBR`，`Extended Boot Record`）
```
# 编译
nasm -f bin ./boot_real_hardware.asm -o ./boot_real_hardware.bin
# 反汇编（可选的）
ndisasm ./boot_real_hardware.bin
# 列出所有可用的分区类型
sudo fdisk -l
# 读取、转换并输出数据 !注意，确保sdb(这里是USB)上没有重要内容
sudo dd if ./boot_real_hardware.bin of=/dev/sdb
# 后续可以用这个USB作为bootloader启动计算机
```

### 中断向量表
```
# 编译
nasm -f bin ./boot_interrupt_vector_table.asm -o ./boot_interrupt_vector_table.bin
# 反汇编（可选的）
ndisasm ./boot_interrupt_vector_table.bin
# 运行
qemu-system-x86_64 -hda ./boot_interrupt_vector_table.bin
```
![image](https://github.com/clee01/multi_thread_kernel/blob/master/img/interrupt_vector_table.jpg)

### 读取硬盘
[DISK - WRITE DISK SECTOR(S)](http://www.ctyme.com/intr/rb-0608.htm)
```
# 编译
nasm -f bin ./boot_read_hard_disk.asm -o ./boot_read_hard_disk.bin
# 查看message.txt
cat message.txt

bingo!

# 追加message.txt到boot_read_hard_disk.bin
dd if=./message.txt >> ./boot_read_hard_disk.bin
# 追加sector到boot_read_hard_disk.bin
dd if=/dev/zero bs=512 count=1 >> ./boot_read_hard_disk.bin
# 安装十六进制编辑器
sudo apt install bless
# 查看bin文件
bless boot_read_hard_disk.bin
```
![image](https://github.com/clee01/multi_thread_kernel/blob/master/img/boot_read_hard_disk_bin.jpg)
```
# 运行
qemu-system-x86_64 -hda ./boot_read_hard_disk.bin
```
![image](https://github.com/clee01/multi_thread_kernel/blob/master/img/run.jpg)

### 保护模式
```
# 编译
nasm -f bin ./boot_protected_mode.asm -o ./boot_protected_mode.bin
# 安装gdb
sudo apt install gdb
# 运行gdb
gdb
# 连接调试
(gdb) target remote | qemu-system-x86_64 -hda ./boot_protected_mode.bin -S -gdb stdio
```
![image](https://github.com/clee01/multi_thread_kernel/blob/master/img/target.jpg)
```
# 继续，可以看到程序运行就停在了"Booting from Hard Disk..."下一行处
(gdb) c
Continuing.
```
![image](https://github.com/clee01/multi_thread_kernel/blob/master/img/continue.jpg)
```
# ctrl-c中断，可以看到程序是运行在0x7c7f处被中断的
^C
```
![image](https://github.com/clee01/multi_thread_kernel/blob/master/img/ctrl_c.jpg)
```
# 显示汇编窗口，可以看到0x7c7f处是"jmp $"指令，符合预期
(gdb) layout asm
```
![image](https://github.com/clee01/multi_thread_kernel/blob/master/img/layout_asm.jpg)
```
# 可以看到对应各个寄存器设定的值也是和程序中所设定的一致
info registers
```
![image](https://github.com/clee01/multi_thread_kernel/blob/master/img/info_registers.jpg)

### 开启A20
A20或地址线20是构成基于`x86`的计算机系统总线的电线之一。A20线特别用于传输地址总线上的第21位。
```
in al, 0x92
or al, 2
out 0x92, al
```

### 加载32位内核程序至内存
```
# 脚本编译
./build.sh
# 运行gdb
gdb
# 载入symbol file
(gdb) add-symbol-file build/kernelfull.o 0x100000
# 打断点
(gdb) break _start
# 连接调试
(gdb) target remote | qemu-system-x86_64 -hda ./bin/os.bin -S -gdb stdio
# 开始执行（可以看到停在了断点处）
(gdb) c
Continuing.

Breakpoint 1, 0x0000000000100000 in _start ()
# 显示汇编窗口
(gdb) layout asm
```
![image](https://github.com/clee01/multi_thread_kernel/blob/master/img/break.jpg)
```
# 单步调试
(gdb) stepi
```
![image](https://github.com/clee01/multi_thread_kernel/blob/master/img/stepi.jpg)

### 运行C代码
```
...
# 打断点
(gdb) break kernel_main
```
![image](https://github.com/clee01/multi_thread_kernel/blob/master/img/break_kernel_main.jpg)
```
# 查看当前堆栈
(gdb) bt
```
![image](https://github.com/clee01/multi_thread_kernel/blob/master/img/backtrace.jpg)
```
# 单步调试
(gdb) stepi
```
![image](https://github.com/clee01/multi_thread_kernel/blob/master/img/stepi_1.jpg)

### 屏幕显示Hello world

### 实现中断描述符表

