#ifndef MEMORY_TYPES_H
#define MEMORY_TYPES_H

#include <stdint.h>


typedef struct {
    uint32_t type;
    void* physAddr;
    void* virtAddr;
    uint64_t numPages;
    uint64_t attr;
} mem_desc_t;


typedef struct {
    mem_desc_t* mMap;
    uint64_t mMapSize;
    uint64_t mMapDescSize;
} mem_info_t;


extern const char* const EFI_MEMORY_TYPES[];


#endif
