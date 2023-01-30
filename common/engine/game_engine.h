#pragma once

#include <input/input.h>
#include <furi.h>
#include <dolphin/dolphin.h>
#include <gui/view_port.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include "objects.h"
#include "util/list.h"

typedef enum {
    InputDown, InputUp, InputPress
} InputState;

typedef struct {
    InputKey inputKey;
    InputType inputType;
} LastInput;

typedef enum {
    EventTick, EventInput
} EventType;

typedef struct {
    EventType type;
    InputEvent inputEvent;
} AppEvent;

typedef struct {
    sprite_t *image;
    Vector position;
} RenderInfo;

typedef struct {
    RenderInfo render_list[64];
    uint8_t render_count;
} RenderQueue;

typedef struct {
    InputState inputState[7];
    LastInput lastInput;
    unsigned int last_tick;
    unsigned int tick_delta;
    FuriMessageQueue *event_queue;
    void *gameState;
    ValueMutex render_mutex;
    RenderQueue *renderQue;
    NotificationApp *notificationApp;
    ViewPort *viewPort;
    FuriTimer *timer;
    Gui *gui;
    uint8_t *prevFrame;
    bool loaded;
    Scene *scene;
    bool processing;
} EngineState;

typedef struct {
    const char *AppName;
    size_t stateSize;

    void (*initState)(void *state);

    bool alwaysOnScreen;
    uint32_t tickRate;
} SetupState;


int32_t setup_engine(SetupState state);

void start_loop();

void set_scene(Scene *s);

unsigned int delta_tick();

unsigned int frame_tick();

void exit_app();

