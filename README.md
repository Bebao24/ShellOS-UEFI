# ShellOS

A simple operating system

## NOTE

This OS is only bootable on UEFI machine, which mean that it is not bootable on BIOS machine.

## Requirements
To be able to build the OS, you will need:
- NASM (Assembler)
- GCC Cross Compiler (x86_64-elf)
- GNU MAKE (Build)
- QEMU (Machine emulator)

## Environment
You need to build this on linux, any distro
If you want to build this on windows, I recommended to install [WSL](https://learn.microsoft.com/en-us/windows/wsl/about)

## Build and run
To build:
``` sh
make
```
To run
``` sh
make run # make sure that you have qemu installed
```

## Real hardware
You can use the dd command to copy from the ISO to the USB drive and then use it to boot on your computer


