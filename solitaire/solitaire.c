#include <stdlib.h>
#include <dolphin/dolphin.h>
#include <furi.h>
#include <gui/canvas_i.h>
#include "defines.h"
#include "common/ui.h"
#include "solitaire_icons.h"

static void render_callback(Canvas *const canvas, void *ctx) {
    UNUSED(ctx);

    draw_card_back_at(2, 0, canvas);
    draw_card_space(110, 0, canvas);
    draw_card_space(92, 0, canvas);
    draw_card_space(74, 0, canvas);
    draw_card_space(56, 0, canvas);


    draw_card_space(2, 25, canvas);
    draw_card_space(20, 25, canvas);
    draw_card_space(38, 25, canvas);
    draw_card_space(56, 25, canvas);
    draw_card_space(74, 25, canvas);
    draw_card_space(92, 25, canvas);
    draw_card_space(110, 25, canvas);



//    draw_rounded_box(canvas, 20,10,CARD_WIDTH,CARD_HEIGHT, Black);
//    draw_rounded_box_frame(canvas, 22,12,CARD_WIDTH-4,CARD_HEIGHT-4, White);


/*
    draw_rounded_box(canvas, 10,10,10,10, Black);

    draw_rounded_box(canvas, 22,10,10,10, White);
    draw_rounded_box_frame(canvas, 22,10,10,10, Black);

    draw_rounded_box(canvas, 44,10,10,10, Black);*//*


    canvas_set_color(canvas, ColorBlack);
    canvas_draw_str(canvas, 56, 18, "Invert");

    canvas_draw_str(canvas, 55, 50, "Rounded invert");
    canvas_draw_str(canvas, 10, 40, "Custom shape");

    draw_icon_clip(canvas, &I_card_grapics, 100,0, 0,0, 5,5, Black);
    draw_icon_clip(canvas, &I_card_grapics, 100,10, 0,5, 5,5, Black);
    draw_icon_clip(canvas, &I_card_grapics, 100,20, 0,10, 7,7, Black);
*/

}

void tick(GameState *game_state) {
    UNUSED(game_state);
}

void init(GameState *game_state) {
    game_state->has_side = false;
    game_state->deckIndex = 0;
    shuffle_deck(&(game_state->deck));
    for (uint8_t i = 0; i < 7; i++)
        game_state->bottom_columns[i].index = 0;
    for (uint8_t i = 0; i < 4; i++)
        game_state->top_cards[i].index = 0;
}

void init_start(GameState *game_state) {
    generate_deck(&(game_state->deck), 1);
    for (uint8_t i = 0; i < 7; i++)
        init_hand(&(game_state->bottom_columns[i]), 13);
    for (uint8_t i = 0; i < 4; i++)
        init_hand(&(game_state->top_cards[i]), 13);
    init(game_state);
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
    set_card_graphics(&I_card_grapics);

    ViewPort *view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, &state_mutex);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    FuriTimer *timer =
            furi_timer_alloc(update_timer_callback, FuriTimerTypePeriodic, event_queue);
    furi_timer_start(timer, furi_kernel_get_tick_frequency() / 25);

    Gui *gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    AppEvent event;
    for (bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);
        GameState *localstate = (GameState *) acquire_mutex_block(&state_mutex);
        if (event_status == FuriStatusOk) {
            if (event.type == EventTypeKey) {
                if (event.input.type == InputTypePress) {
                    switch (event.input.key) {
                        case InputKeyUp:
                        case InputKeyDown:
                        case InputKeyRight:
                        case InputKeyOk:
                        case InputKeyLeft:
                            game_state->input = event.input.key;
                            break;
                        case InputKeyBack:
                            processing = false;
                            return_code = 1;
                            break;
                        default:
                            break;
                    }
                }
            } else if (event.type == EventTypeTick) {
                tick(localstate);
                processing = localstate->processing;
            }
        } else {
            FURI_LOG_D(APP_NAME, "osMessageQueue: event timeout");
            // event timeout
        }
        view_port_update(view_port);
        release_mutex(&state_mutex, localstate);
    }

    furi_timer_free(timer);
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(view_port);
    delete_mutex(&state_mutex);

    free_and_exit:
    ui_cleanup();
    for (uint8_t i = 0; i < 7; i++)
        free_hand(&(game_state->bottom_columns[i]));
    for (uint8_t i = 0; i < 4; i++)
        free_hand(&(game_state->top_cards[i]));

    free(game_state->deck.cards);
    queue_clear(&(game_state->queue_state));
    free(game_state);
    furi_message_queue_free(event_queue);
    return return_code;
}