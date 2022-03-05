#include <efi.h>
#include <efilib.h>
#include <elf.h>
#include <stddef.h>

#define PSF1_MAGIC0 0x00000036
#define PSF1_MAGIC1 0x00000036

typedef struct {
    void* baseAddr;
    size_t bufferSize;
    unsigned int width;
    unsigned int height;
    unsigned int ppsl;      // Pixels per scanline.
} framebuffer_t;


typedef struct {
    unsigned char magic[2];
    unsigned char mode;
    unsigned char chsize;
} psf1_header_t;


typedef struct {
    psf1_header_t* header;
    void* glyphBuffer;
} psf1_font_t;


typedef struct {
    EFI_MEMORY_DESCRIPTOR* mMap;
    UINTN mMapSize;
    UINTN mMapDescriptorSize;
} meminfo_t;


framebuffer_t* initGOP(EFI_SYSTEM_TABLE* sysTable) {
    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;

    EFI_STATUS s = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL, (void**)&gop);

    if (EFI_ERROR(s)) {
        return NULL;
    }

    framebuffer_t* linearFrameBuffer;
    sysTable->BootServices->AllocatePool(EfiLoaderData, sizeof(framebuffer_t), (void**)&linearFrameBuffer);
    linearFrameBuffer->baseAddr = (void*)gop->Mode->FrameBufferBase;
    linearFrameBuffer->bufferSize = gop->Mode->FrameBufferSize;
    linearFrameBuffer->width = gop->Mode->Info->HorizontalResolution;
    linearFrameBuffer->height = gop->Mode->Info->VerticalResolution;
    linearFrameBuffer->ppsl = gop->Mode->Info->PixelsPerScanLine;
    return linearFrameBuffer;
}


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

psf1_font_t* load_psf1_font(EFI_FILE* dir, CHAR16* path, EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* systable) {
    EFI_FILE* font = loadFile(dir, path, imageHandle, systable);

    if (!(font)) {
        return NULL;    // File does not exist.
    }

    psf1_header_t* fontHeader;
    systable->BootServices->AllocatePool(EfiLoaderData, sizeof(psf1_header_t), (void**)&fontHeader);   // Allocate memory for header.
    UINTN size = sizeof(psf1_header_t);
    font->Read(font, &size, fontHeader);

    if (!(fontHeader->magic[0] & PSF1_MAGIC0) || !(fontHeader->magic[1] & PSF1_MAGIC1)) {
        return NULL;   // Format bad.
    }

    UINTN glyphBuffersize = fontHeader->chsize * 256;

    if (fontHeader->mode == 1) {
        glyphBuffersize = fontHeader->chsize * 512;
    }

    void* glyphBuffer;
    font->SetPosition(font, sizeof(psf1_header_t));
    systable->BootServices->AllocatePool(EfiLoaderData, glyphBuffersize, (void**)&glyphBuffer);
    font->Read(font, &glyphBuffersize, glyphBuffer);

    psf1_font_t* fontRes;
    systable->BootServices->AllocatePool(EfiLoaderData, sizeof(psf1_font_t), (void**)&fontRes);

    fontRes->header = fontHeader;
    fontRes->glyphBuffer = glyphBuffer;
    return fontRes;
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

            framebuffer_t* lfb = initGOP(sysTable);
            #ifdef HSCORPOS_DUMP_LFB_INFO
            Print(L"LFB_BASE_ADDR => 0x%X\nLFB_WIDTH => %d\nLFB_HEIGHT: => %d\n", (UINTN)lfb->baseAddr, lfb->width, lfb->height);
            #endif 

            void(*kernel_entry)(framebuffer_t*, psf1_font_t*, meminfo_t meminfo) = ((__attribute__((sysv_abi))void(*)(framebuffer_t*, psf1_font_t*, meminfo_t meminfo))kernelHeader.e_entry);
            psf1_font_t* defaultFont = load_psf1_font(NULL, L"zap-light16.psf", imageHandle, sysTable);

            EFI_MEMORY_DESCRIPTOR* mMap = NULL;
            UINTN mMapSize, mMapKey, mMapDescSize;
            UINT32 descriptorVersion;

            sysTable->BootServices->GetMemoryMap(&mMapSize, mMap, &mMapKey, &mMapDescSize, &descriptorVersion);
            sysTable->BootServices->AllocatePool(EfiLoaderData, mMapSize, (void**)&mMap);
            sysTable->BootServices->GetMemoryMap(&mMapSize, mMap, &mMapKey, &mMapDescSize, &descriptorVersion);

            meminfo_t meminfo = {
                .mMap = mMap,
                .mMapSize = mMapSize,
                .mMapDescriptorSize = mMapDescSize
            };


            if (defaultFont != NULL) {
                kernel_entry(lfb, defaultFont, meminfo);
            } else {
                Print(L"FONT_LOAD_FAILURE.\n");
            }
        }

    }

    return EFI_SUCCESS;
}
