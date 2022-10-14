#include "util.h"

static QueueItem *afterDelay;

float lerp(float v0, float v1, float t) {
    if(t>1) return v1;
    return (1 - t) * v0 + t * v1;
}

Vector lerp_2d(Vector start, Vector end, float t) {
    return (Vector){
            lerp(start.x, end.x, t),
            lerp(start.y, end.y, t),
    };
}

Vector quadratic_2d(Vector start, Vector control, Vector end, float t) {
    return lerp_2d(
            lerp_2d(start, control, t),
            lerp_2d(control, end, t),
            t
    );
}


void queue(GameState *game_state,
           void (*callback)(GameState *game_state),
           void (*start)(GameState *game_state),
           void (*processing)(const GameState *gameState, Canvas *const canvas)
) {
    if (afterDelay == NULL) {
        game_state->animationStart = game_state->last_tick;
        afterDelay = malloc(sizeof(QueueItem));
        afterDelay->callback = callback;
        afterDelay->processing = processing;
        afterDelay->start = start;
        afterDelay->next = NULL;
    } else {
        QueueItem *next = afterDelay;
        while (next->next != NULL) { next = (QueueItem *) (next->next); }
        next->next = malloc(sizeof(QueueItem));
        ((QueueItem *) next->next)->callback = callback;
        ((QueueItem *) next->next)->processing = processing;
        ((QueueItem *) next->next)->start = start;
        ((QueueItem *) next->next)->next = NULL;
    }
}

void queue_clear() {
    while (afterDelay != NULL) {
        QueueItem *f = afterDelay;
        free(f);
        afterDelay = f->next;
    }
}

void dequeue(GameState *game_state) {
    ((QueueItem *) afterDelay)->callback(game_state);
    QueueItem *f = afterDelay;
    afterDelay = (QueueItem *) afterDelay->next;
    free(f);
    if (afterDelay != NULL && afterDelay->start != NULL)afterDelay->start(game_state);
    game_state->animationStart = game_state->last_tick;
}

void animateQueue(const GameState *game_state, Canvas *const canvas) {
    if (afterDelay != NULL && ((QueueItem *) afterDelay)->processing != NULL) {
        ((QueueItem *) afterDelay)->processing(game_state, canvas);
    }
}

bool run_queue(GameState *game_state) {
    if (afterDelay != NULL) {
        game_state->animating = true;
        if ((game_state->last_tick - game_state->animationStart) > ANIMATION_TIME) {
            dequeue(game_state);
        }
        return true;
    }
    game_state->animating = false;
    return false;
}
