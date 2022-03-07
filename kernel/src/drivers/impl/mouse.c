#include "../mouse.h"

uint8_t MousePointer[] = {
    0b11111111, 0b11100000, 
    0b11111111, 0b10000000, 
    0b11111110, 0b00000000, 
    0b11111100, 0b00000000, 
    0b11111000, 0b00000000, 
    0b11110000, 0b00000000, 
    0b11100000, 0b00000000, 
    0b11000000, 0b00000000, 
    0b11000000, 0b00000000, 
    0b10000000, 0b00000000, 
    0b10000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
    0b00000000, 0b00000000, 
};


extern canvas_t defaultcanvas;

static uint8_t cycle = 0;
static uint8_t packet[4];
bool packetReady = false;
static void handle_mouse(uint8_t byte) {
    switch (cycle) {
        case 0:
            if (packetReady) {
                break;
            } 

            if (byte & 0b00001000 == 0) {
                break;
            }

            packet[0] = byte;
            ++cycle;
            break;
        case 1: 
            if (packetReady) {
                break;
            }

            packet[1] = byte;
            ++cycle;
            break;
        case  2:       
            if (packetReady) {
                break;
            }

            packet[2] = byte;
            packetReady = true;
            cycle = 0;
            break;
    }
}

point_t mousepos = {
    .x = 50,
    .y = 50
};

static void eval_mouse_packet(canvas_t* canvas) {
    if (!(packetReady)) {
        return;
    }

    bool xNeg, yNeg, xOverflow, yOverflow;

    if (packet[0] & PS2XSign) {
        xNeg = true;
    } else {
        xNeg = false;
    }

    if (packet[0] & PS2YSign) {
        yNeg = true;
    } else {
        yNeg = false;
    }

    if (packet[0] & PS2XOverflow) {
        xOverflow = true;
    } else {
        xOverflow = false;
    }

    if (packet[0] & PS2YOverflow) {
        yOverflow = true;
    } else {
        yOverflow = false;
    }

    if (!(xNeg)) {
        mousepos.x += packet[1];

        if (xOverflow) {
            mousepos.x += 255;
        } 

    } else {
        packet[1] = 256 - packet[1];
        mousepos.x -= packet[1];
        if (xOverflow) {
            mousepos.x -= 255;
        }
    }


    if (!(yNeg)) {
        mousepos.y -= packet[2];

        if (yOverflow) {
            mousepos.y -= 255;
        }
    } else {
        packet[2] = 256 - packet[2];
        mousepos.y += packet[2];
        if (yOverflow) {
            mousepos.y += 255;
        }
    } 
    
    if (mousepos.x < 0) {
        mousepos.x = 0;
    }

    if (mousepos.x > canvas->lfb->width - 8) {
        mousepos.x = canvas->lfb->width - 8;
    }

    if (mousepos.y < 0) {
        mousepos.y = 0;
    }

    if (mousepos.y > canvas->lfb->height - 16) {
        mousepos.y = canvas->lfb->height -16;
    }

    packetReady = false;
    putChar(canvas->lfb, canvas->font, 0xFFFFFFFF, 'O', mousepos.x, mousepos.y);
}


__attribute__((interrupt)) void mouse_isr(int_frame_t*) {
    handle_mouse(inportb(0x60));
    eval_mouse_packet(&defaultcanvas);
    inportb(0x60);      // ACK.
    pic_sendEOI(12);
}


static void mouse_wait() {
    uint64_t timeout = 100000;

    while (timeout--) {
        if (!(inportb(0x64) & 0x1)) {
            return;
        }
    }
}


static void mouse_wait_inp() {
    uint64_t timeout = 100000;

    while (timeout--) {
        if (inportb(0x64) & 0b1) {
            return;
        }
    }
}


static uint8_t mouse_read() {
    mouse_wait_inp();
    return inportb(0x60);
}


static void mouse_write(uint8_t byte) {
    mouse_wait();
    outportb(0x64, 0xD4);       // Command byte.
    mouse_wait();
    outportb(0x60, byte);
}


void mouse_init() {
    outportb(0x64, 0xA8);       // Enable auxiliary device.
    mouse_wait();
    outportb(0x64, 0x20);
    mouse_wait_inp();
    uint8_t s = inportb(0x60);
    s |= 0b10;
    mouse_wait();
    outportb(0x64, 0x60);
    mouse_wait();
    outportb(0x60, s);
    
    mouse_write(0xF6);      // Default settings.
    mouse_read();           // ACK.
    mouse_write(0xF4);      // Enable mouse.
    mouse_read();           // ACK.
}
