#include <stdio.h>
#include "brx_window.h"

int main(void) {
    brx_window_manager_t manager;
    brx_window_manager_init(&manager);
    brx_window_t *window = brx_window_create(&manager, "CONSOLE", (brx_window_rect_t){100, 80, 320, 200});
    if (!window || manager.focused_id != window->id) return 1;
    if (brx_window_at(&manager, 120, 100) != window) return 2;
    if (brx_window_handle_mouse(&manager, (brx_mouse_event_t){120, 100, 1, 0, 0}) != 1) return 3;
    if (!window->dragging) return 4;
    if (brx_window_handle_mouse(&manager, (brx_mouse_event_t){220, 150, 1, 0, 0}) != 1 || window->rect.x != 200 || window->rect.y != 130) return 5;
    if (brx_window_close(&manager, window->id) != 0 || brx_window_find(&manager, window->id)) return 6;
    puts("BRX window manager tests: PASS");
    return 0;
}
