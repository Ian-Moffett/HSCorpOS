#ifndef LFBGOP_H
#define LFBGOP_H

#define BBP 4

#include <stddef.h>
#include "../font/PSF1.h"

typedef struct {
    void* baseAddr;
    size_t bufsize;
    unsigned int width;
    unsigned int height;
    unsigned int ppsl;          // Pixels per scan line.
} framebuffer_t;

void putChar(framebuffer_t* framebuffer, psf1_font* font, unsigned int color, char chr, unsigned int xOffset, unsigned int yOffset);


#endif
