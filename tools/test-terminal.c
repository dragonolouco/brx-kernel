#include <stdio.h>
#include <string.h>
#include "brx_terminal.h"
int main(void) {
    brx_terminal_t terminal;
    brx_terminal_init(&terminal);
    brx_terminal_handle_key(&terminal, 'h'); brx_terminal_handle_key(&terminal, 'e'); brx_terminal_handle_key(&terminal, 'l'); brx_terminal_handle_key(&terminal, 'p'); brx_terminal_handle_key(&terminal, '\n');
    if (!strstr(brx_terminal_line(&terminal, brx_terminal_line_count(&terminal) - 1), "COMMANDS")) return 1;
    brx_terminal_handle_key(&terminal, 'e'); brx_terminal_handle_key(&terminal, 'c'); brx_terminal_handle_key(&terminal, 'h'); brx_terminal_handle_key(&terminal, 'o'); brx_terminal_handle_key(&terminal, ' '); brx_terminal_handle_key(&terminal, 'o'); brx_terminal_handle_key(&terminal, 'k'); brx_terminal_handle_key(&terminal, '\n');
    if (strcmp(brx_terminal_line(&terminal, brx_terminal_line_count(&terminal) - 1), "ok")) return 2;
    brx_terminal_handle_key(&terminal, 'x'); brx_terminal_handle_key(&terminal, 'y'); brx_terminal_handle_key(&terminal, '\b');
    if (brx_terminal_input_length(&terminal) != 1) return 3;
    puts("BRX terminal tests: PASS"); return 0;
}
