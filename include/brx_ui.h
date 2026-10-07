#ifndef BRX_UI_H
#define BRX_UI_H

#include <stdint.h>
#include "brx_boot.h"
#include "brx_window.h"
#include "brx_terminal.h"

typedef struct {
    volatile uint8_t *address;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    uint16_t bpp;
    uint8_t red_mask_size;
    uint8_t red_mask_shift;
    uint8_t green_mask_size;
    uint8_t green_mask_shift;
    uint8_t blue_mask_size;
    uint8_t blue_mask_shift;
    uint8_t ready;
} brx_ui_surface_t;

int brx_ui_init(brx_ui_surface_t *surface, const brx_framebuffer_t *framebuffer);
void brx_ui_clear(brx_ui_surface_t *surface, uint32_t color);
void brx_ui_rect(brx_ui_surface_t *surface, uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint32_t color);
void brx_ui_text(brx_ui_surface_t *surface, uint64_t x, uint64_t y, const char *text, uint32_t color, uint64_t scale);
void brx_ui_cursor(brx_ui_surface_t *surface, uint64_t x, uint64_t y, uint32_t color);
void brx_ui_desktop(brx_ui_surface_t *surface);
void brx_ui_render_windows(brx_ui_surface_t *surface, const brx_window_manager_t *manager);
void brx_ui_render_terminal(brx_ui_surface_t *surface, const brx_window_t *window, const brx_terminal_t *terminal);

#endif
