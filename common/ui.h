#pragma once

#include <furi.h>
#include <gui/canvas.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

typedef enum {
    Black,
    White,
    Inverse
} DrawMode;


uint32_t pixel_index(uint8_t x, uint8_t y);

void draw_rounded_box(Canvas *const canvas, int16_t x, int16_t y, uint8_t w, uint8_t h, DrawMode drawMode);

void draw_rounded_box_frame(Canvas *const canvas, int16_t x, int16_t y, uint8_t w, uint8_t h, DrawMode drawMode);

void invert_rectangle(Canvas *const canvas, int16_t x, int16_t y, uint8_t w, uint8_t h);

void invert_shape(Canvas *const canvas, uint8_t *data, int16_t x, int16_t y, uint8_t w, uint8_t h);

bool read_pixel(Canvas *const canvas, int16_t x, int16_t y);

void set_pixel(Canvas *const canvas, int16_t x, int16_t y, DrawMode draw_mode);

void draw_line(Canvas *const canvas, int16_t x1, int16_t y1, int16_t x2, int16_t y2, DrawMode draw_mode);

bool in_screen(int16_t x, int16_t y);