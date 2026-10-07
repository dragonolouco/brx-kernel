#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "brx_pe.h"

static void put16(uint8_t *p, uint16_t v) { p[0] = v; p[1] = v >> 8; }
static void put32(uint8_t *p, uint32_t v) { p[0] = v; p[1] = v >> 8; p[2] = v >> 16; p[3] = v >> 24; }
static void put64(uint8_t *p, uint64_t v) { put32(p, v); put32(p + 4, v >> 32); }

int main(void) {
    uint8_t image[2048] = {0};
    image[0] = 'M'; image[1] = 'Z'; put32(image + 0x3c, 0x80);
    uint8_t *pe = image + 0x80;
    pe[0] = 'P'; pe[1] = 'E';
    uint8_t *coff = pe + 4;
    put16(coff, 0x8664); put16(coff + 2, 1); put16(coff + 16, 0xf0);
    uint8_t *optional = coff + 20;
    put16(optional, 0x20b); put32(optional + 16, 0x1000); put64(optional + 24, 0x140000000ULL);
    put32(optional + 32, 0x1000); put32(optional + 56, 0x2000); put32(optional + 60, 0x400);
    uint8_t *section = optional + 0xf0;
    memcpy(section, ".text", 5); put32(section + 8, 0x100); put32(section + 12, 0x1000);
    put32(section + 16, 0x200); put32(section + 20, 0x400); put32(section + 36, 0x60000020);
    brx_pe_image_t info;
    if (!brx_pe_validate_x86_64(image, sizeof(image), &info)) return 1;
    if (info.section_count != 1 || info.executable_sections != 1 || info.entry_address != 0x140001000ULL) return 2;
    image[0x80 + 4 + 20 + 17] = 0;
    if (brx_pe_validate_x86_64(image, sizeof(image), &info)) return 3;
    puts("BRX PE loader tests: PASS");
    return 0;
}
