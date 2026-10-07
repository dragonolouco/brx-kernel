#ifndef BRX_TERMINAL_H
#define BRX_TERMINAL_H

#include <stdint.h>

#define BRX_TERMINAL_LINES 12
#define BRX_TERMINAL_COLUMNS 64
#define BRX_TERMINAL_INPUT 64

typedef struct {
    char lines[BRX_TERMINAL_LINES][BRX_TERMINAL_COLUMNS];
    uint64_t line_count;
    char input[BRX_TERMINAL_INPUT];
    uint64_t input_length;
} brx_terminal_t;

void brx_terminal_init(brx_terminal_t *terminal);
void brx_terminal_handle_key(brx_terminal_t *terminal, char key);
const char *brx_terminal_line(const brx_terminal_t *terminal, uint64_t index);
const char *brx_terminal_input(const brx_terminal_t *terminal);
uint64_t brx_terminal_line_count(const brx_terminal_t *terminal);
uint64_t brx_terminal_input_length(const brx_terminal_t *terminal);

#endif
