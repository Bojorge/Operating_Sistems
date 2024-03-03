ASM=nasm

SRC_DIR=src
BULD_DIR=build

$(BULD_DIR)/main.img: $(BULD_DIR)/main.bin
	cp $(BULD_DIR)/main.bin $(BULD_DIR)/main.img
	truncate -s 1440k $(BULD_DIR)/main.img 


$(BULD_DIR)/main.bin: $(SRC_DIR)/main.asm
	$(ASM) $(SRC_DIR)/main.asm -f bin -o $(BULD_DIR)/main.bin
	