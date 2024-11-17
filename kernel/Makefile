CC = x86_64-elf-gcc
LD = x86_64-elf-gcc

ASM = nasm

LDS = kernel.ld

CFLAGS = -ffreestanding -fshort-wchar -mno-red-zone -I src -I src/graphics -I src/memory -I src/libc++ -I src/arch/x86_64 \
	-I src/arch/x86_64/cpu
LDFLAGS = -T $(LDS) -static -Bsymbolic -nostdlib -lgcc -fno-exceptions

ASMFLAGS = -f elf64

SRC_DIR = src
OBJECT_DIR = objs
BUILD_DIR = bin

C_SOURCES := $(shell find src -name "*.cpp" -type f)
C_OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJECT_DIR)/%.o, $(C_SOURCES))

C_HEADERS := $(shell find src -name "*.h" -type f)

ASM_SOURCES := $(shell find src -name "*.asm" -type f)
ASM_OBJECTS := $(patsubst $(SRC_DIR)/%.asm, $(OBJECT_DIR)/%_asm.o, $(ASM_SOURCES))

ASM_HEADERS := $(shell find src -name "*.inc" -type f)

.PHONY: all kernel clean

all: kernel

kernel: $(BUILD_DIR)/kernel.bin

$(BUILD_DIR)/kernel.bin: $(C_OBJECTS) $(ASM_OBJECTS)
	@ mkdir -p $(BUILD_DIR)
	$(LD) $(LDFLAGS) -o $@ $^ -lgcc

$(OBJECT_DIR)/arch/x86_64/cpu/interrupts.o: $(SRC_DIR)/arch/x86_64/cpu/interrupts.cpp $(SRC_DIR)/arch/x86_64/cpu/interrupts.h
	@ echo "Compiling " $<
	@ mkdir -p $(dir $@)
	@ $(CC) -mno-red-zone -mgeneral-regs-only -ffreestanding -c $< -o $@ -I src -I src/graphics -I src/memory \
	 -I src/libc++ -I src/arch/x86_64 \
	-I src/arch/x86_64/cpu

$(OBJECT_DIR)/%.o: $(SRC_DIR)/%.cpp $(C_HEADERS)
	@ echo "Compiling" $<
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) -c -o $@ $<

$(OBJECT_DIR)/%_asm.o: $(SRC_DIR)/%.asm $(ASM_HEADERS)
	@ echo "Assembly " $<
	@ mkdir -p $(dir $@)
	@ $(ASM) $(ASMFLAGS) -o $@ $<
	

clean:
	rm -rf $(OBJECT_DIR)/*
	rm -rf $(BUILD_DIR)/*



