#pragma once
#include <furi.h>
#include <input/input.h>
#include <gui/elements.h>
#include <flipper_format/flipper_format.h>
#include <flipper_format/flipper_format_i.h>
#include "common/queue.h"

#define APP_NAME "Pinball"

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

typedef struct Vector Vector;

typedef struct {
    Vector *position;
    Vector *velocity;
} Ball;

typedef struct {
    InputKey input;

    bool started;
    bool processing;
    bool longPress;
    PlayState state;
    unsigned int last_tick;
    uint8_t *buffer;
    Ball ball;

} GameState;