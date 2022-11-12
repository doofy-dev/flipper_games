#pragma once
#include <furi.h>
#include <input/input.h>
#include <gui/elements.h>
#include <flipper_format/flipper_format.h>
#include <flipper_format/flipper_format_i.h>
#include "common/card.h"
#include "common/queue.h"

#define APP_NAME "Solitaire"

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} AppEvent;

typedef enum {
    GameStateGameOver,
    GameStateStart,
    GameStatePlay,
    GameStateAnimate
} PlayState;

typedef struct {
    Deck deck;
    int deckIndex;
    bool has_side;
    Hand bottom_columns[7]; //13 items
    Hand top_cards[4]; //13 items

    InputKey input;

    bool started;
    bool processing;
    PlayState state;
    QueueState queue_state;
    unsigned int last_tick;
} GameState;