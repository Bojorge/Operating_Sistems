all:
	nasm -f bin bootsector_pong.asm -o bootsector_pong.bin
run:
	qemu-system-i386 -drive format=raw,file=bootsector_pong.bin