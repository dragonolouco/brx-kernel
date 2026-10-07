#include "brx_memory.h"

static uint64_t align_up(uint64_t value, uint64_t alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
}

void brx_memory_init(brx_physical_memory_t *memory, brx_memmap_response_t *map) {
    memory->region_count = 0;
    memory->usable_bytes = 0;
    memory->allocated_pages = 0;

    if (!map || !map->entries) return;
    for (uint64_t i = 0; i < map->entry_count && memory->region_count < 32; ++i) {
        brx_memmap_entry_t *entry = map->entries[i];
        if (!entry || entry->type != BRX_MEM_USABLE || entry->length < BRX_PAGE_SIZE) continue;

        uint64_t base = align_up(entry->base, BRX_PAGE_SIZE);
        uint64_t end = entry->base + entry->length;
        if (end <= base + BRX_PAGE_SIZE) continue;

        brx_page_region_t *region = &memory->regions[memory->region_count++];
        region->base = base;
        region->next = base;
        region->end = end & ~(BRX_PAGE_SIZE - 1);
        region->length = region->end - base;
        memory->usable_bytes += region->length;
    }
}

uint64_t brx_page_alloc(brx_physical_memory_t *memory) {
    for (uint64_t i = 0; i < memory->region_count; ++i) {
        brx_page_region_t *region = &memory->regions[i];
        if (region->next + BRX_PAGE_SIZE <= region->end) {
            uint64_t page = region->next;
            region->next += BRX_PAGE_SIZE;
            memory->allocated_pages++;
            return page;
        }
    }
    return 0;
}

uint64_t brx_memory_usable_bytes(const brx_physical_memory_t *memory) {
    return memory->usable_bytes;
}

uint64_t brx_memory_allocated_pages(const brx_physical_memory_t *memory) {
    return memory->allocated_pages;
}
