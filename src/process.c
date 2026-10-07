#include "brx_process.h"

static void copy_name(char *destination, const char *source) {
    uint64_t i = 0;
    if (source) while (source[i] && i + 1 < BRX_PROCESS_NAME_MAX) { destination[i] = source[i]; ++i; }
    destination[i] = 0;
}

void brx_process_table_init(brx_process_table_t *table) {
    table->next_pid = 1;
    for (uint64_t i = 0; i < BRX_PROCESS_MAX; ++i) table->entries[i].state = BRX_PROCESS_FREE;
}

brx_process_t *brx_process_create(brx_process_table_t *table, const char *name, uint64_t entrypoint) {
    for (uint64_t i = 0; i < BRX_PROCESS_MAX; ++i) {
        brx_process_t *process = &table->entries[i];
        if (process->state != BRX_PROCESS_FREE) continue;
        process->pid = table->next_pid++;
        process->state = BRX_PROCESS_CREATED;
        process->entrypoint = entrypoint;
        process->stack_pointer = 0;
        copy_name(process->name, name);
        brx_vm_space_init(&process->address_space);
        return process;
    }
    return 0;
}

int brx_process_prepare_stack(brx_process_t *process, uint64_t stack_virtual, uint64_t stack_physical, uint64_t stack_length) {
    if (!process || process->state != BRX_PROCESS_CREATED || brx_vm_map(&process->address_space, stack_virtual, stack_physical, stack_length, BRX_VM_READ | BRX_VM_WRITE | BRX_VM_USER) != 0) return -1;
    process->stack_pointer = stack_virtual + stack_length;
    process->state = BRX_PROCESS_READY;
    return 0;
}

brx_process_t *brx_process_find(brx_process_table_t *table, uint64_t pid) {
    for (uint64_t i = 0; i < BRX_PROCESS_MAX; ++i) if (table->entries[i].state != BRX_PROCESS_FREE && table->entries[i].pid == pid) return &table->entries[i];
    return 0;
}
