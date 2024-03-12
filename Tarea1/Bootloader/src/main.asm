org 0x7c00
%define SECTOR_AMOUNT 0x4
jmp short start


;; Variables
initial_message: db 'Presiona Enter para iniciar el juego'
.len equ ($-initial_message)

start:
    ;; Ocultar cursor
    mov ah, 1       ; Configurar la forma del cursor en modo texto
    mov cx, 2607h   ; CH bit 5 = invisible (2) & scan line start (6), CL = scan line end (7)
    int 10h

    ;; Configurar memoria de video
    mov ax, 0B800h  
    mov es, ax      ; ES = Memoria de video en modo texto color (B8000)
    xor di, di      ; ES:DI = Puntero a la memoria de video

    ;; Limpiar pantalla con fondo azul
    mov ax, 1020h       ; AH = color de fondo (1 = azul), AL = espacio ASCII (0)
    mov cx, 80*25       ; Número de caracteres en la pantalla
    rep stosw           ; Escribir todos los caracteres en la memoria de video
    xor di, di          ; Resetear di

    ;; Imprimir mensaje
    mov si, initial_message
    mov cx, initial_message.len
    call write_string

wait_for_enter:
    ;; Esperar la tecla "Enter"
    mov ah, 0       ; Función 0 de la interrupción 16h: leer tecla presionada
    int 16h         ; Llamar a la interrupción del teclado BIOS
    cmp al, 0Dh     ; Verificar si la tecla presionada fue "Enter"
    jne wait_for_enter  ; Si no fue "Enter", continuar esperando

    ;; Limpiar pantalla antes de continuar
    mov ax, 1020h   ; AH = color de fondo (1 = azul), AL = espacio ASCII (0)
    mov cx, 80*25   ; Número de caracteres en la pantalla
    xor di, di      ; Resetear di
    rep stosw       ; Escribir todos los caracteres en la memoria de video

    ;; Saltar a la rutina para jugar
    jmp to_play



to_play:
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


;; Subroutine to write a string
;;  Inputs:
;;    SI = address of string
;;    CX = length of string
write_string:
    mov ah, 17h     ; BG color (1 = blue) FG color (7 = light gray) 
    .loop:
        lodsb       ; mov AL, [DS:SI]; inc SI
        stosw       ; Write character to video memory (ES:DI)
    loop .loop      ; decrement CX; if CX != 0, jmp to label
    ret     


times 510-($-$$) db 0
db 0x55
db 0xaa   ;numero magico