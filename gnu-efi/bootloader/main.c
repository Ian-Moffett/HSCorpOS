#include <efi.h>
#include <efilib.h>
#include <elf.h>
#include <stddef.h>


int memcmp(const void* aptr, const void* bptr, size_t n) {
	const unsigned char* a = aptr, *b = bptr;
	for (size_t i = 0; i < n; i++) {
		if (a[i] < b[i]) return -1;
		else if (a[i] > b[i]) return 1;
	}
	return 0;
}


EFI_FILE* loadFile(EFI_FILE* directory, CHAR16* path, EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* sysTable) {
    EFI_FILE* fileres;
    EFI_LOADED_IMAGE_PROTOCOL* loadedImage;
    sysTable->BootServices->HandleProtocol(imageHandle, &gEfiLoadedImageProtocolGuid, (void**)&loadedImage);

    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* filesystem;
    sysTable->BootServices->HandleProtocol(loadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void**)&filesystem);
    
    if (!(directory)) {
        filesystem->OpenVolume(filesystem, &directory);
    }

    EFI_STATUS status = directory->Open(directory, &fileres, path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);

    if (status != EFI_SUCCESS) {
        return NULL;
    } 

    return fileres;
}

EFI_STATUS efi_main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* sysTable) {
    InitializeLib(imageHandle, sysTable);
    EFI_FILE* kernel = loadFile(NULL, L"kernel.elf", imageHandle, sysTable);
    Elf64_Ehdr kernelHeader;

    if (!(kernel)) {
        Print(L"Kernel ELF file does not exist!\n");
    } else {
        UINTN fileInfoSize;
        kernel->GetInfo(kernel, &gEfiFileInfoGuid, &fileInfoSize, NULL);
        kernel->Read(kernel, &fileInfoSize, &kernelHeader);
        
        if (memcmp(&kernelHeader.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 || 
                kernelHeader.e_ident[EI_CLASS] != ELFCLASS64 || 
                kernelHeader.e_type != ET_EXEC || 
                kernelHeader.e_machine != EM_X86_64 || kernelHeader.e_version != EV_CURRENT) {
            
            Print(L"ELF exists but header is bad.\n");
        } else {
            Elf64_Phdr* programHeaders;
            kernel->SetPosition(kernel, kernelHeader.e_phoff);
            UINTN progHeaderSize = kernelHeader.e_phnum * kernelHeader.e_phentsize;
            sysTable->BootServices->AllocatePool(EfiLoaderData, progHeaderSize, (void**)&programHeaders);
            kernel->Read(kernel, &progHeaderSize, programHeaders);

            for (Elf64_Phdr* progHeader = programHeaders; (char*)progHeader < (char*)programHeaders + kernelHeader.e_phnum * kernelHeader.e_phentsize; progHeader = (Elf64_Phdr*)((char*)progHeader + kernelHeader.e_phentsize)) {
                switch (progHeader->p_type) {
                    case PT_LOAD:
                        {
                            unsigned int pages = (progHeader->p_memsz + 0x1000 - 1) / 0x1000;
                            Elf64_Addr segment = progHeader->p_paddr;
                            sysTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);
                            kernel->SetPosition(kernel, progHeader->p_offset);
                            UINTN size = progHeader->p_filesz;
                            kernel->Read(kernel, &size, (void*)segment);
                            break;
                        }
                }
            }

            int(*kernel_entry)() = ((__attribute__((sysv_abi))int(*)())kernelHeader.e_entry);
            Print(L"Kernel returned: %d\n", kernel_entry());
        }

    }

    return EFI_SUCCESS;
}
