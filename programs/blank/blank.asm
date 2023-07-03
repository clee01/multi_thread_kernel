[BITS 32]

section .asm

global _start

_start:

label:
    mov eax, 0
    int 0x80
    
    jmp $