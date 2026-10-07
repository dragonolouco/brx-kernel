#include "brx_pci.h"

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

static inline void out32(uint16_t port, uint32_t value) {
    __asm__ volatile ("outl %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint32_t in32(uint16_t port) {
    uint32_t value;
    __asm__ volatile ("inl %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

uint32_t brx_pci_read32(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset) {
    uint32_t address = 0x80000000U
        | ((uint32_t)bus << 16)
        | ((uint32_t)slot << 11)
        | ((uint32_t)function << 8)
        | (offset & 0xfc);
    out32(PCI_CONFIG_ADDRESS, address);
    return in32(PCI_CONFIG_DATA);
}

void brx_pci_scan(brx_pci_inventory_t *inventory) {
    inventory->count = 0;
    for (uint16_t bus = 0; bus < 256 && inventory->count < BRX_PCI_MAX_DEVICES; ++bus) {
        for (uint8_t slot = 0; slot < 32 && inventory->count < BRX_PCI_MAX_DEVICES; ++slot) {
            for (uint8_t function = 0; function < 8 && inventory->count < BRX_PCI_MAX_DEVICES; ++function) {
                uint32_t vendor_device = brx_pci_read32(bus, slot, function, 0);
                uint16_t vendor = vendor_device & 0xffff;
                if (vendor == 0xffff) {
                    if (function == 0) break;
                    continue;
                }
                uint32_t class_data = brx_pci_read32(bus, slot, function, 8);
                uint32_t header_data = brx_pci_read32(bus, slot, function, 0x0c);
                brx_pci_device_t *device = &inventory->devices[inventory->count++];
                device->bus = bus;
                device->slot = slot;
                device->function = function;
                device->vendor_id = vendor;
                device->device_id = vendor_device >> 16;
                device->class_code = class_data >> 24;
                device->subclass = class_data >> 16;
                device->prog_if = class_data >> 8;
                device->header_type = header_data >> 16;
                if (function == 0 && !(device->header_type & 0x80)) break;
            }
        }
    }
}
