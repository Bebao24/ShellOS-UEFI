#include <efi.h>
#include <efilib.h>
#include <elf.h>

#include <stddef.h>
#include <stdint.h>

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

    int (*KernelStart)() = ((__attribute__((sysv_abi)) int (*)()) header.e_entry);

    // Print the kernel return code
    Print(L"Kernel exited with code: %d\r\n", KernelStart());

    return EFI_SUCCESS;
}
