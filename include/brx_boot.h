#ifndef BRX_BOOT_H
#define BRX_BOOT_H

#include <stdint.h>

#define BRX_COMMON_MAGIC_A 0xc7b1dd30df4c8b88ULL
#define BRX_COMMON_MAGIC_B 0x0a82e883a194f07bULL
#define BRX_REQUESTS_START_A 0xf6b8f4b39de7d1aeULL
#define BRX_REQUESTS_START_B 0xfab91a6940fcb9cfULL
#define BRX_REQUESTS_START_C 0x785c6ed015d3e316ULL
#define BRX_REQUESTS_START_D 0x181e920a7852b9d9ULL
#define BRX_REQUESTS_END_A 0xadc0e0531bb10d03ULL
#define BRX_REQUESTS_END_B 0x9572709f31764c62ULL
#define BRX_BASE_REV_A 0xf9562b2d5c95a6c8ULL
#define BRX_BASE_REV_B 0x6a7b384944536bdcULL
#define BRX_MEMMAP_ID_A BRX_COMMON_MAGIC_A
#define BRX_MEMMAP_ID_B BRX_COMMON_MAGIC_B
#define BRX_MEMMAP_ID_C 0x67cf3d9d378a806fULL
#define BRX_MEMMAP_ID_D 0xe304acdfc50c3c62ULL

#define BRX_MEM_USABLE 0
#define BRX_MEM_RESERVED 1
#define BRX_MEM_ACPI_RECLAIMABLE 2
#define BRX_MEM_ACPI_NVS 3
#define BRX_MEM_BAD 4
#define BRX_MEM_BOOTLOADER 5
#define BRX_MEM_KERNEL 6
#define BRX_MEM_FRAMEBUFFER 7

typedef struct {
    uint64_t base;
    uint64_t length;
    uint64_t type;
} brx_memmap_entry_t;

typedef struct {
    uint64_t revision;
    uint64_t entry_count;
    brx_memmap_entry_t **entries;
} brx_memmap_response_t;

typedef struct {
    uint64_t id[4];
    uint64_t revision;
    brx_memmap_response_t *response;
} brx_memmap_request_t;

#endif
