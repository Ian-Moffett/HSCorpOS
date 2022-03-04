#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stddef.h>


typedef struct {
    void* baseAddr;
    size_t bufferSize;
    unsigned int width;
    unsigned int height;
    unsigned int ppsl;      // Pixels per scanline.
} framebuffer_t;


#endif
