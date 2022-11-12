#include "ui.h"
#include <gui/canvas_i.h>
#include <u8g2_glue.h>
#include <gui/icon_animation_i.h>
#include <gui/icon.h>
#include <gui/icon_i.h>
#include <furi_hal.h>

TileMap *tileMap;
uint8_t tileMapCount = 0;

void ui_cleanup() {
    for (uint8_t i = 0; i < tileMapCount; i++) {
        free(tileMap[i].data);
    }
    free(tileMap);
}

void add_new_tilemap(uint8_t *data, unsigned long iconId) {
    TileMap *old = tileMap;
    tileMapCount++;
    tileMap = malloc(sizeof(TileMap) * tileMapCount);
    if (tileMapCount > 1) {
        for (uint8_t i = 0; i < tileMapCount; i++)
            tileMap[i] = old[i];
    }
    tileMap[tileMapCount - 1] = (TileMap) {data, iconId};
}


uint8_t *get_tilemap(unsigned long icon_id) {
    for (uint8_t i = 0; i < tileMapCount; i++) {
        if (tileMap[i].iconId == icon_id)
            return tileMap[i].data;
    }

    return NULL;
}

uint32_t pixel_index(uint8_t x, uint8_t y) {
    return y * SCREEN_WIDTH + x;
}

bool in_screen(int16_t x, int16_t y) {
    return x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT;
}

unsigned flipBit(uint8_t x, uint8_t bit) {
    return x ^ (1 << bit);
}

unsigned setBit(uint8_t x, uint8_t bit) {
    return x | (1 << bit);
}

unsigned unsetBit(uint8_t x, uint8_t bit) {
    return x & ~(1 << bit);
}

bool read_pixel(Canvas *const canvas, int16_t x, int16_t y) {
    if (in_screen(x, y)) {
        uint8_t current_bit = (y % 8);
        uint8_t current_row = ((y - current_bit) / 8);
        uint32_t i = pixel_index(x, current_row);
        uint8_t current_value = canvas->fb.tile_buf_ptr[i];
        return current_value & (1 << current_bit);
    }
    return false;
}

void set_pixel(Canvas *const canvas, int16_t x, int16_t y, DrawMode draw_mode) {
    if (in_screen(x, y)) {
        uint8_t current_bit = (y % 8);
        uint8_t current_row = ((y - current_bit) / 8);
        uint32_t i = pixel_index(x, current_row);

        uint8_t current_value = canvas->fb.tile_buf_ptr[i];
        if (draw_mode == Inverse) {
            canvas->fb.tile_buf_ptr[i] = flipBit(current_value, current_bit);
        } else {
            if (draw_mode == White) {
                canvas->fb.tile_buf_ptr[i] = unsetBit(current_value, current_bit);
            } else {
                canvas->fb.tile_buf_ptr[i] = setBit(current_value, current_bit);
            }
        }
    }
}

void draw_line(Canvas *const canvas, int16_t x1, int16_t y1, int16_t x2, int16_t y2, DrawMode draw_mode) {
    for (int16_t x = x1; x <= x2; x++) {
        for (int16_t y = y1; y <= y2; y++) {
            set_pixel(canvas, x, y, draw_mode);
        }
    }
}

void draw_rounded_box_frame(Canvas *const canvas, int16_t x, int16_t y, uint8_t w, uint8_t h, DrawMode draw_mode) {
    int16_t xMinCorner = x + 1;
    int16_t xMax = x + w - 1;
    int16_t xMaxCorner = x + w - 2;
    int16_t yMinCorner = y + 1;
    int16_t yMax = y + h - 1;
    int16_t yMaxCorner = y + h - 2;
    draw_line(canvas, xMinCorner, y, xMaxCorner, y, draw_mode);
    draw_line(canvas, xMinCorner, yMax, xMaxCorner, yMax, draw_mode);
    draw_line(canvas, x, yMinCorner, x, yMaxCorner, draw_mode);
    draw_line(canvas, xMax, yMinCorner, xMax, yMaxCorner, draw_mode);
}

void draw_rounded_box(Canvas *const canvas, int16_t x, int16_t y, uint8_t w, uint8_t h, DrawMode drawMode) {
    for (int16_t o = 0; o < w; o++) {
        for (int16_t p = 0; p < h; p++) {
            int16_t curr_x = x + o;
            int16_t curr_y = y + p;
            if (in_screen(curr_x, curr_y)) {
                if (
                        (o == 0 && p == 0) ||
                        (o == 0 && p == h - 1) ||
                        (o == w - 1 && p == 0) ||
                        (o == w - 1 && p == h - 1)
                        )
                    continue;
                set_pixel(canvas, curr_x, curr_y, drawMode);
            }
        }
    }
}

void invert_shape(Canvas *const canvas, uint8_t *data, int16_t x, int16_t y, uint8_t w, uint8_t h) {
    draw_pixels(canvas, data, x, y, w, h, Inverse);
}

void draw_pixels(Canvas *const canvas, uint8_t *data, int16_t x, int16_t y, uint8_t w, uint8_t h, DrawMode drawMode) {
    for (int8_t o = 0; o < w; o++) {
        for (int8_t p = 0; p < h; p++) {
            if (in_screen(o + x, p + y) && data[p * w + o] == 1)
                set_pixel(canvas, o + x, p + y, drawMode);
        }
    }
}

void draw_rectangle(Canvas *const canvas, int16_t x, int16_t y, uint8_t w, uint8_t h, DrawMode drawMode) {
    for (int8_t o = 0; o < w; o++) {
        for (int8_t p = 0; p < h; p++) {
            if (in_screen(o + x, p + y)) {
                set_pixel(canvas, o + x, p + y, drawMode);
            }
        }
    }
}

void invert_rectangle(Canvas *const canvas, int16_t x, int16_t y, uint8_t w, uint8_t h) {
    draw_rectangle(canvas, x, y, w, h, Inverse);
}

uint8_t *image_data(Canvas *const canvas, const Icon *icon) {
    uint8_t *data = malloc(sizeof(uint8_t) * 8 * 126);
    uint8_t *screen = canvas->fb.tile_buf_ptr;
    canvas->fb.tile_buf_ptr = data;
    canvas_draw_icon(canvas, 0, 0, icon);
    canvas->fb.tile_buf_ptr = screen;
    return data;
}

uint8_t *getOrAddIconData(Canvas *const canvas, const Icon *icon) {
    uint8_t *icon_data = get_tilemap((unsigned long) icon);
    if (icon_data == NULL) {
        icon_data = image_data(canvas, icon);
        add_new_tilemap(icon_data, (unsigned long) icon);
    }
    return icon_data;
}

void draw_icon_clip(Canvas *const canvas, const Icon *icon, int16_t x, int16_t y, uint8_t left, uint8_t top, uint8_t w,
                    uint8_t h, DrawMode drawMode) {

    uint8_t *icon_data = getOrAddIconData(canvas, icon);

    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            uint8_t cy = top + j;

            uint8_t current_bit = (cy % 8);
            uint8_t current_row = ((cy - current_bit) / 8);
            uint8_t cx = left + i;
            uint8_t current_value = icon_data[current_row * SCREEN_WIDTH + cx];
            if (current_value & (1 << current_bit))
                set_pixel(canvas, x + i, y + j, drawMode);
        }
    }
}

void draw_icon_clip_flipped(Canvas *const canvas, const Icon *icon, int16_t x, int16_t y, uint8_t left, uint8_t top,
                            uint8_t w,
                            uint8_t h, DrawMode drawMode) {

    uint8_t *icon_data = getOrAddIconData(canvas, icon);

    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            uint8_t cy = top + j;
            uint8_t current_bit = (cy % 8);
            uint8_t current_row = ((cy - current_bit) / 8);
            uint8_t cx = left + i;
            uint8_t current_value = icon_data[current_row * SCREEN_WIDTH + cx];
            if (current_value & (1 << current_bit))
                set_pixel(canvas, x + w-i-1, y + h-j-1, drawMode);
        }
    }
}