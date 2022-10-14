#pragma once

#include "defines.h"
#include "util.h"
#include <gui/gui.h>

void draw_player_scene(Canvas *const canvas, const GameState *game_state);

void draw_dealer_scene(Canvas *const canvas, const GameState *game_state);

void draw_message_scene(Canvas *const canvas, const GameState *game_state);

void draw_play_menu(Canvas *const canvas, const GameState *game_state);

void draw_score(Canvas *const canvas, bool top, uint8_t amount);

void draw_money(Canvas *const canvas, uint32_t score);

void popupFrame(Canvas *const canvas);