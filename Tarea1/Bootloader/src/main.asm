bits 16         ; Modo de compilación de 16 bits

org 0x7C00      ; Dirección donde se va a cargar el programa

jmp INICIO      ; Se indica donde debe iniciar el programa

;; Variables
initial_message db 'Presione Enter para iniciar', 0
initial_message_len equ $ - initial_message

INICIO:
    ;; Print strings
    mov si, initial_message
    call write_string

    ;; Esperar la entrada del usuario
    call wait_for_enter

    jmp $         ; Bucle infinito

;; Subrutina para escribir una cadena de caracteres
write_string:
    mov ah, 0x0E  ; Función 0x0E de la int 10h: Imprimir carácter en pantalla
.loop:
    lodsb         ; Carga el siguiente carácter de la cadena en AL y avanza SI
    cmp al, 0     ; Comprueba si es el byte nulo (fin de la cadena)
    je .done      ; Si es así, termina la impresión
    int 0x10      ; Llama a la interrupción de video BIOS
    jmp .loop     ; Repite el bucle para el siguiente carácter
.done:
    ret           ; Retorna de la subrutina

;; Subrutina para esperar a que se presione la tecla Enter
wait_for_enter:
    mov ah, 0x00  ; Función 0x00 de la int 16h: Esperar una tecla
    int 0x16      ; Llama a la interrupción del teclado BIOS
    cmp ah, 0x1C  ; Comprueba si la tecla presionada fue Enter
    jne wait_for_enter ; Si no, vuelve a esperar
    jmp 0x8000           ; Pasa a la direccion 0x8000 donde inicia el otro archivo

;; Espacio de relleno hasta el sector de arranque
times 510-($-$$) db 0

dw 0xAA55      ; Firma del sector de arranque
