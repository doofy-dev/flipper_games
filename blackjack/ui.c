#include <math.h>
#include <notification/notification_messages.h>

#include "ui.h"
#include "card.h"
#include "util.h"

const char MoneyMul[4] = {
        'K', 'B', 'T', 'S'
};

void draw_player_scene(Canvas *const canvas, const GameState *game_state) {
    int max_card = game_state->player_card_count;

    if (max_card > 0)
        drawPlayerDeck((game_state->player_cards), max_card, canvas);

    if(game_state->dealer_card_count>0)
        drawCardBackAt(13, 5, canvas);

    max_card = game_state->dealer_card_count;
    if (max_card > 1) {
        drawCardAt(2, 2, game_state->dealer_cards[1].pip, game_state->dealer_cards[1].character, Normal,
                   canvas);
    }
}

void draw_dealer_scene(Canvas *const canvas, const GameState *game_state) {
    uint8_t max_card = game_state->dealer_card_count;
    drawPlayerDeck((game_state->dealer_cards), max_card, canvas);
}

void popup_frame(Canvas *const canvas) {
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, 32, 15, 66, 13);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_frame(canvas, 32, 15, 66, 13);
    canvas_set_font(canvas, FontSecondary);
}


void draw_play_menu(Canvas *const canvas, const GameState *game_state) {
    const char *menus[3] = {"Double", "Hit", "Stay"};
    for (uint8_t m = 0; m < 3; m++) {
        if (m == 0 && (game_state->doubled || game_state->player_score < ROUND_PRICE)) continue;
        int y = m * 13 + 25;
        canvas_set_color(canvas, ColorBlack);

        if (game_state->selectedMenu == m) {
            canvas_set_color(canvas, ColorBlack);
            canvas_draw_box(canvas, 1, y, 31, 12);
        } else {
            canvas_set_color(canvas, ColorWhite);
            canvas_draw_box(canvas, 1, y, 31, 12);
            canvas_set_color(canvas, ColorBlack);
            canvas_draw_frame(canvas, 1, y, 31, 12);
        }

        if (game_state->selectedMenu == m)
            canvas_set_color(canvas, ColorWhite);
        else
            canvas_set_color(canvas, ColorBlack);
        canvas_draw_str_aligned(canvas, 16, y + 6, AlignCenter, AlignCenter, menus[m]);
    }
}

void draw_screen(Canvas *const canvas, const bool* points){
    for(uint8_t x=0;x<128;x++){
        for(uint8_t y=0;y<64;y++){
            if(points[y*128 + x])
                canvas_draw_dot(canvas, x, y);
        }
    }
}

void draw_score(Canvas *const canvas, bool top, uint8_t amount) {
    char drawChar[20];
    snprintf(drawChar, sizeof(drawChar), "Player score: %i", amount);
    if (top)
        canvas_draw_str_aligned(canvas, 64, 2, AlignCenter, AlignTop, drawChar);
    else
        canvas_draw_str_aligned(canvas, 64, 62, AlignCenter, AlignBottom, drawChar);
}

void draw_money(Canvas *const canvas, uint32_t score) {
    canvas_set_font(canvas, FontSecondary);
    char drawChar[10];
    uint32_t currAmount = score;
    if (currAmount < 1000) {
        snprintf(drawChar, sizeof(drawChar), "$%lu", currAmount);
    } else {
        char c = 'K';
        for (uint8_t i = 0; i < 4; i++) {
            currAmount = currAmount / 1000;
            if (currAmount < 1000) {
                c = MoneyMul[i];
                break;
            }
        }

        snprintf(drawChar, sizeof(drawChar), "$%lu %c", currAmount, c);
    }
    canvas_draw_str_aligned(canvas, 126, 2, AlignRight, AlignTop, drawChar);
}