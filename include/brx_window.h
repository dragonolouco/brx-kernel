#ifndef BRX_WINDOW_H
#define BRX_WINDOW_H

#include <stdint.h>

#define BRX_WINDOW_MAX 16
#define BRX_WINDOW_TITLE_MAX 32

typedef enum {
    BRX_WINDOW_FREE = 0,
    BRX_WINDOW_OPEN,
    BRX_WINDOW_CLOSED
} brx_window_state_t;

typedef struct {
    uint64_t x;
    uint64_t y;
    uint64_t width;
    uint64_t height;
} brx_window_rect_t;

typedef struct {
    uint64_t x;
    uint64_t y;
    uint8_t left;
    uint8_t right;
    uint8_t middle;
} brx_mouse_event_t;

typedef struct {
    uint64_t id;
    brx_window_state_t state;
    brx_window_rect_t rect;
    char title[BRX_WINDOW_TITLE_MAX];
    uint8_t focused;
    uint8_t dragging;
    uint64_t drag_offset_x;
    uint64_t drag_offset_y;
} brx_window_t;

typedef struct {
    brx_window_t windows[BRX_WINDOW_MAX];
    uint64_t next_id;
    uint64_t focused_id;
} brx_window_manager_t;

void brx_window_manager_init(brx_window_manager_t *manager);
brx_window_t *brx_window_create(brx_window_manager_t *manager, const char *title, brx_window_rect_t rect);
brx_window_t *brx_window_find(brx_window_manager_t *manager, uint64_t id);
brx_window_t *brx_window_at(brx_window_manager_t *manager, uint64_t x, uint64_t y);
int brx_window_close(brx_window_manager_t *manager, uint64_t id);
int brx_window_handle_mouse(brx_window_manager_t *manager, brx_mouse_event_t event);

#endif
