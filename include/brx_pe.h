#ifndef BRX_PE_H
#define BRX_PE_H

#include <stdint.h>

typedef struct {
    uint64_t image_base;
    uint64_t entry_address;
    uint64_t image_size;
    uint64_t header_size;
    uint64_t section_count;
    uint64_t executable_sections;
} brx_pe_image_t;

int brx_pe_validate_x86_64(const void *image, uint64_t length, brx_pe_image_t *result);

#endif
