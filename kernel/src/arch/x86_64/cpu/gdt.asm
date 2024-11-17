[bits 64]

global LoadGDT
LoadGDT:
    lgdt [rdi] ; Load the GDT, the GDT should be in RDI

    ; Load data segments
    mov ax, 0x10
    mov es, ax
    mov ds, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Perform a far return with the code segment
    pop rdi
    mov rax, 0x08
    push rax
    push rdi
    retfq ; return far in x86_64
