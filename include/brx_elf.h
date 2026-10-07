#ifndef BRX_ELF_H
#define BRX_ELF_H

#include <stdint.h>

typedef struct {
    uint64_t entry;
    uint64_t lowest_load_address;
    uint64_t highest_load_address;
    uint64_t loadable_segments;
    uint64_t memory_size;
    uint8_t is_position_independent;
} brx_elf_image_t;

/* Validates the image and extracts only load information; it does not trust file offsets. */
int brx_elf_validate_x86_64(const void *image, uint64_t length, brx_elf_image_t *result);

#endif
