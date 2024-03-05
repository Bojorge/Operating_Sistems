ASM_FILES = ./src/main ./src/micromundos ./src/test

all: $(ASM_FILES)
	cat main.bin micromundos.bin test.bin > micromundos
	rm *.bin
	mv micromundos micromundos.bin


$(ASM_FILES):
	nasm -f bin $@.asm -o $@.bin
	mv $@.bin ./

clean:
	rm *.bin
	
run:
	qemu-system-i386 -drive format=raw,file=micromundos.bin