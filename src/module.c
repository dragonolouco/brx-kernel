#include "brx_module.h"

void brx_modules_init(brx_module_registry_t *registry) {
    registry->count = 0;
}

int brx_module_register(brx_module_registry_t *registry, brx_module_t module) {
    if (registry->count >= BRX_MODULE_MAX || !module.name || !module.version) return -1;
    registry->entries[registry->count++] = module;
    return 0;
}

uint64_t brx_module_count(const brx_module_registry_t *registry) {
    return registry->count;
}

const brx_module_t *brx_module_at(const brx_module_registry_t *registry, uint64_t index) {
    if (index >= registry->count) return 0;
    return &registry->entries[index];
}
