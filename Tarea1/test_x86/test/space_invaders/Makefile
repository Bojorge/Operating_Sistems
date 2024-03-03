.PHONY: all

all:
	nasm -f bin invaders_ish.asm -o invaders_ish.bin
run:
	qemu-system-i386 -drive format=raw,file=invaders_ish.bin