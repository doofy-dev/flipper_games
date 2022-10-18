#pragma once

#include <gui/gui.h>
#include <math.h>
#include <stdlib.h>
#include "dml.h"

#define CARD_HEIGHT 24
#define CARD_HALF_HEIGHT CARD_HEIGHT/2
#define CARD_WIDTH 18
#define CARD_HALF_WIDTH CARD_WIDTH/2
#define DECK_COUNT 6

//region types
typedef struct {
    uint8_t pip;
    uint8_t character;
} Card;

typedef struct {
    Card cards[52 * DECK_COUNT];
    int index;
} Deck;
//endregion

bool is_at_edge(uint8_t index);


Vector card_pos_at_index(uint8_t index);

bool get_pip_pixel(uint8_t pip, uint8_t x, uint8_t y);

void drawCardAt(int8_t pos_x, int8_t pos_y, uint8_t pip, uint8_t character, Canvas *const canvas);

void drawPlayerDeck(const Card *cards, uint8_t count, Canvas *const canvas);

void drawCardBackAt(int8_t pos_x, int8_t pos_y, Canvas *const canvas);

void generateDeck(Deck *deck_ptr);

void shuffleDeck(Deck *deck_ptr);

uint8_t hand_count(const Card *cards, uint8_t count);


void draw_card_animation(Card animatingCard, Vector from, Vector control, Vector to, float t, bool extra_margin,
                                Canvas *const canvas);