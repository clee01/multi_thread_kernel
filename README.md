# multi_thread_kernel(多线程系统内核)

## 学习路线
### 准备
* 操作系统发行版本
```powershell
cat /etc/issue

Ubuntu 18.04.1 LTS \n \l
```
* 安装`nasm`汇编语言编译程序
```powershell
# 安装
sudo apt install nasm

# 编译命令如下
nasm -f <format> <filename> [-o <output>]
```

* 安装`qemu-system-x86`虚拟机软件
```powershell
# 安装
sudo apt install qemu-system-x86

# 常见使用命令
qemu-system-x86_64 -hda <filename>  # 为客户机指定块存储设备
```

### Hello World Bootloader
```powershell
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
```powershell
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
```powershell
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
```powershell
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
```powershell
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
```powershell
# 运行
qemu-system-x86_64 -hda ./boot_read_hard_disk.bin
```
![image](https://github.com/clee01/multi_thread_kernel/blob/master/img/run.jpg)

### 保护模式
```powershell
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
```powershell
# 继续，可以看到程序运行就停在了"Booting from Hard Disk..."下一行处
(gdb) c
Continuing.
```
![image](https://github.com/clee01/multi_thread_kernel/blob/master/img/continue.jpg)
```powershell
# ctrl-c中断，可以看到程序是运行在0x7c7f处被中断的
^C
```
![image](https://github.com/clee01/multi_thread_kernel/blob/master/img/ctrl_c.jpg)
```powershell
# 显示汇编窗口，可以看到0x7c7f处是"jmp $"指令，符合预期
(gdb) layout asm
```
![image](https://github.com/clee01/multi_thread_kernel/blob/master/img/layout_asm.jpg)
```powershell
# 可以看到对应各个寄存器设定的值也是和程序中所设定的一致
info registers
```
![image](https://github.com/clee01/multi_thread_kernel/blob/master/img/info_registers.jpg)

### 开启A20
A20或地址线20是构成基于`x86`的计算机系统总线的电线之一。A20线特别用于传输地址总线上的第21位。
```powershell
in al, 0x92
or al, 2
out 0x92, al
```

### 加载32位内核程序至内存
```powershell
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
```powershell
# 单步调试
(gdb) stepi
```
![image](https://github.com/clee01/multi_thread_kernel/blob/master/img/stepi.jpg)

### 运行C代码
```powershell
...
# 打断点
(gdb) break kernel_main
```
![image](https://github.com/clee01/multi_thread_kernel/blob/master/img/break_kernel_main.jpg)
```powershell
# 查看当前堆栈
(gdb) bt
```
![image](https://github.com/clee01/multi_thread_kernel/blob/master/img/backtrace.jpg)
```powershell
# 单步调试
(gdb) stepi
```
![image](https://github.com/clee01/multi_thread_kernel/blob/master/img/stepi_1.jpg)

### 屏幕显示Hello world

### 实现中断描述符表

### 实现I/O读取接口
```c
// src/io/io.h
// 函数原型
unsigned char insb(unsigned short port);
unsigned short insw(unsigned short port);

void outb(unsigned short port, unsigned char val);
void outw(unsigned short port, unsigned short val);
```

### 可编程中断处理器（PIC）实现
```x86asm
setup_pic:
    ; Initialize some flags in the PIC's
    mov al, 00010001b  ; b4=1: Init; b3=0: Edge; b1=0: Cascade; b0=1: Need 4th init step
    out 0x20, al  ; Tell master

    mov al, 0x20  ; Master IRQ0 should be on INT 0x20 (Just after intel exception)
    out 0x21, al

    mov al, 00000001b  ; b4=0: FNM; b3-2=00: Master/Slave set by hardware; b1=0: Not AEOI; b0=1: x86 mode
    out 0x21, al

    ret
```

### 自定义堆实现
```c
void* ptr = kmalloc(50);  // 0x1000000
void* ptr2 = kmalloc(5000);  // 0x1001000
void* ptr3 = kmalloc(5600);  // 0x1003000
kfree(ptr);
void* ptr4 = kmalloc(50);  // 0x1000000
```
> 注意：此时需要用`qemu-system-i386`执行，模拟`32`位`Intel`架构`CPU`

### 创建中断使能函数
```c
void enable_interrupts();
void disable_interrupts();
```

删除`kernel.asm`中的`sti`语句，可能有潜在的`panic`风险

### 分页机制
```x86asm
; 开启分页
[BITS 32]

section .asm

global paging_load_directory
global enable_paging

paging_load_directory:
    push ebp
    mov ebp, esp
    mov eax, [ebp+8]
    mov cr3, eax
    pop ebp
    ret

enable_paging:
    push ebp
    mov ebp, esp
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    pop ebp
    ret
```

### 调整页表

### 磁盘读取（`C`）
```c
outb(0x1F6, (lba >> 24) | 0xE0);
outb(0x1F2, total);
outb(0x1F3, (unsigned char)(lba & 0xFF));
outb(0x1F4, (unsigned char)(lba >> 8));
outb(0x1F5, (unsigned char)(lba >> 16));
outb(0x1F7, 0x20);

unsigned short* ptr = (unsigned short*)buf;
for (int b = 0; b < total; ++b) {
    // Wait for the buffer to be ready
    char c = insb(0x1F7);
    while (!(c & 0x08)) {
        c = insb(0x1F7);
    }

    // Copy from hard disk to memory
    for (int i = 0; i < 256; ++i) {
        *ptr = insw(0x1F0);
        ptr++;
    }
}
```

### 改进磁盘驱动程序
```c
// in /src/disk/disk.h
void disk_search_and_init();
struct disk* disk_get(int index);
int disk_read_block(struct disk* idisk, unsigned int lba, int total, void* buf);
```

### 创建路径解析器
```c
// in src/fs/pparser.h
void pathparser_free(struct path_root* root);
struct path_root* pathparser_parse(const char* path, const char* current_directory_path);
```

### 创建一个磁盘流
```c
// in src/disk/streamer.h
struct disk_stream* diskstream_new(int disk_id);
int diskstream_seek(struct disk_stream* stream, int pos);
int diskstream_read(struct disk_stream* stream, void* out, int total);
void diskstream_close(struct disk_stream* stream);
```

### 创建`FAT`文件系统
```x86asm
; FAT16 Header
OEMIdentifier           db 'PEACHOS '
BytesPerSector          dw 0x200
SectorsPerCluster       db 0x80
ReservedSectors         dw 200
FATCopies               db 0x02
RootDirEntries          dw 0x40
NumSectors              dw 0x00
MediaType               db 0xF8
SectorsPerFat           dw 0x100
SectorsPerTrack         dw 0x20
NumberOfHeads           dw 0x40
HiddenSectors           dd 0x00
SectorsBig              dd 0x773594

; Extended BPB (Dos 4.0)
DriveNumber             db 0x80
WinNTBit                db 0x00
Signature               db 0x29
VolumeID                dd 0xD105
VolumeIDString          db 'PEACHOS BOO'
SystemIDString          db 'FAT16   '
```

### 实现`VFS`核心功能

### 实现`FAT16`文件系统驱动核心功能

### 实现`FAT16`各类结构

### 实现`FAT16`解析函数

### 实现`FAT16` `fopen`函数
```c
// in src/fs/file.c
int fopen(const char* filename, const char* mode_str);

// in src/fs/fat/fat16.c
void* fat16_open(struct disk* disk, struct path_part* path, FILE_MODE mode);
```

### 实现`FAT16` `fread`函数
```c
// in src/fs/file.c
int fread(void* ptr, uint32_t size, uint32_t nmemb, int fd);

// in src/fs/fat/fat16.c
int fat16_read(struct disk* disk, void* descriptor, uint32_t size, uint32_t nmemb, char* out_ptr);
```

### 实现`FAT16` `fseek`函数
```c
// in src/fs/file.c
int fseek(int fd, int offset, FILE_SEEK_MODE whence);

// in src/fs/fat/fat16.c
int fat16_seek(void *private, uint32_t offset, FILE_SEEK_MODE seek_mode);
```

### 实现`FAT16` `fstat`函数
```c
// in src/fs/file.c
int fstat(int fd, struct file_stat* stat);

// in src/fs/fat/fat16.c
int fat16_stat(struct disk* disk, void* private, struct file_stat* stat);
```

### 实现`FAT16` `fclose`函数
```c
// in src/fs/file.c
int fclose(int fd);


```