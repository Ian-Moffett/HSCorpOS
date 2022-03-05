#ifndef MEM_INFO_H
#define MEM_INFO_H

#include <stdint.h>


typedef struct {
    uint32_t type;
    void* physicalAddress;
    void* virtualAddress;
    uint64_t nPages;
    uint64_t attr;
} memdesc_t;


typedef struct {
    memdesc_t* mMap;
    uint64_t mMapSize;
    uint64_t mMapDescSize;
} memory_info_t;

extern const char* const MSEGMENT_TYPES[];


#endif
