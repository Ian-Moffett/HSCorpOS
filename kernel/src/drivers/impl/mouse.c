#include "../mouse.h"

__attribute__((interrupt)) void mouse_isr(int_frame_t*) {
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
