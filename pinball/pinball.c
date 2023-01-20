#include <stdlib.h>
#include <dolphin/dolphin.h>
#include <furi.h>
#include <gui/canvas_i.h>
#include "defines.h"
#include "common/ui.h"
#include "pinball_icons.h"
#include <notification/notification.h>
#include <notification/notification_messages.h>

void init(GameState *game_state);

const NotificationSequence sequence_fail = {
        &message_vibro_on,
        &message_note_c4,
        &message_delay_10,
        &message_vibro_off,
        &message_sound_off,
        &message_delay_10,

        &message_vibro_on,
        &message_note_a3,
        &message_delay_10,
        &message_vibro_off,
        &message_sound_off,
        NULL,
};

static void draw_scene(Canvas *const canvas, const GameState *game_state) {
    UNUSED(canvas);
    UNUSED(game_state);
}

static void render_callback(Canvas *const canvas, void *ctx) {
    const GameState *game_state = acquire_mutex((ValueMutex *) ctx, 25);
    if (game_state == NULL) {
        return;
    }

    UNUSED(canvas);

    release_mutex((ValueMutex *) ctx, game_state);

}

void tick(GameState *game_state, NotificationApp *notification) {
    game_state->last_tick = furi_get_tick();

    if (game_state->state != GameStatePlay && game_state->state != GameStateAnimate) return;

}

void init(GameState *game_state) {
    game_state->state = GameStatePlay;

}

void init_start(GameState *game_state) {
    game_state->input = InputKeyMAX;
}


static void input_callback(InputEvent *input_event, FuriMessageQueue *event_queue) {
    furi_assert(event_queue);
    AppEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void update_timer_callback(FuriMessageQueue *event_queue) {
    furi_assert(event_queue);
    AppEvent event = {.type = EventTypeTick};
    furi_message_queue_put(event_queue, &event, 0);
}

int32_t solitaire_app(void *p) {
    UNUSED(p);
    int32_t return_code = 0;
    FuriMessageQueue *event_queue = furi_message_queue_alloc(8, sizeof(AppEvent));
    GameState *game_state = malloc(sizeof(GameState));
    init_start(game_state);

    game_state->state = GameStateStart;

    game_state->processing = true;
    ValueMutex state_mutex;
    if (!init_mutex(&state_mutex, game_state, sizeof(GameState))) {
        FURI_LOG_E(APP_NAME, "cannot create mutex\r\n");
        return_code = 255;
        goto free_and_exit;
    }
    NotificationApp *notification = furi_record_open(RECORD_NOTIFICATION);

    notification_message_block(notification, &sequence_display_backlight_enforce_on);

    ViewPort *view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, &state_mutex);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    FuriTimer *timer =
            furi_timer_alloc(update_timer_callback, FuriTimerTypePeriodic, event_queue);
    furi_timer_start(timer, furi_kernel_get_tick_frequency() / 30);

    Gui *gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    AppEvent event;
    for (bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 150);
        GameState *localstate = (GameState *) acquire_mutex_block(&state_mutex);
        bool hadChange = false;
        if (event_status == FuriStatusOk) {
            if (event.type == EventTypeKey) {
                if (event.input.type == InputTypePress) {
                    game_state->longPress = false;
                    switch (event.input.key) {
                        case InputKeyUp:
                        case InputKeyDown:
                        case InputKeyRight:
                        case InputKeyLeft:
                        case InputKeyOk:
                            if (event.input.key == InputKeyOk && localstate->state == GameStateStart) {
                                localstate->state = GameStatePlay;
                                init(game_state);
                            } else {
                                hadChange = true;
                                localstate->input = event.input.key;
                            }
                            break;
                        case InputKeyBack:
                            init(game_state);
                            processing = false;
                            return_code = 1;
                            break;
                        default:
                            break;
                    }
                }
            } else if (event.type == EventTypeTick) {
                tick(localstate, notification);
                processing = localstate->processing;
                localstate->input = InputKeyMAX;
            }
        } else {
            FURI_LOG_W(APP_NAME, "osMessageQueue: event timeout");
            // event timeout
        }
        if (hadChange || game_state->state == GameStateAnimate)
            view_port_update(view_port);
        release_mutex(&state_mutex, localstate);
    }


    notification_message_block(notification, &sequence_display_backlight_enforce_auto);
    furi_timer_free(timer);
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);
    view_port_free(view_port);
    delete_mutex(&state_mutex);

    free_and_exit:
    ui_cleanup();

    free(game_state);
    furi_message_queue_free(event_queue);
    return return_code;
}