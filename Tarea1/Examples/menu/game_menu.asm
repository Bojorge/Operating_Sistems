;;;
;;; game_menu.asm: Screen to choose and load selected bootsector game
;;;   
use16           ; Generate 16bit realmode assembly code
org 7C00h       ; Start bootsector in memory here for x86

jmp startup     ; Skip over variables/constants

;; Variables
choose_game_str: db 'Select a game with the W/S keys and press Enter'
.len equ ($-choose_game_str)
pong_str: db '1. pong'
.len equ ($-pong_str)
snake_str: db '2. snake'
.len equ ($-snake_str)
space_invaders_str: db '3. space invaders'
.len equ ($-space_invaders_str)

drive_number: db 0

;; Constants
FIRST_LINE equ 320  ; Start of 1st selectable line in bytes
LAST_LINE  equ 640  ; Start of last selectable line in bytes

startup:
;; Zero out Extra & Data segments to boot better on hardware
xor ax, ax
mov es, ax
mov ds, ax

;; Save drive number
mov [drive_number], dl

;; Set video mode
mov al, 3           ; AH = 0, AL = 3 (text mode 80x25 16 colors)
int 10h

;; Hide cursor
mov ah, 1       ; Set text-mode cursor shape
mov cx, 2607h   ; CH bit 5 = invisible (2) & scan line start (6), CL = scan line end (7)
int 10h

;; Set up video memory
mov ax, 0B800h  
mov es, ax      ; ES = B8000 color text mode video memory
xor di, di      ; ES:DI = pointer to video memory

;; Clear screen to blue
mov ax, 1020h       ; AH = BG color (1 = blue) FG color (0 = black), AL = ascii space
mov cx, 80*25       ; # of Characters on screen
rep stosw           ; Write all characters to video memory
xor di, di          ; Reset di

;; Print strings
mov si, choose_game_str
mov cx, choose_game_str.len
call write_string

sub di, choose_game_str.len*2   ; "CR"
add di, 160                     ; "LF"
add di, 160                     ; "LF"

mov si, pong_str
mov cx, pong_str.len
call write_string

sub di, pong_str.len*2          ; "CR"
add di, 160                     ; "LF"

mov si, snake_str
mov cx, snake_str.len
call write_string

sub di, snake_str.len*2         ; "CR"
add di, 160                     ; "LF"

mov si, space_invaders_str
mov cx, space_invaders_str.len
call write_string

sub di, space_invaders_str.len*2    ; "CR"

; Reset "cursor" position
sub di, 320                         ; Move up 2 lines

;; Get user input
input_loop:
    ;; "Highlight" current line
    mov bl, 31h                 ; BG color = 3 (cyan) FG color = 1 (blue)
    call change_line_colors

    ;; Get key
    xor ax, ax
    int 16h                     ; AH = 0 BIOS get keystroke, AH = scancode AL = ascii character

    cmp al, 'w'
    jne check_s
    ;; Move up 1 line
    mov bl, 17h                 ; BG color = 1 (Blue) FG color = 7 (light gray)
    call change_line_colors
    sub di, 160                 ; Go to start of previous line
    cmp di, FIRST_LINE          ; Went past line 1?
    jge input_loop              ; No, go on
    add di, 160                 ; Yes, move back down first
    jmp input_loop

    check_s:
    cmp al, 's'
    jne check_enter
    ;; Move down 1 line
    mov bl, 17h                 ; BG color = 1 (Blue) FG color = 7 (light gray)
    call change_line_colors
    add di, 160                 ; Go to start of next line
    cmp di, LAST_LINE           ; Went past last line?
    jle input_loop              ; No, go on
    sub di, 160                 ; Yes, move back up first
    jmp input_loop

    check_enter:
    cmp al, 0Dh                 ; Enter key / carriage return
    jne input_loop

    ;; Load and execute chosen game
    ; Reset disk to be safe
    xor ax, ax              ; AH = 0
    mov dl, [drive_number]  ; DL = drive number
    retry_disk_reset:
        int 13h
        jc retry_disk_reset

    ; DI / 160 = sector on disk where game is stored
    xor ax, ax
    mov es, ax      ; ES = 0
    cwd             ; Convert word to dbl word, sign extend AX to DX (DX = 0 here)
    mov ax, di
    mov bx, 160
    div bx          ; DX:AX / BX, AX = quotient, DX = remainder

    ; Read disk sectors to memory - BIOS int 13h ah 02h
    mov ch, 0       ; Cylinder low
    mov cl, al      ; bits 0-5 = Sector to start reading at, bits 6-7 = Cylinder high
    mov ax, 0201h   ; AH = 2, AL = # of sectors to read
    xor dx, dx      ; DH = head number, DL = drive number
    mov dl, [drive_number]
    mov bx, 8000h   ; ES:BX = buffer to load sectors to (using 0000:8000h here)

    retry_load:
        int 13h
        cmp al, 1       ; AL should = # of sectors read
        jne retry_load  
        jc retry_load   ; Otherwise carry flag set = error

    jmp 0000h:8000h     ; Far jump to loaded sector to execute code, does not return!

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
    ret             ; return to caller

;; Subroutine to change a line's colors
;;  Inputs:
;;    BL = colors to change to (bits 0-3 = FG color, 4-7 = BG color)
change_line_colors:
    mov cx, 80              ; Length of 1 line in characters
    highlight_loop:
        mov ax, [es:di]     ; Get character and attributes
        mov ah, bl          
        stosw               ; Write new character to position
    loop highlight_loop
    sub di, 160             ; Reset to start of line
    ret

;; Bootsector padding
times 510-($-$$) db 0   
dw 0AA55h   ; Bootsector signature