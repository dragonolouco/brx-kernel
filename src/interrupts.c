#include <stdint.h>
#include "brx_interrupts.h"
#include "brx_ps2.h"

#define PIC_MASTER_CMD 0x20
#define PIC_MASTER_DATA 0x21
#define PIC_SLAVE_CMD 0xA0
#define PIC_SLAVE_DATA 0xA1
#define PIT_COMMAND 0x43
#define PIT_CHANNEL0 0x40

typedef struct __attribute__((packed)) {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t attributes;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t reserved;
} brx_idt_gate_t;

typedef struct __attribute__((packed)) {
    uint16_t limit;
    uint64_t base;
} brx_idtr_t;

extern void brx_isr_default(void);
extern void brx_isr_timer(void);
extern void brx_isr_keyboard(void);
extern void brx_isr_mouse(void);

static brx_idt_gate_t idt[256] __attribute__((aligned(16)));
static volatile uint64_t ticks;

static inline void out8(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline void io_wait(void) {
    out8(0x80, 0);
}

static void idt_set_gate(uint8_t vector, void (*handler)(void), uint8_t attributes) {
    uint64_t address = (uint64_t)handler;
    idt[vector].offset_low = address & 0xffff;
    idt[vector].selector = 0x08;
    idt[vector].ist = 0;
    idt[vector].attributes = attributes;
    idt[vector].offset_mid = (address >> 16) & 0xffff;
    idt[vector].offset_high = address >> 32;
    idt[vector].reserved = 0;
}

static void pic_remap(void) {
    uint8_t master_mask = 0xff;
    uint8_t slave_mask = 0xff;
    out8(PIC_MASTER_CMD, 0x11); io_wait();
    out8(PIC_SLAVE_CMD, 0x11); io_wait();
    out8(PIC_MASTER_DATA, 0x20); io_wait();
    out8(PIC_SLAVE_DATA, 0x28); io_wait();
    out8(PIC_MASTER_DATA, 0x04); io_wait();
    out8(PIC_SLAVE_DATA, 0x02); io_wait();
    out8(PIC_MASTER_DATA, 0x01); io_wait();
    out8(PIC_SLAVE_DATA, 0x01); io_wait();
    out8(PIC_MASTER_DATA, master_mask);
    out8(PIC_SLAVE_DATA, slave_mask);
}

void brx_interrupts_init(void) {
    for (uint16_t vector = 0; vector < 256; ++vector) {
        idt_set_gate((uint8_t)vector, brx_isr_default, 0x8e);
    }
    idt_set_gate(32, brx_isr_timer, 0x8e);
    idt_set_gate(33, brx_isr_keyboard, 0x8e);
    idt_set_gate(44, brx_isr_mouse, 0x8e);
    pic_remap();
    brx_idtr_t descriptor = { sizeof(idt) - 1, (uint64_t)&idt[0] };
    __asm__ volatile ("lidt %0" : : "m"(descriptor));
}

void brx_timer_init(uint32_t frequency_hz) {
    if (frequency_hz == 0) frequency_hz = 100;
    uint32_t divisor = 1193182U / frequency_hz;
    if (divisor > 65535) divisor = 65535;
    if (divisor < 1) divisor = 1;
    out8(PIT_COMMAND, 0x36);
    out8(PIT_CHANNEL0, divisor & 0xff);
    out8(PIT_CHANNEL0, (divisor >> 8) & 0xff);
    out8(PIC_MASTER_DATA, 0xfc);
    out8(PIC_SLAVE_DATA, 0xef);
}

void brx_interrupts_enable(void) {
    __asm__ volatile ("sti");
}

uint64_t brx_timer_ticks(void) {
    return ticks;
}

void brx_interrupt_dispatch(uint64_t vector) {
    if (vector == 32) {
        ticks++;
        out8(PIC_MASTER_CMD, 0x20);
    } else if (vector == 33) {
        brx_ps2_handle_irq();
        out8(PIC_MASTER_CMD, 0x20);
    } else if (vector == 44) {
        brx_ps2_handle_mouse_irq();
        out8(PIC_SLAVE_CMD, 0x20);
        out8(PIC_MASTER_CMD, 0x20);
    } else if (vector >= 40 && vector <= 47) {
        out8(PIC_SLAVE_CMD, 0x20);
        out8(PIC_MASTER_CMD, 0x20);
    }
}
