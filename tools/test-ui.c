#include <stdint.h>
#include <stdio.h>
#include "brx_ui.h"

int main(void) {
    uint32_t pixels[64 * 64] = {0};
    brx_framebuffer_t framebuffer = { pixels, 64, 64, 64 * 4, 32, 1, 8, 16, 8, 8, 8, 0, {0}, 0, 0, 0, 0 };
    brx_ui_surface_t surface;
    if (!brx_ui_init(&surface, &framebuffer)) return 1;
    brx_ui_clear(&surface, 0x102030);
    brx_ui_rect(&surface, 4, 4, 12, 12, 0xff0000);
    brx_ui_text(&surface, 2, 24, "BRX", 0xffffff, 1);
    brx_ui_cursor(&surface, 40, 40, 0xffffff);
    brx_window_manager_t manager;
    brx_window_manager_init(&manager);
    if (!brx_window_create(&manager, "TEST WINDOW", (brx_window_rect_t){8, 8, 64, 64})) return 3;
    brx_ui_render_windows(&surface, &manager);
    if (pixels[0] != 0x102030 || pixels[4 + 4 * 64] != 0xff0000 || pixels[40 + 40 * 64] == 0) return 2;
    puts("BRX graphical interface tests: PASS");
    return 0;
}
