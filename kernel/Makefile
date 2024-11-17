CC = x86_64-elf-gcc
LD = x86_64-elf-gcc

LDS = kernel.ld

CFLAGS = -ffreestanding -fshort-wchar -mno-red-zone -I src -I src/graphics -I src/memory
LDFLAGS = -T $(LDS) -static -Bsymbolic -nostdlib -lgcc -fno-exceptions

SRC_DIR = src
OBJECT_DIR = objs
BUILD_DIR = bin

C_SOURCES := $(shell find src -name "*.cpp" -type f)
C_OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJECT_DIR)/%.o, $(C_SOURCES))

C_HEADERS := $(shell find src -name "*.h" -type f)

.PHONY: all kernel clean

all: kernel

kernel: $(BUILD_DIR)/kernel.bin

$(BUILD_DIR)/kernel.bin: $(C_OBJECTS)
	@ mkdir -p $(BUILD_DIR)
	$(LD) $(LDFLAGS) -o $@ $^

$(OBJECT_DIR)/%.o: $(SRC_DIR)/%.cpp $(C_HEADERS)
	@ echo "Compiling" $^
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILD_DIR)/*



