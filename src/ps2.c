#include "brx_ps2.h"

#define PS2_DATA 0x60
#define PS2_STATUS 0x64
#define PS2_COMMAND 0x64

static volatile uint64_t events;
static volatile uint8_t last_scancode;
static volatile uint64_t mouse_events;
static volatile uint8_t last_mouse_byte;
static volatile uint8_t mouse_packet[3];
static volatile uint8_t mouse_packet_index;
static volatile int64_t mouse_x;
static volatile int64_t mouse_y;
static volatile uint8_t mouse_buttons;
static volatile char key_queue[128];
static volatile uint8_t key_read;
static volatile uint8_t key_write;
static volatile uint8_t shift_down;

static char key_from_scancode(uint8_t code) {
    static const char letters[] = "qwertyuiopasdfghjklzxcvbnm";
    if (code >= 0x10 && code <= 0x19) return letters[code - 0x10];
    if (code >= 0x1e && code <= 0x26) return letters[10 + code - 0x1e];
    if (code >= 0x2c && code <= 0x32) return letters[19 + code - 0x2c];
    switch (code) {
        case 0x02: return '1'; case 0x03: return '2'; case 0x04: return '3'; case 0x05: return '4'; case 0x06: return '5';
        case 0x07: return '6'; case 0x08: return '7'; case 0x09: return '8'; case 0x0a: return '9'; case 0x0b: return '0';
        case 0x1c: return '\n'; case 0x39: return ' '; case 0x0e: return '\b'; case 0x35: return '/'; case 0x34: return '.';
        default: return 0;
    }
}

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
    uint8_t controller_config = in8(PS2_DATA);
    controller_config |= 0x03;
    controller_config &= (uint8_t)~0x30;
    wait_input_clear();
    out8(PS2_COMMAND, 0x60);
    wait_input_clear();
    out8(PS2_DATA, controller_config);
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

static void handle_keyboard_byte(uint8_t scancode) {
        last_scancode = scancode;
        events++;
        if (scancode == 0x2a || scancode == 0x36) { shift_down = 1; return; }
        if (scancode == 0xaa || scancode == 0xb6) { shift_down = 0; return; }
        if (scancode & 0x80) return;
        char key = key_from_scancode(scancode);
        if (key >= 'a' && key <= 'z' && shift_down) key = (char)(key - 'a' + 'A');
        if (key && (uint8_t)(key_write + 1) != key_read) { key_queue[key_write] = key; key_write = (uint8_t)(key_write + 1); }
}
void brx_ps2_handle_irq(void) {
    if (in8(PS2_STATUS) & 1) handle_keyboard_byte(in8(PS2_DATA));
}
void brx_ps2_poll(void) {
    if (in8(PS2_STATUS) & 1) handle_keyboard_byte(in8(PS2_DATA));
}

uint64_t brx_ps2_event_count(void) { return events; }
uint8_t brx_ps2_last_scancode(void) { return last_scancode; }
void brx_ps2_handle_mouse_irq(void) {
    if (in8(PS2_STATUS) & 1) {
        last_mouse_byte = in8(PS2_DATA);
        if (mouse_packet_index == 0 && !(last_mouse_byte & 0x08)) return;
        mouse_packet[mouse_packet_index++] = last_mouse_byte;
        if (mouse_packet_index == 3) {
            int16_t dx = (int8_t)mouse_packet[1];
            int16_t dy = (int8_t)mouse_packet[2];
            if (mouse_packet[0] & 0x40) dx = 0;
            if (mouse_packet[0] & 0x80) dy = 0;
            mouse_x += dx;
            mouse_y -= dy;
            mouse_buttons = mouse_packet[0] & 0x07;
            mouse_packet_index = 0;
            mouse_events++;
        }
    }
}
uint64_t brx_ps2_mouse_event_count(void) { return mouse_events; }
uint8_t brx_ps2_last_mouse_byte(void) { return last_mouse_byte; }
int64_t brx_ps2_mouse_x(void) { return mouse_x; }
int64_t brx_ps2_mouse_y(void) { return mouse_y; }
uint8_t brx_ps2_mouse_buttons(void) { return mouse_buttons; }
int brx_ps2_read_key(void) {
    if (key_read == key_write) return -1;
    char key = key_queue[key_read];
    key_read = (uint8_t)(key_read + 1);
    return (unsigned char)key;
}
