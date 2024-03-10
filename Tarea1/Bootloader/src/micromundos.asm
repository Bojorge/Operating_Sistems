use16       ; use 16 bit code when assembling
org 8000h   ; Set memory offsets to start here

;; CONSTANTS -----------
VIDMEM      equ 0B800h       ; Color text mode VGA memory location
SCREENW     equ 80           ; Screen width
SCREENH     equ 25           ; Screen height
PLAYERCHAR  equ 248          ; ASCII code for player character (°)
PLAYERCOLOR equ 02h          ; Green color for the player

;; VARIABLES -----------
playerX:    dw 40            ; Player X position
playerY:    dw 12            ; Player Y position

;; LOGIC ----------------
setup_game:
    ;; Set up video mode
    mov ax, 03h         ; Set video mode to 80x25 text mode, 16 colors
    int 10h

    ;; Set up video memory
    mov ax, VIDMEM
    mov es, ax          ; ES:DI <- video memory (0B800:0000 or B8000)

    
    ;; Initialize player position
    mov ax, [playerX]
    mov bx, [playerY]
    call draw_player

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
    
    jmp game_loop       ; Wait for valid key press


move_southwest:
    ;cmp word [playerY], SCREENH   ; Check if at bottom of screen
    ;cmp word [playerX], 0  ; Check if at left edge of screen
    ;jle game_loop          ; dont move up
    
    inc word [playerY]     ; Move player down
    dec word [playerX]     ; Move player left
    call draw_player
    jmp game_loop


move_southest:
    ;cmp word [playerY], SCREENH   ; Check if at bottom of screen
    ;cmp word [playerX], SCREENW  ; Check if at right edge of screen
    ;jle game_loop          ; dont move up
    
    inc word [playerY]     ; Move player down
    inc word [playerX]     ; Move player right
    call draw_player
    jmp game_loop

move_northwest:
    ;cmp word [playerY], 0   ; Check if at top of screen
    ;cmp word [playerX], 0  ; Check if at left edge of screen
    ;jle game_loop          ; dont move up
    
    dec word [playerY]     ; Move player down
    dec word [playerX]     ; Move player right
    call draw_player
    jmp game_loop


move_northest:
    ;cmp word [playerY], 0   ; Check if at top of screen
    ;cmp word [playerX], SCREENW  ; Check if at right edge of screen
    ;jle game_loop          ; dont move up
    
    dec word [playerY]     ; Move player down
    inc word [playerX]     ; Move player right
    call draw_player
    jmp game_loop



move_up:
    cmp word [playerY], 0  ; Check if at top of screen
    jle game_loop          ; If at top, dont move up
    dec word [playerY]     ; Move player up
    call draw_player
    jmp game_loop

move_down:
    cmp word [playerY], SCREENH    ; Check if at bottom of screen
    jge game_loop                       ; If at bottom, dont move down
    inc word [playerY]                  ; Move player down
    call draw_player
    jmp game_loop

move_left:
    cmp word [playerX], 0  ; Check if at left edge of screen
    jle game_loop          ; If at left edge, dont move left
    dec word [playerX]     ; Move player left
    call draw_player
    jmp game_loop

move_right:
    cmp word [playerX], SCREENW    ; Check if at right edge of screen
    jge game_loop                       ; If at right edge, dont move right
    inc word [playerX]                  ; Move player right
    call draw_player
    jmp game_loop

draw_player:
    mov di, [playerY]       ; Calculate offset in video memory
    imul di, SCREENW
    add di, [playerX]

    mov al, PLAYERCHAR      ; Draw player character
    mov ah, PLAYERCOLOR     ; Set player color
    stosw                   ; Store character and color in video memory
    ret

;; Bootsector padding
times 510-($-$$) db 0
dw 0AA55h       ; Bootsector signature