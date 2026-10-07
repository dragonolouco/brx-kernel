#ifndef BRX_PCI_H
#define BRX_PCI_H

#include <stdint.h>

#define BRX_PCI_MAX_DEVICES 64

typedef struct {
    uint8_t bus;
    uint8_t slot;
    uint8_t function;
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t class_code;
    uint8_t subclass;
    uint8_t prog_if;
    uint8_t header_type;
} brx_pci_device_t;

typedef struct {
    brx_pci_device_t devices[BRX_PCI_MAX_DEVICES];
    uint64_t count;
} brx_pci_inventory_t;

void brx_pci_scan(brx_pci_inventory_t *inventory);
uint32_t brx_pci_read32(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset);

#endif
