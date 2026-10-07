#include <stdint.h>
#include "brx_boot.h"

#define COM1 0x3F8

static inline void out8(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t in8(uint16_t port) {
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static void serial_init(void) {
    out8(COM1 + 1, 0x00);
    out8(COM1 + 3, 0x80);
    out8(COM1 + 0, 0x03);
    out8(COM1 + 1, 0x00);
    out8(COM1 + 3, 0x03);
    out8(COM1 + 2, 0xC7);
    out8(COM1 + 4, 0x0B);
}

static void serial_putc(char c) {
    while ((in8(COM1 + 5) & 0x20) == 0) { }
    out8(COM1, (uint8_t)c);
}

static void serial_write(const char *text) {
    while (*text) {
        if (*text == '\n') serial_putc('\r');
        serial_putc(*text++);
    }
}

static void serial_hex(uint64_t value) {
    static const char digits[] = "0123456789abcdef";
    serial_write("0x");
    for (int shift = 60; shift >= 0; shift -= 4) serial_putc(digits[(value >> shift) & 0xf]);
}

static void halt_forever(void) {
    for (;;) __asm__ volatile ("hlt");
}

__attribute__((used, section(".limine_requests_start"), aligned(8)))
static volatile uint64_t brx_requests_start[] = {
    BRX_REQUESTS_START_A, BRX_REQUESTS_START_B, BRX_REQUESTS_START_C, BRX_REQUESTS_START_D
};

__attribute__((used, section(".limine_requests"), aligned(8)))
static volatile uint64_t brx_base_revision[] = { BRX_BASE_REV_A, BRX_BASE_REV_B, 6 };

__attribute__((used, section(".limine_requests"), aligned(8)))
static volatile brx_memmap_request_t brx_memory_request = {
    { BRX_MEMMAP_ID_A, BRX_MEMMAP_ID_B, BRX_MEMMAP_ID_C, BRX_MEMMAP_ID_D }, 0, 0
};

__attribute__((used, section(".limine_requests_end"), aligned(8)))
static volatile uint64_t brx_requests_end[] = { BRX_REQUESTS_END_A, BRX_REQUESTS_END_B };

void kmain(void) {
    serial_init();
    serial_write("BRX Kernel 0.1.0\n");
    serial_write("status: native x86_64 kernel online\n");

    brx_memmap_response_t *map = brx_memory_request.response;
    if (!map) {
        serial_write("error: bootloader did not provide a memory map\n");
        halt_forever();
    }

    uint64_t usable = 0;
    uint64_t regions = map->entry_count;
    for (uint64_t i = 0; i < regions; ++i) {
        brx_memmap_entry_t *entry = map->entries[i];
        if (entry->type == BRX_MEM_USABLE) usable += entry->length;
    }

    serial_write("memory regions: ");
    serial_hex(regions);
    serial_write("\nusable memory: ");
    serial_hex(usable);
    serial_write(" bytes\n");
    serial_write("status: BRX bootstrap complete\n");
    serial_write("idle: waiting for scheduler bring-up\n");
    halt_forever();
}
