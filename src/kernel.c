#include <stdint.h>
#include "brx_boot.h"
#include "brx_memory.h"
#include "brx_interrupts.h"
#include "brx_module.h"
#include "brx_pci.h"
#include "brx_ps2.h"
#include "brx_cpu.h"

#define COM1 0x3F8

__attribute__((used, section(".data")))
static volatile uint64_t brx_data_anchor = 0x4252584441544131ULL;

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

static void serial_module(const brx_module_t *module) {
    serial_write("module: ");
    serial_write(module->name);
    serial_write(" state=");
    serial_write(module->state == BRX_MODULE_READY ? "ready" : "planned");
    serial_write("\n");
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
    serial_write("BRX Kernel\n");
    serial_write("status: native x86_64 kernel online\n");

    brx_memmap_response_t *map = brx_memory_request.response;
    if (!map) {
        serial_write("error: bootloader did not provide a memory map\n");
        brx_cpu_halt();
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

    brx_pci_inventory_t pci;
    brx_pci_scan(&pci);
    serial_write("pci devices: ");
    serial_hex(pci.count);
    serial_write("\n");
    for (uint64_t i = 0; i < pci.count && i < 8; ++i) {
        serial_write("pci ");
        serial_hex(pci.devices[i].bus);
        serial_write(":");
        serial_hex(pci.devices[i].slot);
        serial_write(" vendor=");
        serial_hex(pci.devices[i].vendor_id);
        serial_write(" device=");
        serial_hex(pci.devices[i].device_id);
        serial_write(" class=");
        serial_hex(pci.devices[i].class_code);
        serial_write("/");
        serial_hex(pci.devices[i].subclass);
        serial_write("\n");
    }

    brx_physical_memory_t physical_memory;
    brx_memory_init(&physical_memory, map);
    uint64_t first_page = brx_page_alloc(&physical_memory);
    serial_write("physical allocator regions: ");
    serial_hex(physical_memory.region_count);
    serial_write("\nfirst allocated page: ");
    serial_hex(first_page);
    serial_write("\n");

    brx_module_registry_t modules;
    brx_modules_init(&modules);
    brx_module_register(&modules, (brx_module_t){ "core.memory", "current", "physical page allocator", 1, 1, BRX_MODULE_READY });
    brx_module_register(&modules, (brx_module_t){ "core.interrupts", "current", "IDT PIC PIT", 1, 2, BRX_MODULE_READY });
    brx_module_register(&modules, (brx_module_t){ "compat.posix", "planned", "POSIX service layer", 1, 4, BRX_MODULE_PLANNED });
    brx_module_register(&modules, (brx_module_t){ "compat.linux-abi", "planned", "Linux syscall translation", 1, 8, BRX_MODULE_PLANNED });
    brx_module_register(&modules, (brx_module_t){ "user.git", "planned", "Git user-space integration", 1, 16, BRX_MODULE_PLANNED });
    for (uint64_t i = 0; i < brx_module_count(&modules); ++i) serial_module(brx_module_at(&modules, i));

    brx_ps2_init();
    serial_write("input: PS/2 keyboard and mouse drivers initialized\n");
    brx_interrupts_init();
    brx_timer_init(100);
    brx_interrupts_enable();
    serial_write("status: BRX modular core online\n");
    serial_write("status: BRX bootstrap complete\n");
    serial_write("idle: timer interrupts enabled; scheduler pending\n");

    uint64_t last_report = 0;
    for (;;) {
        __asm__ volatile ("hlt");
        uint64_t now = brx_timer_ticks();
        if (now >= last_report + 100) {
            last_report = now;
            serial_write("timer ticks: ");
            serial_hex(now);
            serial_write("\n");
        }
    }
}
