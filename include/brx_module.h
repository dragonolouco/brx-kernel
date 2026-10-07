#ifndef BRX_MODULE_H
#define BRX_MODULE_H

#include <stdint.h>

#define BRX_MODULE_ABI_VERSION 1
#define BRX_MODULE_MAX 32

typedef enum {
    BRX_MODULE_PLANNED = 0,
    BRX_MODULE_READY = 1,
    BRX_MODULE_FAILED = 2
} brx_module_state_t;

typedef struct {
    const char *name;
    const char *version;
    const char *description;
    uint64_t abi_version;
    uint64_t capabilities;
    brx_module_state_t state;
} brx_module_t;

typedef struct {
    brx_module_t entries[BRX_MODULE_MAX];
    uint64_t count;
} brx_module_registry_t;

void brx_modules_init(brx_module_registry_t *registry);
int brx_module_register(brx_module_registry_t *registry, brx_module_t module);
uint64_t brx_module_count(const brx_module_registry_t *registry);
const brx_module_t *brx_module_at(const brx_module_registry_t *registry, uint64_t index);

#endif
