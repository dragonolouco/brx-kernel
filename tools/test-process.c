#include <stdio.h>
#include "brx_process.h"

int main(void) {
    brx_process_table_t table;
    brx_process_table_init(&table);
    brx_process_t *process = brx_process_create(&table, "sample-app", 0x401000);
    if (!process || process->pid != 1) return 1;
    if (brx_process_prepare_stack(process, 0x7000000000ULL, 0x200000, 0x2000) != 0) return 2;
    if (process->state != BRX_PROCESS_READY || process->stack_pointer != 0x7000002000ULL) return 3;
    if (!brx_vm_find(&process->address_space, 0x7000001000ULL)) return 4;
    if (brx_vm_map(&process->address_space, 0x7000001000ULL, 0x300000, 0x1000, BRX_VM_READ) == 0) return 5;
    if (brx_process_find(&table, process->pid) != process) return 6;
    puts("BRX process/vmem tests: PASS");
    return 0;
}
