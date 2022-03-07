#include "../keyboard.h"

extern canvas_t defaultcanvas;
int keybuf = '\0';

static const uint8_t* const SC_ASCII = "\x00\x1B" "1234567890-=" "\x08"
"\x00" "qwertyuiop[]" "\x0D\x1D" "asdfghjkl;'`" "\x00" "\\"
"zxcvbnm,./" "\x00\x00\x00" " ";


static bool special_key(char c) {
    if (c >= 'a' && c <= 'z') {
        return false;
    }

    return true;
}


__attribute__((interrupt)) void kb_isr(int_frame_t* frame) {
    int scancode = inportb(0x60);

    
    kwrite(&defaultcanvas, "A", 0xA600CD);

    if (!(scancode & 0x80)) {           // If just key press.
        char c = SC_ASCII[scancode];

        if (!(special_key(c))) {
            c -= 0x20;
        }

        keybuf = c;
    }

    pic_sendEOI(1);
}
