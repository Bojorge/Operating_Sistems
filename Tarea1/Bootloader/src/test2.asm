use16       ; Indica que se usará el modo de 16 bits
org 0x7C00  ; Dirección de inicio del código de arranque

SECTION .data
    countdown db 60  ; Variable para almacenar el tiempo restante
    
SECTION .text
    mov ah, 0x00   ; Limpia la pantalla
    mov al, 0x03   ; Función para seleccionar modo de video de texto de 80x25
    int 0x10       ; Llama a la interrupción del BIOS para cambiar el modo de video
    
    mov si, countdown  ; Carga la dirección de memoria de countdown en SI
    
countdown_loop:
    
    

    ; Convierte el número en countdown a un formato de dos dígitos y lo muestra en pantalla
    mov ah, 0x0E        ; Función de teletipo de BIOS
    mov bh, 0x00        ; Página de pantalla (0 para el modo de texto)
    mov bl, 0x07        ; Color de texto (blanco sobre negro)
    mov al, [si]        ; Carga el valor de countdown en AL
    add al, '0'         ; Convierte el número a su representación ASCII
    int 0x10            ; Llama a la interrupción de BIOS para mostrar el carácter
    
    ; Resta 1 al valor de countdown
    dec byte [si]
    
    ; Espera 1 segundo
    call delay

    ; Vuelve al inicio del bucle si countdown no ha alcanzado cero
    cmp byte [si], 0
    jne countdown_loop
    
    ; Si countdown alcanza cero, termina el programa
    jmp $  ; Bucle infinito


delay:
    mov cx, 0xFFFF  ; Inicializa el contador para un retraso

delay_loop:
    dec cx          ; Decrementa el contador
    jnz delay_loop  ; Salta de nuevo si el contador no es cero
    ret             ; Retorna

times 510 - ($ - $$) db 0  ; Rellena el resto del sector de arranque con ceros
dw 0xAA55                    ; Firma de arranque
