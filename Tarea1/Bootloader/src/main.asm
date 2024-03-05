org 0x7c00
%define SECTOR_AMOUNT 0x4
jmp short start


start:
cli
xor ax, ax
mov ds, ax
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax
mov sp, 0x6ef0
sti

mov ah, 0
int 0x13

mov bx, 0x8000
mov al, SECTOR_AMOUNT
mov ch, 0
mov dh, 0
mov cl, 2
mov ah, 2
int 0x13
jmp 0x8000

times 510-($-$$) db 0
db 0x55
db 0xaa   ;numero magico