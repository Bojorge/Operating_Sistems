use16       ; use 16 bit code when assembling
org 8000h   ; Set memory offsets to start here

;; CONSTANTS -----------
VIDMEM      equ 0B800h       ; Color text mode VGA memory location
SCREENW     equ 160          ; Screen width
SCREENH     equ 50           ; Screen height
PLAYERCHAR  equ 248          ; ASCII code for player character (°)
PLAYERCOLOR equ 02h          ; Green color for the player

win_msg_buffer db "WIN$"
lose_msg_buffer db "LOSE$"

;; VARIABLES -----------
playerX:    dw 80            ; Player X position
playerY:    dw 24            ; Player Y position
spaceFlag:  dw 0             ; Space key flag

;; LOGIC ----------------
setup_game:
    ;; Set up video mode
    mov ax, 13h         ; Set video mode to 160x50 text mode, 16 colors
    int 10h

    ;; Set up video memory
    mov ax, VIDMEM
    mov es, ax          ; ES:DI <- video memory (0B800:0000 or B8000)

    ;; Initialize player position
    mov ax, [playerX]
    mov bx, [playerY]

    ;; Draw player
    call draw_player
    
    ;; Draw countdown numbers
    mov dx, 60           ; Start countdown from 60
    mov di, 0           ; Starting position for countdown

draw_countdown:
    mov ah, 0Ch         ; Set character and attribute function
    add al, '0'         ; Convert number to ASCII
    mov cx, 2           ; Number of characters to print
    mov bh, 00h         ; Video page number
    mov bl, 0Fh         ; White color attribute
    mov ah, 09h         ; BIOS function to write character and attribute
    int 10h             ; Call BIOS interrupt
    add di, 2           ; Move to next character position
    dec dx              ; Decrement countdown
    jnz draw_countdown ; Continue until countdown reaches 0
    

game_loop:
    ;; Get player input
    mov ah, 0           ; BIOS keyboard input function
    int 16h             ; Wait for key press

    cmp ah, 48h         ; Up arrow key
    je move_up
    cmp ah, 50h         ; Down arrow key
    je move_down
    cmp ah, 4Bh         ; Left arrow key
    je move_left
    cmp ah, 4Dh         ; Right arrow key
    je move_right

    cmp ah, 10h         ; Q key (SurOeste)
    je move_southwest
    cmp ah, 12h         ; E key (SurEste)
    je move_southest
    cmp ah, 1Eh         ; A key (NorOeste)
    je move_northwest
    cmp ah, 20h         ; D key (NorEste)
    je move_northest

    cmp ah, 2Ch         ; Space key
    je toggle_Flag       

    jmp game_loop       ; Wait for valid key press


toggle_Flag:
    mov dl, [spaceFlag]    
    xor dl, 1              
    mov [spaceFlag], dl  
    jmp game_loop

move_southwest:
    inc word [playerY]              ; Move player down
    dec word [playerX]              ; Move player left
    mov ah, 33h                    ;Color
    ;cmp byte [spaceFlag], 1
    ;je delete_path
    call draw_player
    jmp game_loop

move_southest:
    inc word [playerY]              ; Move player down
    inc word [playerX]              ; Move player right
    mov ah, 44h                    ;Color
    ;cmp byte [spaceFlag], 1
    ;je delete_path
    call draw_player
    jmp game_loop

move_northwest:
    dec word [playerY]              ; Move player up
    dec word [playerX]              ; Move player left
    mov ah, 99h                    ;Color
    ;cmp byte [spaceFlag], 1
    ;je delete_path
    call draw_player
    jmp game_loop

move_northest:
    dec word [playerY]              ; Move player up
    inc word [playerX]              ; Move player right
    mov ah, 66h                    ;Color
    ;cmp byte [spaceFlag], 1
    ;je delete_path
    call draw_player
    jmp game_loop

move_up:
    cmp word [playerY], 0         ; Check if at top of screen
    jle game_loop                  ; If at top, dont move up
    dec word [playerY]             ; Move player up
    mov ah, 87h                    ;Color
    ;cmp byte [spaceFlag], 1
    ;je delete_path
    call draw_player
    jmp game_loop

move_down:
    cmp word [playerY], SCREENH-1  ; Check if at bottom of screen
    jge game_loop                  ; If at bottom, dont move down
    inc word [playerY]             ; Move player down
    mov ah, 64h                    ;Color
    ;cmp byte [spaceFlag], 1
    ;je delete_path
    call draw_player
    jmp game_loop

move_left:
    cmp word [playerX], 0         ; Check if at left edge of screen
    jle game_loop                  ; If at left edge, dont move left
    dec word [playerX]             ; Move player left
    mov ah, 45h                    ;Color
    ;cmp byte [spaceFlag], 1
    ;je delete_path
    call draw_player
    jmp game_loop

move_right:
    cmp word [playerX], SCREENW-1  ; Check if at right edge of screen
    jge game_loop                  ; If at right edge, dont move right
    inc word [playerX]             ; Move player right
    mov ah, 31h                    ;Color
    ;cmp byte [spaceFlag], 1
    ;je delete_path
    call draw_player
    jmp game_loop

delete_path:
    mov ah, 11h                    ;Mismo color del fondo
    ret

draw_player:
    mov di, [playerY]              ; Calculate offset in video memory
    imul di, SCREENW
    add di, [playerX]

    mov al, PLAYERCHAR             ; Draw player character
    ;mov ah, PLAYERCOLOR            ; Set player color
    stosw                          ; Store character and color in video memory
    ret

clear_screen_and_print_win:
    ;; Clear the screen
    mov ax, 0600h              ; Function 06h - Scroll up window
    mov bh, 00h                ; Display all attributes
    mov cx, 0000h              ; Upper-left corner
    mov dx, SCREENW * SCREENH  ; Lower-right corner
    mov es, ax                 ; ES:DI <- video memory (0B800:0000 or B8000)
    mov di, 0000h              ; Upper-left corner
    mov ax, 0720h              ; Space character with white on green attribute
    mov cx, 5                  ; Total number of characters to write
    rep stosw                  ; Clear screen by writing spaces with attributes

    ;; Print "WIN" message in the center
    mov ah, 09h                ; Function 09h - Write string
    mov al, 0                  ; Display string until '$' character
    mov bh, 00h                ; Video page number
    mov bl, 02Fh               ; White text on green background
    mov dx, win_msg_buffer     ; DX points to the message buffer
    int 10h                    ; Call BIOS interrupt
    ret

clear_screen_and_print_lose:
    ;; Clear the screen
    mov ax, 0600h              ; Function 06h - Scroll up window
    mov bh, 00h                ; Display all attributes
    mov cx, 0000h              ; Upper-left corner
    mov dx, SCREENW * SCREENH  ; Lower-right corner
    mov es, ax                 ; ES:DI <- video memory (0B800:0000 or B8000)
    mov di, 0000h              ; Upper-left corner
    mov ax, 0720h              ; Space character with white on black attribute
    mov cx, 5                  ; Total number of characters to write
    rep stosw                  ; Clear screen by writing spaces with attributes

    ;; Print "LOSE" message in the center
    mov ah, 09h                ; Function 09h - Write string
    mov al, 0                  ; Display string until '$' character
    mov bh, 00h                ; Video page number
    mov bl, 04Fh               ; White text on red background
    mov dx, lose_msg_buffer    ; DX points to the message buffer
    int 10h                    ; Call BIOS interrupt
    ret


;; Bootsector padding
times 510-($-$$) db 0
dw 0AA55h       ; Bootsector signature