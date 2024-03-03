ASM_FILES = ./menu/game_menu ./pong/bootsector_pong ./snake_game/snake ./space_invaders/invaders_ish

all: $(ASM_FILES)
	cat game_menu.bin bootsector_pong.bin snake.bin invaders_ish.bin > games
	rm *.bin
	mv games games.bin

$(ASM_FILES):
	nasm -f bin $@.asm -o $@.bin
	mv $@.bin ./

clean:
	rm *.bin
	
run:
	qemu-system-i386 -drive format=raw,file=games.bin