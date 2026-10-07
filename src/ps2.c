#include "brx_ps2.h"

#define PS2_DATA 0x60
#define PS2_STATUS 0x64
#define PS2_COMMAND 0x64

static volatile uint64_t events;
static volatile uint8_t last_scancode;
static volatile uint64_t mouse_events;
static volatile uint8_t last_mouse_byte;

static inline void out8(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t in8(uint16_t port) {
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static void wait_input_clear(void) {
    for (uint32_t i = 0; i < 100000; ++i) if (!(in8(PS2_STATUS) & 2)) return;
}

static void wait_output_full(void) {
    for (uint32_t i = 0; i < 100000; ++i) if (in8(PS2_STATUS) & 1) return;
}

void brx_ps2_init(void) {
    wait_input_clear();
    out8(PS2_COMMAND, 0xAD);
    wait_input_clear();
    out8(PS2_COMMAND, 0xA7);
    wait_input_clear();
    out8(PS2_COMMAND, 0x20);
    wait_output_full();
    (void)in8(PS2_DATA);
    wait_input_clear();
    out8(PS2_COMMAND, 0xAE);
    wait_input_clear();
    out8(PS2_DATA, 0xF4);
    wait_output_full();
    (void)in8(PS2_DATA);

    wait_input_clear();
    out8(PS2_COMMAND, 0xA8);
    wait_input_clear();
    out8(PS2_COMMAND, 0xD4);
    wait_input_clear();
    out8(PS2_DATA, 0xF4);
    wait_output_full();
    (void)in8(PS2_DATA);
}

void brx_ps2_handle_irq(void) {
    if (in8(PS2_STATUS) & 1) {
        last_scancode = in8(PS2_DATA);
        events++;
    }
}

uint64_t brx_ps2_event_count(void) { return events; }
uint8_t brx_ps2_last_scancode(void) { return last_scancode; }
void brx_ps2_handle_mouse_irq(void) {
    if (in8(PS2_STATUS) & 1) {
        last_mouse_byte = in8(PS2_DATA);
        mouse_events++;
    }
}
uint64_t brx_ps2_mouse_event_count(void) { return mouse_events; }
uint8_t brx_ps2_last_mouse_byte(void) { return last_mouse_byte; }
