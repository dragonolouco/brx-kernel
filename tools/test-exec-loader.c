#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "brx_elf.h"
#include "brx_exec.h"

static int test_elf(void) {
    uint8_t image[256] = {0};
    image[0] = 0x7f; image[1] = 'E'; image[2] = 'L'; image[3] = 'F';
    image[4] = 2; image[5] = 1;
    uint16_t type = 2, machine = 62, ehsize = 64, phentsize = 56, phnum = 1;
    uint64_t entry = 0x400000, phoff = 64;
    memcpy(image + 16, &type, 2); memcpy(image + 18, &machine, 2);
    memcpy(image + 24, &entry, 8); memcpy(image + 32, &phoff, 8);
    memcpy(image + 52, &ehsize, 2); memcpy(image + 54, &phentsize, 2); memcpy(image + 56, &phnum, 2);
    uint32_t load = 1, flags = 5; uint64_t offset = 0, vaddr = 0x400000, filesz = 256, memsz = 4096;
    memcpy(image + 64, &load, 4); memcpy(image + 68, &flags, 4);
    memcpy(image + 72, &offset, 8); memcpy(image + 80, &vaddr, 8);
    memcpy(image + 96, &filesz, 8); memcpy(image + 104, &memsz, 8);
    brx_elf_image_t info;
    return brx_elf_validate_x86_64(image, sizeof(image), &info) && info.loadable_segments == 1 && info.entry == entry;
}

int main(void) {
    const uint8_t pe[] = { 'M', 'Z' };
    const uint8_t script[] = { '#', '!', '/', 'b', 'i', 'n' };
    if (!test_elf()) return 1;
    if (brx_exec_detect(pe, sizeof(pe)) != BRX_EXEC_UNKNOWN) return 2;
    if (brx_exec_detect(script, sizeof(script)) != BRX_EXEC_SCRIPT) return 3;
    puts("BRX loader host tests: PASS");
    return 0;
}
