#include <efi.h>
#include <efilib.h>
#include <elf.h>

#include <stddef.h>
#include <stdint.h>

typedef struct
{
    void* BaseAddress;
    size_t BufferSize;
    uint32_t width;
    uint32_t height;
    uint32_t PixelsPerScanLine;
} GOP_Framebuffer_t;

#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04

typedef struct
{
    uint8_t magic[2];
    uint8_t mode;
    uint8_t charSize;
} PSF1_HEADER;

typedef struct
{
    PSF1_HEADER* fontHeader;
    void* glyphBuffer;
} PSF1_FONT;

GOP_Framebuffer_t framebuffer;
GOP_Framebuffer_t* InitializeGOP()
{
    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
    EFI_STATUS status;

    status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL, (void**)&gop);
    if (EFI_ERROR(status))
    {
        Print(L"Failed to locate GOP!\r\n");
        return NULL;
    }
    else
    {
        Print(L"GOP located!\r\n");
    }

    framebuffer.BaseAddress = (void*)gop->Mode->FrameBufferBase;
    framebuffer.BufferSize = gop->Mode->FrameBufferSize;
    framebuffer.width = gop->Mode->Info->HorizontalResolution;
    framebuffer.height = gop->Mode->Info->VerticalResolution;
    framebuffer.PixelsPerScanLine = gop->Mode->Info->PixelsPerScanLine;
    return &framebuffer;
}

EFI_FILE* LoadFile(EFI_FILE* Directory, CHAR16* Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
    EFI_STATUS status;
    EFI_FILE* LoadedFile;

    EFI_LOADED_IMAGE_PROTOCOL* LoadedImage;
    SystemTable->BootServices->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, (void**)&LoadedImage);

    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FileSystem;
    SystemTable->BootServices->HandleProtocol(LoadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void**)&FileSystem);

    if (Directory == NULL)
    {
        // Open the root directory
        FileSystem->OpenVolume(FileSystem, &Directory);
    }

    status = Directory->Open(Directory, &LoadedFile, Path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
    if (status != EFI_SUCCESS)
    { 
        return NULL; // file not found
    }

    return LoadedFile;
}

PSF1_FONT* LoadFont(EFI_FILE* Directory, CHAR16* Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
    EFI_FILE* fontFile = LoadFile(Directory, Path, ImageHandle, SystemTable);
    
    if (fontFile == NULL) return NULL; // Font file not found
    
    PSF1_HEADER* fontHeader;
    SystemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF1_HEADER), (void**)&fontHeader);
    UINTN size = sizeof(PSF1_HEADER);
    fontFile->Read(fontFile, &size, fontHeader);

    if (fontHeader->magic[0] != PSF1_MAGIC0 || fontHeader->magic[1] != PSF1_MAGIC1)
    {
        return NULL; // Font not valid
    }

    UINTN glyphBufferSize = fontHeader->charSize * 256;
    if (fontHeader->mode == 1)
    {
        glyphBufferSize = fontHeader->charSize * 512;
    }

    void* glyphBuffer;
    {
        fontFile->SetPosition(fontFile, sizeof(PSF1_HEADER)); // Set the position to the glyph buffer
        SystemTable->BootServices->AllocatePool(EfiLoaderData, glyphBufferSize, (void**)&glyphBuffer);
        fontFile->Read(fontFile, &glyphBufferSize, glyphBuffer);
    }

    PSF1_FONT* finishedFont;
    SystemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF1_FONT), (void**)&finishedFont);
    finishedFont->fontHeader = fontHeader;
    finishedFont->glyphBuffer = glyphBuffer;
    return finishedFont;
}

int memcmp(const void* ptr1, const void* ptr2, size_t size)
{
    const uint8_t* u8Ptr1 = (const uint8_t*)ptr1;
    const uint8_t* u8Ptr2 = (const uint8_t*)ptr2;

    for (size_t i = 0; i < size; i++)
    {
        if (u8Ptr1[i] != u8Ptr2[i])
        {
            return 1;
        }
    }

    return 0;
}

typedef struct
{
    GOP_Framebuffer_t* framebuffer;
    PSF1_FONT* font;
} BootInfo;

EFI_STATUS efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    InitializeLib(ImageHandle, SystemTable);

    Print(L"Booting...\r\n");

    EFI_FILE* kernel = LoadFile(NULL, L"kernel.bin", ImageHandle, SystemTable);
    if (kernel == NULL)
    {
        Print(L"kernel.bin not found!\r\n");
        return EFI_NOT_FOUND;
    }
    else
    { 
        Print(L"kernel.bin found!\r\n");
    }

    Elf64_Ehdr header;
    {
        UINTN FileInfoSize;
        EFI_FILE_INFO* FileInfo;
        kernel->GetInfo(kernel, &gEfiFileInfoGuid, &FileInfoSize, NULL);
        SystemTable->BootServices->AllocatePool(EfiLoaderData, FileInfoSize, (void**)&FileInfo);
        kernel->GetInfo(kernel, &gEfiFileInfoGuid, &FileInfoSize, (void**)&FileInfo);
    
        UINTN size = sizeof(header);
        kernel->Read(kernel, &size, &header);
    }

    // Check if the kernel binary is really a efl binary
    if (
		memcmp(&header.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 ||
		header.e_ident[EI_CLASS] != ELFCLASS64 ||
		header.e_ident[EI_DATA] != ELFDATA2LSB ||
		header.e_type != ET_EXEC ||
		header.e_machine != EM_X86_64 ||
		header.e_version != EV_CURRENT
	)
    {
        Print(L"kernel.bin is not elf binary!\r\n");
        return EFI_INVALID_PARAMETER;
    }
    else
    {
        Print(L"kernel.bin header verified!\r\n");
    }

    Elf64_Phdr* phdrs;
    {
        kernel->SetPosition(kernel, header.e_phoff);
        UINTN size = header.e_phnum * header.e_phentsize;
        SystemTable->BootServices->AllocatePool(EfiLoaderData, size, (void**)&phdrs);
        kernel->Read(kernel, &size, phdrs);
    }

    for (
		Elf64_Phdr* phdr = phdrs;
		(char*)phdr < (char*)phdrs + header.e_phnum * header.e_phentsize;
		phdr = (Elf64_Phdr*)((char*)phdr + header.e_phentsize)
	)
    {
        switch (phdr->p_type)
        {
            case PT_LOAD:
                int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000;
                Elf64_Addr segment = phdr->p_paddr;
                SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);

                kernel->SetPosition(kernel, phdr->p_offset);
                UINTN size = phdr->p_filesz;
                kernel->Read(kernel, &size, (void*)segment);
                break;
        }
    }

    Print(L"Kernel loaded successfully!\r\n");

    GOP_Framebuffer_t* newBuffer = InitializeGOP();

    // Print the GOP framebuffer info
    Print(L"Framebuffer Info\r\n");

    Print(L"Base: 0x%x\n\rSize: 0x%x\n\rWidth: %d\n\rHeight: %d\n\rPixelsPerScanline: %d\n\r", 
	newBuffer->BaseAddress, 
	newBuffer->BufferSize, 
	newBuffer->width, 
	newBuffer->height, 
	newBuffer->PixelsPerScanLine);
    
    // Load the PSF font
    PSF1_FONT* font = LoadFont(NULL, L"default.psf", ImageHandle, SystemTable);
    if (font == NULL)
    {
        Print(L"Font not found or not valid!\r\n");
        return EFI_NOT_FOUND;
    }
    else
    {
        Print(L"Font found! charSize=%d\r\n", font->fontHeader->charSize);
    }

    void (*KernelStart)(BootInfo*) = ((__attribute__((sysv_abi)) void (*)(BootInfo*)) header.e_entry);

    // Passing boot infomations to the kernel
    BootInfo bootInfo;
    bootInfo.framebuffer = newBuffer;
    bootInfo.font = font;

    KernelStart(&bootInfo);

    return EFI_SUCCESS;
}
