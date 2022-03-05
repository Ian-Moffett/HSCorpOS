#include "../string.h"

void __stack_chk_fail() {}


char res[25];

size_t strlen(const char* const STR) {
    size_t size = 0;
    while (STR[size++]);
    return size - 1;
}


char* dec2str(int dec) {
    unsigned char isNeg = 0;

    if (dec < 0) {
        isNeg = 1;
        dec = dec * -1;
    }

    for (int i = 0; i < 25; ++i) {
        res[i] = '\0';
    }

    char s[25];
    unsigned char ridx = 0;
    unsigned char residx = 0;

    while (dec) {
        s[ridx] = '0' + dec % 10;
        dec /= 10;
        ++ridx;
    }

    if (isNeg) {
        res[0] = '-';
        ++residx;
    }

    for (int i = strlen(s) - 1; i > -1; --i) {
        res[residx] = s[i];
        ++residx;
    }

    res[residx] = '\0';

    unsigned char rl = 1;

    for (int i = 0; i < strlen(res) && rl; ++i) {
        if (res[i] < '0' && res[i] > '9') {
            rl = 0;
            const char* const ERR = "UNKNOWN";

            for (int j = 0; j < strlen(res); ++j) {
                res[j] = '\0';
            }

            for (int j = 0; j < strlen(ERR); ++j) {
                res[j] = ERR[j];
            }
        }
    }

    return res;
}



uint8_t* hex2str(uint32_t hex_num) {
    static uint8_t hex_string[80];
    uint8_t *ascii_numbers = "0123456789ABCDEF";
    uint8_t nibble;
    uint8_t i = 0, j, temp;
    uint8_t pad = 0;
    
    // If passed in 0, print a 0
    if (hex_num == 0) {
        strncpy(hex_string, "0\0", 2);
        i = 1;
    }

    if (hex_num < 0x10) pad = 1;    // If one digit, will pad out to 2 later

    while (hex_num > 0) {
        // Convert hex values to ascii string
        nibble = (uint8_t)hex_num & 0x0F;  // Get lowest 4 bits
        nibble = ascii_numbers[nibble];    // Hex to ascii 
        hex_string[i] = nibble;             // Move ascii char into string
        hex_num >>= 4;                     // Shift right by 4 for next nibble
        i++;
    }
        
    if (pad) hex_string[i++] = '0';  // Pad out string with extra 0    

    // Add initial "0x" to front of hex string
    hex_string[i++] = 'x';
    hex_string[i++] = '0';
    hex_string[i] = '\0';   // Null terminate string

    // Number is stored backwards in hex_string, reverse the string by swapping ends
    //   until they meet in the middle
    i--;    // Skip null byte
    for (j = 0; j < i; j++, i--) {
        temp          = hex_string[j];
        hex_string[j] = hex_string[i];
        hex_string[i] = temp;
    }

    // Print hex string
    return hex_string;
}

uint8_t* strncpy(uint8_t *dst, const uint8_t *src, const uint8_t len) {
    for (uint8_t i = 0; src[i] && i < len; i++)
        dst[i] = src[i];

    return dst;
}
