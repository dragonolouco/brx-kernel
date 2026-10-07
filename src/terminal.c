#include "brx_terminal.h"

static void clear_text(char *text, uint64_t size) { for (uint64_t i = 0; i < size; ++i) text[i] = 0; }
static int equal(const char *a, const char *b) { uint64_t i = 0; while (a[i] && b[i] && a[i] == b[i]) ++i; return a[i] == 0 && b[i] == 0; }
static void append_line(brx_terminal_t *terminal, const char *text) {
    if (terminal->line_count == BRX_TERMINAL_LINES) {
        for (uint64_t i = 1; i < BRX_TERMINAL_LINES; ++i) for (uint64_t j = 0; j < BRX_TERMINAL_COLUMNS; ++j) terminal->lines[i - 1][j] = terminal->lines[i][j];
        terminal->line_count--;
    }
    uint64_t i = 0;
    while (text && text[i] && i + 1 < BRX_TERMINAL_COLUMNS) { terminal->lines[terminal->line_count][i] = text[i]; ++i; }
    terminal->lines[terminal->line_count][i] = 0;
    terminal->line_count++;
}
static void command(brx_terminal_t *terminal) {
    append_line(terminal, terminal->input);
    if (equal(terminal->input, "help")) {
        append_line(terminal, "BRX COMMANDS: help status clear echo");
    } else if (equal(terminal->input, "status")) {
        append_line(terminal, "KERNEL ONLINE / FRAMEBUFFER ONLINE");
        append_line(terminal, "USER SPACE NOT AVAILABLE");
    } else if (equal(terminal->input, "clear")) {
        terminal->line_count = 0;
    } else if (terminal->input[0] == 'e' && terminal->input[1] == 'c' && terminal->input[2] == 'h' && terminal->input[3] == 'o' && terminal->input[4] == ' ') {
        append_line(terminal, terminal->input + 5);
    } else if (terminal->input[0]) {
        append_line(terminal, "COMMAND NOT IMPLEMENTED");
    }
    clear_text(terminal->input, BRX_TERMINAL_INPUT);
    terminal->input_length = 0;
}
void brx_terminal_init(brx_terminal_t *terminal) {
    clear_text((char *)terminal, sizeof(*terminal));
    append_line(terminal, "BRX TERMINAL READY");
    append_line(terminal, "TYPE HELP FOR COMMANDS");
}
void brx_terminal_handle_key(brx_terminal_t *terminal, char key) {
    if (key == '\n' || key == '\r') { command(terminal); return; }
    if (key == '\b') { if (terminal->input_length) terminal->input[--terminal->input_length] = 0; return; }
    if (key >= 32 && key <= 126 && terminal->input_length + 1 < BRX_TERMINAL_INPUT) {
        terminal->input[terminal->input_length++] = key;
        terminal->input[terminal->input_length] = 0;
    }
}
const char *brx_terminal_line(const brx_terminal_t *terminal, uint64_t index) { return index < terminal->line_count ? terminal->lines[index] : ""; }
const char *brx_terminal_input(const brx_terminal_t *terminal) { return terminal->input; }
uint64_t brx_terminal_line_count(const brx_terminal_t *terminal) { return terminal->line_count; }
uint64_t brx_terminal_input_length(const brx_terminal_t *terminal) { return terminal->input_length; }
