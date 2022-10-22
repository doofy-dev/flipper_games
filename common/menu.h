#pragma once

#include <furi.h>
#include <gui/gui.h>

typedef struct {
    const char *name;
    bool enabled;

    void (*callback)(void *state);
} MenuItem;

typedef struct {
    MenuItem *items;
    uint8_t menu_count;
    uint8_t current_menu;
    uint8_t menu_width;
    bool enabled;
} Menu;


void free_menu(Menu *menu);

void add_menu(Menu *menu, const char *name, void (*callback)(void *));

void set_menu_state(Menu *menu, uint8_t index, bool state);

void move_menu(Menu *menu, int8_t direction);

void activate_menu(Menu *menu, void *state);

void render_menu(Menu *menu, Canvas* canvas, uint8_t pos_x, uint8_t pos_y);