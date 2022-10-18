#pragma once

#include <gui/gui.h>
#include <furi.h>

typedef struct {
    void (*callback)(void *state);

    void (*render)(const void *state, Canvas *const canvas);

    void (*start)(void *state);

    void *next;
    uint32_t duration;
} QueueItem;

typedef struct {
    unsigned int start;
    QueueItem *current;
    bool running;
} QueueState;

void enqueue(QueueState *queue_state, void *app_state,
             void(*done)(void *state), void(*start)(void *state),
             void (*render)(const void *state, Canvas *const canvas), uint32_t duration);

void queue_clear(QueueState *queue_state);

void dequeue(QueueState *queue_state, void *app_state);

bool run_queue(QueueState *queue_state, void *app_state);

void render_queue(const QueueState *queue_state, const void *app_state, Canvas *const canvas);