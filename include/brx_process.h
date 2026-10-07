#ifndef BRX_PROCESS_H
#define BRX_PROCESS_H

#include <stdint.h>
#include "brx_vmem.h"

#define BRX_PROCESS_MAX 32
#define BRX_PROCESS_NAME_MAX 32

typedef enum {
    BRX_PROCESS_FREE = 0,
    BRX_PROCESS_CREATED,
    BRX_PROCESS_READY,
    BRX_PROCESS_RUNNING,
    BRX_PROCESS_STOPPED,
    BRX_PROCESS_FAILED
} brx_process_state_t;

typedef struct {
    uint64_t pid;
    brx_process_state_t state;
    uint64_t entrypoint;
    uint64_t stack_pointer;
    char name[BRX_PROCESS_NAME_MAX];
    brx_vm_space_t address_space;
} brx_process_t;

typedef struct {
    brx_process_t entries[BRX_PROCESS_MAX];
    uint64_t next_pid;
} brx_process_table_t;

void brx_process_table_init(brx_process_table_t *table);
brx_process_t *brx_process_create(brx_process_table_t *table, const char *name, uint64_t entrypoint);
int brx_process_prepare_stack(brx_process_t *process, uint64_t stack_virtual, uint64_t stack_physical, uint64_t stack_length);
brx_process_t *brx_process_find(brx_process_table_t *table, uint64_t pid);

#endif
