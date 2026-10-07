#ifndef BRX_PS2_H
#define BRX_PS2_H

#include <stdint.h>

void brx_ps2_init(void);
void brx_ps2_handle_irq(void);
void brx_ps2_handle_mouse_irq(void);
uint64_t brx_ps2_event_count(void);
uint8_t brx_ps2_last_scancode(void);
uint64_t brx_ps2_mouse_event_count(void);
uint8_t brx_ps2_last_mouse_byte(void);

#endif
