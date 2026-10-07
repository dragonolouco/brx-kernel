#ifndef BRX_INTERRUPTS_H
#define BRX_INTERRUPTS_H

#include <stdint.h>

void brx_interrupts_init(void);
void brx_timer_init(uint32_t frequency_hz);
void brx_interrupts_enable(void);
uint64_t brx_timer_ticks(void);

#endif
