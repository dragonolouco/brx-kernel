#ifndef BRX_VMEM_H
#define BRX_VMEM_H

#include <stdint.h>

#define BRX_VM_MAX_REGIONS 64
#define BRX_VM_PAGE_SIZE 4096
#define BRX_VM_READ 1
#define BRX_VM_WRITE 2
#define BRX_VM_EXEC 4
#define BRX_VM_USER 8

typedef struct {
    uint64_t virtual_start;
    uint64_t physical_start;
    uint64_t length;
    uint32_t flags;
    uint32_t reserved;
} brx_vm_region_t;

typedef struct {
    brx_vm_region_t regions[BRX_VM_MAX_REGIONS];
    uint64_t count;
    uint64_t next_hint;
} brx_vm_space_t;

void brx_vm_space_init(brx_vm_space_t *space);
int brx_vm_map(brx_vm_space_t *space, uint64_t virtual_start, uint64_t physical_start, uint64_t length, uint32_t flags);
int brx_vm_unmap(brx_vm_space_t *space, uint64_t virtual_start, uint64_t length);
const brx_vm_region_t *brx_vm_find(const brx_vm_space_t *space, uint64_t address);
int brx_vm_overlaps(const brx_vm_space_t *space, uint64_t start, uint64_t length);

#endif
