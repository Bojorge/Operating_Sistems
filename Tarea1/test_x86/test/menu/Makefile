all:
	nasm -f bin game_menu.asm -o game_menu.bin

run:
	qemu-system-i386 -drive format=raw,file=game_menu.bin