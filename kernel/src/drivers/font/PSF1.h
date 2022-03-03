#ifndef PSF1_H
#define PSF1_H


typedef struct {
    unsigned char magic[2];
    unsigned char mode;             // Font mode.
    unsigned char chsize;           // Size of characters in bytes.
} psf1_header;


typedef struct {
    psf1_header* header;
    void* glyphBuffer;
} psf1_font;


#endif
