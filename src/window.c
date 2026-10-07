#include "brx_window.h"

static void copy_title(char *destination, const char *source) {
    uint64_t i = 0;
    if (source) while (source[i] && i + 1 < BRX_WINDOW_TITLE_MAX) { destination[i] = source[i]; ++i; }
    destination[i] = 0;
}

static int inside(const brx_window_t *window, uint64_t x, uint64_t y) {
    return window->state == BRX_WINDOW_OPEN && x >= window->rect.x && y >= window->rect.y && x < window->rect.x + window->rect.width && y < window->rect.y + window->rect.height;
}

void brx_window_manager_init(brx_window_manager_t *manager) {
    manager->next_id = 1;
    manager->focused_id = 0;
    for (uint64_t i = 0; i < BRX_WINDOW_MAX; ++i) manager->windows[i].state = BRX_WINDOW_FREE;
}

brx_window_t *brx_window_create(brx_window_manager_t *manager, const char *title, brx_window_rect_t rect) {
    if (!manager || rect.width < 64 || rect.height < 40) return 0;
    for (uint64_t i = 0; i < BRX_WINDOW_MAX; ++i) {
        brx_window_t *window = &manager->windows[i];
        if (window->state != BRX_WINDOW_FREE && window->state != BRX_WINDOW_CLOSED) continue;
        window->id = manager->next_id++;
        window->state = BRX_WINDOW_OPEN;
        window->rect = rect;
        window->focused = 1;
        window->dragging = 0;
        copy_title(window->title, title);
        manager->focused_id = window->id;
        for (uint64_t j = 0; j < BRX_WINDOW_MAX; ++j) if (&manager->windows[j] != window) manager->windows[j].focused = 0;
        return window;
    }
    return 0;
}

brx_window_t *brx_window_find(brx_window_manager_t *manager, uint64_t id) {
    if (!manager || id == 0) return 0;
    for (uint64_t i = 0; i < BRX_WINDOW_MAX; ++i) if (manager->windows[i].id == id && manager->windows[i].state == BRX_WINDOW_OPEN) return &manager->windows[i];
    return 0;
}

brx_window_t *brx_window_at(brx_window_manager_t *manager, uint64_t x, uint64_t y) {
    if (!manager) return 0;
    for (uint64_t i = BRX_WINDOW_MAX; i > 0; --i) if (inside(&manager->windows[i - 1], x, y)) return &manager->windows[i - 1];
    return 0;
}

int brx_window_close(brx_window_manager_t *manager, uint64_t id) {
    brx_window_t *window = brx_window_find(manager, id);
    if (!window) return -1;
    window->state = BRX_WINDOW_CLOSED;
    window->focused = 0;
    window->dragging = 0;
    if (manager->focused_id == id) manager->focused_id = 0;
    return 0;
}

int brx_window_handle_mouse(brx_window_manager_t *manager, brx_mouse_event_t event) {
    brx_window_t *window = brx_window_at(manager, event.x, event.y);
    if (event.left && window) {
        if (manager->focused_id != window->id) {
            manager->focused_id = window->id;
            for (uint64_t i = 0; i < BRX_WINDOW_MAX; ++i) manager->windows[i].focused = manager->windows[i].id == window->id;
        }
        if (event.y < window->rect.y + 32) {
            window->dragging = 1;
            window->drag_offset_x = event.x - window->rect.x;
            window->drag_offset_y = event.y - window->rect.y;
        }
    } else if (!event.left && window) window->dragging = 0;
    if (window && window->dragging && event.left) {
        window->rect.x = event.x > window->drag_offset_x ? event.x - window->drag_offset_x : 0;
        window->rect.y = event.y > window->drag_offset_y ? event.y - window->drag_offset_y : 0;
        return 1;
    }
    return window != 0;
}
