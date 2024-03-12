use16        ; Indica que se usará el modo de 16 bits
org 0x7C00   ; Dirección de inicio del código de arranque

SECTION .text
    mov dx, 0       ; Inicializa DX a 0

    mov ax, 60      ; Número inicial para la cuenta regresiva (por ejemplo, 60)
    mov cx, 1       ; Contador de tiempo para mostrar la cuenta regresiva (por ejemplo, 5 segundos)

countdown_loop:
    call print_number  ; Llama a la función para imprimir el número
    dec cx            ; Reduce el contador de tiempo
    test cx, cx       ; Comprueba si el contador de tiempo ha alcanzado cero
    jz done           ; Si el contador de tiempo es cero, termina el programa
    jmp countdown_loop ; Si el contador de tiempo no es cero, continúa la cuenta regresiva

print_number:
    mov bx, 10         ; Base para la división (número decimal)
    mov si, ax         ; Carga el número actual en SI
    mov cx, 0          ; Contador para el número de dígitos procesados

print_digit_loop:
    xor dx, dx         ; Borra DX para dividir (DX:AX / BX -> AX:DX)
    div bx             ; Divide SI por BX; resultado en AX, residuo en DX
    add dl, '0'        ; Convierte el dígito en ASCII
    mov ah, 0x0E       ; Función de teletipo de BIOS
    mov bh, 0x00       ; Página de pantalla (0 para el modo de texto)
    mov bl, 0x07       ; Color de texto (blanco sobre negro)
    mov al, dl         ; Carga el dígito convertido en AL
    int 0x10           ; Llama a la interrupción de BIOS para mostrar el dígito
    inc cx             ; Incrementa el contador de dígitos

    test ax, ax        ; Comprueba si quedan más dígitos para procesar
    jnz print_digit_loop  ; Si hay más dígitos, continúa el bucle

    ret

done:
    mov eax, 1         ; Código de llamada al sistema para 'exit'
    xor ebx, 0
    int 0x80

times 510 - ($ - $$) db 0  ; Rellena el resto del sector de arranque con ceros
dw 0xAA55                    ; Firma de arranque
