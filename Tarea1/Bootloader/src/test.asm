bits 16  ;modo de compilacion 16 bits

org 0x8000   ;dirección donde se va a cargar el programa

jmp INICIO    ;se indica donde debe iniciar el programa

sectiion .data
    msg db "Moi", 0x0a
    len equ $ - msg

section .text
INICIO:
    mov al, "x" ;se muestra la letra x
    mov ah, 0x0e    ;indica a la bios que mueva 1 pocision el cursor
    int 0x10    ;interrupcion a la bios - Invoca servicios de vídeo de la ROM BIOS -Muestra registros de paleta
    hlt         ;detiene la ejecucion (halt)


    mov eax, 4
    mov ebx, 1
    mov ecx, msg
    mov edx, len
    int 0x80
    mov eax, 1
    mov ebx, 0
    int 0x80


;los espacios de memoria que no se usan se hacen cero
times 510-($-$$) db 0

dw 0aa55h   ;firma - mbr
