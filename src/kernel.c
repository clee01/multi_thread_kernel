#include "kernel.h"
#include <stdint.h>
#include <stddef.h>
#include "idt/idt.h"
#include "io/io.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "disk/disk.h"
#include "fs/pparser.h"
#include "string/string.h"
#include "disk/streamer.h"
#include "fs/file.h"
#include "gdt/gdt.h"
#include "config.h"
#include "memory/memory.h"
#include "task/tss.h"

uint16_t* video_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;

uint16_t terminal_make_char(char c, char colour) {
    return (colour << 8) | c;
}

void terminal_putchar(int x, int y, char c, char colour) {
    video_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c, colour);
}

void terminal_writechar(char c, char colour) {
    if (c == '\n') {
        terminal_row += 1;
        terminal_col = 0;
        return;
    }

    terminal_putchar(terminal_col, terminal_row, c, colour);
    terminal_col += 1;
    if (terminal_col >= VGA_WIDTH) {
        terminal_col = 0;
        terminal_row += 1;
    }
}

void terminal_initialize() {
    video_mem = (uint16_t*)(0xB8000);
    terminal_row = 0;
    terminal_col = 0;
    for (int y = 0; y < VGA_HEIGHT; ++y) {
        for (int x = 0; x < VGA_WIDTH; ++x) {
            terminal_putchar(x, y, ' ', 0);
        }
    }
}

void print(const char* str) {
    size_t len = strlen(str);
    for (int i = 0; i < len; ++i) {
        terminal_writechar(str[i], 15);
    }
}

extern void problem();

static struct paging_4gb_chunk* kernel_chunk = 0;

void panic(const char* msg) {
    print(msg);
    while (1) {}
}

struct tss tss;
struct gdt gdt_real[PEACHOS_TOTAL_GDT_SEGMENTS];
struct gdt_structured gdt_structured[PEACHOS_TOTAL_GDT_SEGMENTS] = {
    {.base = 0x00, .limit = 0x00, .type = 0x00},  // Null Segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0x9A},  // Kernel code segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0x92},  // Kernel data segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0xf8},  // User code segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0xf2},  // User data segment
    {.base = (uint32_t)&tss, .limit = sizeof(tss), .type = 0xE9}  // TSS Segment
};

void kernel_main() {
    terminal_initialize();
    print("Hello world!\ntest");

    memset(gdt_real, 0x00, sizeof(gdt_real));
    gdt_structured_to_gdt(gdt_real, gdt_structured, PEACHOS_TOTAL_GDT_SEGMENTS);

    // Load the gdt
    gdt_load(gdt_real, sizeof(gdt_real));

    // panic("The system cannot continue! ERROR ERROR\n");

    // Initialize the heap
    kheap_init();

    // Initialize filesystem
    fs_init();

    // Search and initialize the disks
    disk_search_and_init();

    // Initialize the interrupt descriptor table
    idt_init();

    // problem();

    // outw(0x60, 0xffff);

    // Setup the TSS
    memset(&tss, 0x00, sizeof(tss));
    tss.esp0 = 0x600000;
    tss.ss0 = KERNEL_DATA_SELECTOR;

    // Load the TSS
    tss_load(0x28);

    // Setup paging
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

    // Switch to kernel paging chunk
    paging_switch(paging_4gb_chunk_get_directory(kernel_chunk));

    // char* ptr = kzalloc(4096);
    // paging_set(paging_4gb_chunk_get_directory(kernel_chunk), (void*)0x1000, (uint32_t)ptr | PAGING_ACCESS_FROM_ALL | PAGING_IS_PRESENT | PAGING_IS_WRITABLE);

    // Enable paging
    enable_paging();
    // char* ptr2 = (char*)0x1000;
    // ptr2[0] = 'A';
    // ptr2[1] = 'B';
    // print(ptr2);
    // print(ptr);

    // char buf[512];
    // disk_read_sector(0, 1, buf);

    // Enable the system interrupts
    enable_interrupts();

    // struct path_root* root_path = pathparser_parse("1:/bin/shell.exe", NULL);
    // if (root_path) {}

    // struct disk_stream* stream = diskstream_new(0);
    // diskstream_seek(stream, 0x1fe);
    // unsigned char* c = kzalloc(3);
    // diskstream_read(stream, c, 3);

    // p disk_get(0)
    // Could not fetch register "orig_eax"; remote failure reply 'E14'

    int fd = fopen("0:/hello.txt", "r");
    if (fd) {
        // print("\nWe opened hello.txt\n");
        // fseek(fd, 2, SEEK_SET);
        // char buf[13];
        // fread(buf, 10, 1, fd);
        // buf[12] = 0x00;
        // print(buf);

        struct file_stat s;
        fstat(fd, &s);
        fclose(fd);

        print("\ntesting\n");
    }
    while (1) {}
}