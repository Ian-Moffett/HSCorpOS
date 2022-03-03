#include <efi.h>
#include <efilib.h>
#include <elf.h>
#include <stddef.h>

#define GOPGUID EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID
#define PSF1_MAGIC0 0x00000036
#define PSF1_MAGIC1 0x00000004


typedef struct {
    void* baseAddr;
    size_t bufsize;
    unsigned int width;
    unsigned int height;
    unsigned int ppsl;          // Pixels per scan line.
} framebuffer_t;


typedef struct {
    unsigned char magic[2];
    unsigned char mode;             // Font mode.
    unsigned char chsize;           // Size of characters in bytes.
} psf1_header;


typedef struct {
    psf1_header* header;
    void* glyphBuffer;
} psf1_font;


framebuffer_t framebuffer;


void initGOP() {
    EFI_GUID gopGuid = GOPGUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
    EFI_STATUS status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL, (void**)&gop);

    if (EFI_ERROR(status)) {
        Print(L"Unable to locate GOP.");
        return;
    } else {
        Print(L"GOP located!\n");
    }

    framebuffer.baseAddr = (void*)gop->Mode->FrameBufferBase;       // Grab base addr.
    framebuffer.bufsize = gop->Mode->FrameBufferSize;               // Grab size.
    framebuffer.width = gop->Mode->Info->HorizontalResolution;
    framebuffer.width = gop->Mode->Info->VerticalResolution;
    framebuffer.ppsl = gop->Mode->Info->PixelsPerScanLine;
}


EFI_FILE* loadFile(EFI_FILE* dir, CHAR16* path, EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* systable) {
    EFI_FILE* loadedFile;
    // Tell UEFI we are using the filesystem we are booting from.
    EFI_LOADED_IMAGE_PROTOCOL* loadedImage;
    systable->BootServices->HandleProtocol(imageHandle, &gEfiLoadedImageProtocolGuid, (void**)&loadedImage);
    
    // We need to get filesystem.
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* filesystem;
    systable->BootServices->HandleProtocol(loadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void**)&filesystem);

    if (dir == NULL) {
        filesystem->OpenVolume(filesystem,  &dir);
    }

    EFI_STATUS status = dir->Open(dir, &loadedFile, path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);

    if (status != EFI_SUCCESS) {
        return NULL;
    }

    return loadedFile;
}


psf1_font* load_psf1_font(EFI_FILE* dir, CHAR16* path, EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* systable) {
    EFI_FILE* font = loadFile(dir, path, imageHandle, systable);

    if (!(font)) {
        return NULL;    // File does not exist.
    }

    psf1_header* fontHeader;
    systable->BootServices->AllocatePool(EfiLoaderData, sizeof(psf1_header), (void**)&fontHeader);   // Allocate memory for header.
    UINTN size = sizeof(psf1_header);
    font->Read(font, &size, fontHeader);

    if (!(fontHeader->magic[0] & PSF1_MAGIC0) || !(fontHeader->magic[1] & PSF1_MAGIC1)) {
        Print(L"BAD_HEADER: MAGIC[0] => 0x%X, MAGIC[1] => 0x%X\n", fontHeader->magic[0], fontHeader->magic[1]);
        return NULL;   // Format bad.
    }

    UINTN glyphBuffersize = fontHeader->chsize * 256;

    if (fontHeader->mode == 1) {
        glyphBuffersize = fontHeader->chsize * 512;
    }

    void* glyphBuffer;
    font->SetPosition(font, sizeof(psf1_header));
    systable->BootServices->AllocatePool(EfiLoaderData, glyphBuffersize, (void**)&glyphBuffer);
    font->Read(font, &glyphBuffersize, glyphBuffer);

    psf1_font* fontRes;
    systable->BootServices->AllocatePool(EfiLoaderData, sizeof(psf1_font), (void**)&fontRes);

    fontRes->header = fontHeader;
    fontRes->glyphBuffer = glyphBuffer;
    return fontRes;
}


int memcmp(const void* aptr, const void* bptr, size_t n) {
	const unsigned char* a = aptr, *b = bptr;
	for (size_t i = 0; i < n; i++) {
		if (a[i] < b[i]) return -1;
		else if (a[i] > b[i]) return 1;
	}
	return 0;
}


EFI_STATUS efi_main (EFI_HANDLE imghandle, EFI_SYSTEM_TABLE* systable) {
    InitializeLib(imghandle, systable);         // Setups UEFI stuff.

    EFI_FILE* kernel = loadFile(NULL, L"kernel.elf", imghandle, systable);

    if (!(kernel)) {
        Print(L"Could not load kernel.elf\n");
    } else {
        Print(L"Kernel loaded sucessfully!\n");
    }

    Elf64_Ehdr header;
    UINTN fileinfosize;
    EFI_FILE_INFO* fileinfo;
    kernel->GetInfo(kernel, &gEfiFileInfoGuid, &fileinfosize, NULL);
    systable->BootServices->AllocatePool(EfiLoaderData, fileinfosize, (void**)&fileinfo);
    // Get file info.
    kernel->GetInfo(kernel, &gEfiFileInfoGuid, &fileinfosize, (void**)&fileinfo);
    
    UINTN headersize = sizeof(header);
    kernel->Read(kernel, &headersize, &header);

    if (memcmp(&header.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 ||
		header.e_ident[EI_CLASS] != ELFCLASS64 ||
		header.e_ident[EI_DATA] != ELFDATA2LSB ||
		header.e_type != ET_EXEC ||
		header.e_machine != EM_X86_64 || header.e_version != EV_CURRENT) {

        Print(L"Kernel header is bad.\n");
    } else {
        Print(L"Kernel header sucessfully verified!\n");     
    }

    Elf64_Phdr* progheaders;

    // Set load offset.
    kernel->SetPosition(kernel, header.e_phoff);
    UINTN phdrsize = header.e_phnum * header.e_phentsize; 
    systable->BootServices->AllocatePool(EfiLoaderData, phdrsize, (void**)&progheaders);  // Allocate memory for program headers.
    kernel->Read(kernel, &phdrsize, progheaders);       // Load info.

    for (Elf64_Phdr* phdr = progheaders; (char*)phdr < (char*)progheaders + header.e_phnum * header.e_phentsize; phdr = (Elf64_Phdr*)((char*)phdr + header.e_phentsize)) {
        switch (phdr->p_type) {
            case PT_LOAD:
                {
                    int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000;
                    Elf64_Addr segment = phdr->p_paddr;
                    systable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);
                    kernel->SetPosition(kernel, phdr->p_offset);
                    UINTN size = phdr->p_filesz;
                    kernel->Read(kernel, &size, (void*)segment);
                    break;
                }
        }
    }

    
    psf1_font* newFont = load_psf1_font(NULL, L"zap-light16.psf", imghandle, systable);

    if (!(newFont)) {
        Print(L"Font does not exist or font header bad.\n");
    } else {
        Print(L"Font found and is loaded at: 0x%X\n", newFont);
    }

    initGOP();

    Print(L"<======== Framebuffer Information ========>\n");
    Print(L"Base address: 0x%X\n", framebuffer.baseAddr);
    Print(L"Buffer size: %d\n", framebuffer.bufsize);
    Print(L"Width: %d\n", framebuffer.width);
    Print(L"Height: %d\n", framebuffer.height);
    Print(L"Pixels per scanline: %d\n", framebuffer.ppsl);

    int(*kernel_entry)(framebuffer_t*, psf1_font*) = ((__attribute((sysv_abi)) int (*)(framebuffer_t*, psf1_font*))header.e_entry);
    kernel_entry(&framebuffer, newFont);

    return EFI_SUCCESS;
}
