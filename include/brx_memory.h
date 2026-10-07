#ifndef BRX_MEMORY_H
#define BRX_MEMORY_H

#include <stdint.h>
#include "brx_boot.h"

#define BRX_PAGE_SIZE 4096ULL

typedef struct {
    uint64_t base;
    uint64_t length;
    uint64_t next;
    uint64_t end;
} brx_page_region_t;

typedef struct {
    brx_page_region_t regions[32];
    uint64_t region_count;
    uint64_t usable_bytes;
    uint64_t allocated_pages;
} brx_physical_memory_t;

void brx_memory_init(brx_physical_memory_t *memory, brx_memmap_response_t *map);
uint64_t brx_page_alloc(brx_physical_memory_t *memory);
uint64_t brx_memory_usable_bytes(const brx_physical_memory_t *memory);
uint64_t brx_memory_allocated_pages(const brx_physical_memory_t *memory);

#endif
