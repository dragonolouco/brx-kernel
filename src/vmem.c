#include "brx_vmem.h"

static int aligned(uint64_t value) { return (value & (BRX_VM_PAGE_SIZE - 1)) == 0; }
static int range_end(uint64_t start, uint64_t length, uint64_t *end) {
    if (length == 0 || start > UINT64_MAX - length) return 0;
    *end = start + length;
    return 1;
}

void brx_vm_space_init(brx_vm_space_t *space) {
    space->count = 0;
    space->next_hint = 0x0000000100000000ULL;
}

int brx_vm_overlaps(const brx_vm_space_t *space, uint64_t start, uint64_t length) {
    uint64_t end;
    if (!range_end(start, length, &end)) return 1;
    for (uint64_t i = 0; i < space->count; ++i) {
        uint64_t region_end = space->regions[i].virtual_start + space->regions[i].length;
        if (start < region_end && space->regions[i].virtual_start < end) return 1;
    }
    return 0;
}

int brx_vm_map(brx_vm_space_t *space, uint64_t virtual_start, uint64_t physical_start, uint64_t length, uint32_t flags) {
    if (space->count >= BRX_VM_MAX_REGIONS || !aligned(virtual_start) || !aligned(physical_start) || !aligned(length) || brx_vm_overlaps(space, virtual_start, length)) return -1;
    uint64_t end;
    if (!range_end(virtual_start, length, &end)) return -1;
    brx_vm_region_t region = { virtual_start, physical_start, length, flags, 0 };
    uint64_t position = space->count;
    while (position > 0 && space->regions[position - 1].virtual_start > virtual_start) {
        space->regions[position] = space->regions[position - 1];
        --position;
    }
    space->regions[position] = region;
    space->count++;
    if (end > space->next_hint) space->next_hint = end;
    return 0;
}

int brx_vm_unmap(brx_vm_space_t *space, uint64_t virtual_start, uint64_t length) {
    if (!aligned(virtual_start) || !aligned(length)) return -1;
    for (uint64_t i = 0; i < space->count; ++i) {
        if (space->regions[i].virtual_start == virtual_start && space->regions[i].length == length) {
            for (; i + 1 < space->count; ++i) space->regions[i] = space->regions[i + 1];
            --space->count;
            return 0;
        }
    }
    return -1;
}

const brx_vm_region_t *brx_vm_find(const brx_vm_space_t *space, uint64_t address) {
    for (uint64_t i = 0; i < space->count; ++i) {
        uint64_t end = space->regions[i].virtual_start + space->regions[i].length;
        if (address >= space->regions[i].virtual_start && address < end) return &space->regions[i];
    }
    return 0;
}
