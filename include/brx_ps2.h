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
int64_t brx_ps2_mouse_x(void);
int64_t brx_ps2_mouse_y(void);
uint8_t brx_ps2_mouse_buttons(void);
int brx_ps2_read_key(void);
void brx_ps2_poll(void);

#endif
