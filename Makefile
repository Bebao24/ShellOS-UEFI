BUILD_DIR = bin
GNU_EFI = gnu-efi
KERNEL = kernel
OVMFDIR = OVMFbin
FONT = fonts

.PHONY: all image kernel bootloader clean run

all: image

image: $(BUILD_DIR)/image.iso

$(BUILD_DIR)/image.iso: kernel bootloader
	@ echo "Creating disk image"
	@ dd if=/dev/zero of=$@ bs=512 count=2880
	@ mformat -i $@ -f 1440 ::
	@ mmd -i $@ ::/EFI
	@ mmd -i $@ ::/EFI/BOOT
	@ echo "Copying files"
	@ mcopy -i $@ $(GNU_EFI)/x86_64/bootloader/main.efi ::/EFI/BOOT
	@ mcopy -i $@ startup.nsh ::
	@ mcopy -i $@ $(KERNEL)/bin/kernel.bin ::
	@ mcopy -i $@ $(FONT)/default.psf ::
	@ echo "Created " $@

kernel: $(KERNEL)/bin/kernel.bin
$(KERNEL)/bin/kernel.bin: always
	@ $(MAKE) -C $(KERNEL) all

bootloader: $(GNU_EFI)/x86_64/bootloader/main.efi
$(GNU_EFI)/x86_64/bootloader/main.efi: always
	@ $(MAKE) -C $(GNU_EFI)
	@ $(MAKE) -C $(GNU_EFI) bootloader

always:
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)/*
	$(MAKE) -C kernel clean

run:
	qemu-system-x86_64 -drive file=$(BUILD_DIR)/image.iso \
		-m 256M -cpu qemu64 \
		-drive if=pflash,format=raw,unit=0,file="$(OVMFDIR)/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="$(OVMFDIR)/OVMF_VARS-pure-efi.fd" -net none 

