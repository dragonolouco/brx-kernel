#include "brx_ui.h"

static uint32_t pack_color(const brx_ui_surface_t *surface, uint32_t color) {
    uint32_t red = (color >> 16) & 0xff;
    uint32_t green = (color >> 8) & 0xff;
    uint32_t blue = color & 0xff;
    uint32_t packed = 0;
    if (surface->red_mask_size) packed |= (red >> (8 - surface->red_mask_size)) << surface->red_mask_shift;
    if (surface->green_mask_size) packed |= (green >> (8 - surface->green_mask_size)) << surface->green_mask_shift;
    if (surface->blue_mask_size) packed |= (blue >> (8 - surface->blue_mask_size)) << surface->blue_mask_shift;
    return packed;
}

static int inside(const brx_ui_surface_t *surface, uint64_t x, uint64_t y) {
    return surface->ready && x < surface->width && y < surface->height;
}

static void pixel(brx_ui_surface_t *surface, uint64_t x, uint64_t y, uint32_t color) {
    if (!inside(surface, x, y)) return;
    volatile uint32_t *destination = (volatile uint32_t *)(surface->address + y * surface->pitch + x * (surface->bpp / 8));
    *destination = pack_color(surface, color);
}

int brx_ui_init(brx_ui_surface_t *surface, const brx_framebuffer_t *framebuffer) {
    if (!surface || !framebuffer || !framebuffer->address || framebuffer->width == 0 || framebuffer->height == 0 || framebuffer->pitch == 0 || framebuffer->bpp < 24) return 0;
    surface->address = (volatile uint8_t *)framebuffer->address;
    surface->width = framebuffer->width;
    surface->height = framebuffer->height;
    surface->pitch = framebuffer->pitch;
    surface->bpp = framebuffer->bpp;
    surface->red_mask_size = framebuffer->red_mask_size;
    surface->red_mask_shift = framebuffer->red_mask_shift;
    surface->green_mask_size = framebuffer->green_mask_size;
    surface->green_mask_shift = framebuffer->green_mask_shift;
    surface->blue_mask_size = framebuffer->blue_mask_size;
    surface->blue_mask_shift = framebuffer->blue_mask_shift;
    surface->ready = 1;
    return 1;
}

void brx_ui_clear(brx_ui_surface_t *surface, uint32_t color) {
    if (!surface || !surface->ready) return;
    brx_ui_rect(surface, 0, 0, surface->width, surface->height, color);
}

void brx_ui_rect(brx_ui_surface_t *surface, uint64_t x, uint64_t y, uint64_t width, uint64_t height, uint32_t color) {
    if (!surface || !surface->ready) return;
    uint64_t max_x = x + width > surface->width ? surface->width : x + width;
    uint64_t max_y = y + height > surface->height ? surface->height : y + height;
    for (uint64_t row = y; row < max_y; ++row) for (uint64_t column = x; column < max_x; ++column) pixel(surface, column, row, color);
}

static const uint8_t *glyph(char character) {
    static const uint8_t glyphs[37][7] = {
        {0x1e,0x21,0x21,0x3f,0x21,0x21,0x21},{0x3e,0x21,0x21,0x3e,0x21,0x21,0x3e},{0x1e,0x21,0x20,0x20,0x20,0x21,0x1e},{0x3e,0x21,0x21,0x21,0x21,0x21,0x3e},{0x3f,0x20,0x20,0x3e,0x20,0x20,0x3f},{0x3f,0x20,0x20,0x3e,0x20,0x20,0x20},{0x1e,0x21,0x20,0x27,0x21,0x21,0x1e},{0x21,0x21,0x21,0x3f,0x21,0x21,0x21},{0x3f,0x0c,0x0c,0x0c,0x0c,0x0c,0x3f},{0x07,0x02,0x02,0x02,0x22,0x22,0x1c},{0x21,0x22,0x24,0x38,0x24,0x22,0x21},{0x20,0x20,0x20,0x20,0x20,0x20,0x3f},{0x21,0x33,0x2d,0x21,0x21,0x21,0x21},{0x21,0x31,0x29,0x25,0x23,0x21,0x21},{0x1e,0x21,0x21,0x21,0x21,0x21,0x1e},{0x3e,0x21,0x21,0x3e,0x20,0x20,0x20},{0x1e,0x21,0x21,0x21,0x25,0x22,0x1d},{0x3e,0x21,0x21,0x3e,0x24,0x22,0x21},{0x1f,0x20,0x20,0x1e,0x01,0x01,0x3e},{0x3f,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c},{0x21,0x21,0x21,0x21,0x21,0x21,0x1e},{0x21,0x21,0x21,0x21,0x21,0x12,0x0c},{0x21,0x21,0x21,0x21,0x2d,0x33,0x21},{0x21,0x21,0x12,0x0c,0x12,0x21,0x21},{0x21,0x21,0x12,0x0c,0x0c,0x0c,0x0c},{0x3f,0x01,0x02,0x0c,0x10,0x20,0x3f},
        {0x1e,0x21,0x23,0x25,0x29,0x31,0x1e},{0x0c,0x14,0x24,0x04,0x04,0x04,0x3f},{0x1e,0x21,0x01,0x06,0x18,0x20,0x3f},{0x3e,0x01,0x01,0x1e,0x01,0x01,0x3e},{0x02,0x06,0x0a,0x12,0x3f,0x02,0x02},{0x3f,0x20,0x20,0x3e,0x01,0x01,0x3e},{0x1e,0x21,0x20,0x3e,0x21,0x21,0x1e},{0x3f,0x01,0x02,0x0c,0x10,0x10,0x10},{0x1e,0x21,0x21,0x1e,0x21,0x21,0x1e},{0x1e,0x21,0x21,0x1f,0x01,0x21,0x1e},{0x00,0x00,0x00,0x00,0x00,0x00,0x00}
    };
    if (character >= 'A' && character <= 'Z') return glyphs[character - 'A'];
    if (character >= '0' && character <= '9') return glyphs[26 + character - '0'];
    return glyphs[36];
}

void brx_ui_text(brx_ui_surface_t *surface, uint64_t x, uint64_t y, const char *text, uint32_t color, uint64_t scale) {
    if (!surface || !surface->ready || !text || scale == 0) return;
    uint64_t origin = x;
    while (*text) {
        if (*text == '\n') { y += 9 * scale; x = origin; ++text; continue; }
        const uint8_t *bitmap = glyph(*text++);
        for (uint64_t row = 0; row < 7; ++row) for (uint64_t column = 0; column < 6; ++column) if (bitmap[row] & (1 << (5 - column))) brx_ui_rect(surface, x + column * scale, y + row * scale, scale, scale, color);
        x += 7 * scale;
    }
}

void brx_ui_cursor(brx_ui_surface_t *surface, uint64_t x, uint64_t y, uint32_t color) {
    if (!surface || !surface->ready) return;
    for (uint64_t i = 0; i < 14; ++i) { pixel(surface, x + i / 2, y + i, color); if (i < 8) pixel(surface, x + i, y + i, color); }
}

static void window(brx_ui_surface_t *surface, uint64_t x, uint64_t y, uint64_t width, uint64_t height, const char *title) {
    brx_ui_rect(surface, x + 4, y + 6, width, height, 0x070b14);
    brx_ui_rect(surface, x, y, width, height, 0x202c43);
    brx_ui_rect(surface, x, y, width, 34, 0x2d3b58);
    brx_ui_rect(surface, x + width - 76, y + 9, 10, 10, 0xf6c453);
    brx_ui_rect(surface, x + width - 52, y + 9, 10, 10, 0x68d391);
    brx_ui_rect(surface, x + width - 28, y + 9, 10, 10, 0xf06c75);
    brx_ui_text(surface, x + 16, y + 10, title, 0xf2f6ff, 2);
}

void brx_ui_desktop(brx_ui_surface_t *surface) {
    if (!surface || !surface->ready) return;
    uint64_t width = surface->width;
    uint64_t height = surface->height;
    brx_ui_clear(surface, 0x0d1220);
    brx_ui_rect(surface, 0, 0, width, 42, 0x172238);
    brx_ui_text(surface, 22, 12, "BRX", 0x7eb6ff, 2);
    brx_ui_text(surface, width > 240 ? width - 210 : 20, 12, "SYSTEM READY", 0x68d391, 2);

    /* Desktop icons. */
    brx_ui_rect(surface, 34, 76, 48, 48, 0x2f81f7);
    brx_ui_rect(surface, 42, 84, 32, 24, 0x9fc7ff);
    brx_ui_text(surface, 28, 134, "FILES", 0xf2f6ff, 1);
    brx_ui_rect(surface, 34, 178, 48, 48, 0x68d391);
    brx_ui_rect(surface, 44, 190, 28, 24, 0x172238);
    brx_ui_text(surface, 24, 234, "TERMINAL", 0xf2f6ff, 1);
    brx_ui_rect(surface, 34, 278, 48, 48, 0xf6c453);
    brx_ui_rect(surface, 45, 289, 26, 26, 0x172238);
    brx_ui_text(surface, 24, 334, "SETTINGS", 0xf2f6ff, 1);

    /* First real window surface. */
    uint64_t window_width = width > 820 ? 700 : width > 520 ? width - 180 : width - 120;
    uint64_t window_height = height > 560 ? 360 : height - 170;
    window(surface, 150, 90, window_width, window_height, "TERMINAL");
    brx_ui_rect(surface, 170, 144, window_width - 40, window_height - 64, 0x0a0f19);
    brx_ui_text(surface, 190, 170, "BRX TERMINAL", 0x68d391, 2);
    brx_ui_text(surface, 190, 208, "WELCOME TO THE BRX SYSTEM", 0xf2f6ff, 1);
    brx_ui_text(surface, 190, 238, "TYPE HELP TO LIST COMMANDS", 0xb9c8df, 1);
    brx_ui_text(surface, 190, 280, "BRX", 0x7eb6ff, 1);
    brx_ui_rect(surface, 224, 280, 9, 15, 0xf2f6ff);

    /* Bottom dock and status area. */
    uint64_t dock_width = width > 760 ? 520 : width - 80;
    uint64_t dock_x = (width - dock_width) / 2;
    uint64_t dock_y = height > 80 ? height - 62 : 0;
    brx_ui_rect(surface, dock_x + 4, dock_y + 5, dock_width, 48, 0x070b14);
    brx_ui_rect(surface, dock_x, dock_y, dock_width, 48, 0x202c43);
    for (uint64_t i = 0; i < 5; ++i) brx_ui_rect(surface, dock_x + 24 + i * 72, dock_y + 12, 24, 24, i == 1 ? 0x68d391 : 0x7eb6ff);
    brx_ui_text(surface, dock_x + dock_width - 110, dock_y + 18, "BRX", 0xf2f6ff, 1);
}
