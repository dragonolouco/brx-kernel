#ifndef BRX_CPU_H
#define BRX_CPU_H

static inline __attribute__((noreturn)) void brx_cpu_halt(void) {
    for (;;) __asm__ volatile ("cli; hlt");
}

#endif
