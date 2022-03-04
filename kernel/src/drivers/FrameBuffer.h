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


typedef struct {
    unsigned char magic[2];
    unsigned char mode;
    unsigned char chsize;
} psf1_header_t;


typedef struct {
    psf1_header_t* header;
    void* glyphBuffer;
} psf1_font_t;


void putChar(framebuffer_t* framebuffer, psf1_font_t* psf1_font, unsigned int color, char chr, unsigned int xOff, unsigned int yOff);

#endif
