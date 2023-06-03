ORG 0x7c00  ; 0x0000:7c00
BITS 16  ; 引导程序，默认是16位模式

start:
    mov si, message
    call print
    jmp $

print:
    mov bx, 0
.loop:
    lodsb
    cmp al, 0
    je .done
    call print_char
    jmp .loop
.done:
    ret

print_char:
    mov ah, 0eh
    int 0x10
    ret

message: db 'Hello World!', 0

times 510-($ - $$) db 0
dw 0xAA55  ; 主引导扇区代码