#include <stdlib.h>
#include <dolphin/dolphin.h>
#include <furi.h>
#include <gui/canvas_i.h>
#include "defines.h"
#include "common/ui.h"
#include "solitaire_icons.h"

int8_t columns[7][3] = {
        {1,   1, 25},
        {19,  1, 25},
        {37,  1, 25},
        {55,  1, 25},
        {73,  1, 25},
        {91,  1, 25},
        {109, 1, 25},
};

bool can_place_card(Card where, Card what) {
    FURI_LOG_D(APP_NAME, "TESTING pip %i, letter %i with pip %i, letter %i", where.pip, where.character, what.pip,
               what.character);
    bool a_black = where.pip == 0 || where.pip == 3;
    bool b_black = what.pip == 0 || what.pip == 3;
    if (a_black == b_black) return false;

    int8_t a_letter = (int8_t) where.character;
    int8_t b_letter = (int8_t) what.character;
    if (a_letter == 12) a_letter = -1;
    if (b_letter == 12) b_letter = -1;

    return (a_letter-1) == b_letter;
}

static void render_callback(Canvas *const canvas, void *ctx) {
    const GameState *game_state = acquire_mutex((ValueMutex *) ctx, 25);
    if (game_state== NULL) {
        return;
    }

    int deckIndex = game_state->deck.index;
    if (game_state->dragging_deck)
        deckIndex--;
    switch (game_state->state) {
        case GameStateStart:
            canvas_draw_icon(canvas, 0, 0, &I_solitaire_main);
            break;
        case GameStatePlay:
            if (game_state->deck.index < (game_state->deck.card_count - 1) || game_state->deck.index == -1) {
                draw_card_back_at(columns[0][0], columns[0][1], canvas);
                if (game_state->selectRow == 0 && game_state->selectColumn == 0)
                    draw_rounded_box(canvas, columns[0][0], columns[0][1], CARD_WIDTH, CARD_HEIGHT, Inverse);
            } else
                draw_card_space(columns[0][0], columns[0][1],
                                game_state->selectRow == 0 && game_state->selectColumn == 0,
                                canvas);
            //deck side
            if (deckIndex >= 0) {
                Card c = game_state->deck.cards[deckIndex];
                draw_card_at(columns[1][0], columns[1][1], c.pip, c.character, canvas);
            } else
                draw_card_space(columns[1][0], columns[1][1],
                                game_state->selectRow == 0 && game_state->selectColumn == 1,
                                canvas);

            for (uint8_t i = 0; i < 4; i++) {
                Card current = game_state->top_cards[i];
                bool selected = game_state->selectRow == 0 && game_state->selectColumn == (i + 3);
                if (current.disabled) {
                    draw_card_space(columns[i + 3][0], columns[i + 3][1], selected, canvas);
                } else {
                    draw_card_at(columns[i + 3][0], columns[i + 3][1], current.pip, current.character, canvas);
                    if (selected) {
                        draw_rounded_box(canvas, columns[i + 3][0], columns[i + 3][1], CARD_WIDTH, CARD_HEIGHT,
                                         Inverse);
                    }
                }
            }
            for (uint8_t i = 0; i < 7; i++) {
                bool selected = game_state->selectRow == 1 && game_state->selectColumn == i;
                draw_hand_column(game_state->bottom_columns[i], columns[i][0], columns[i][2], 4,
                                 selected ? game_state->selected_card : 0, canvas);
            }

            int8_t pos[2] = {columns[game_state->selectColumn][0],
                             columns[game_state->selectColumn][game_state->selectRow + 1]};

            draw_icon_clip(canvas, &I_card_graphics, pos[0] + CARD_HALF_WIDTH, pos[1] + CARD_HALF_HEIGHT, 30, 5, 5, 5, Filled);

            if (game_state->dragging_hand.index > 0) {
                draw_hand_column(game_state->dragging_hand,
                                 pos[0] + CARD_HALF_WIDTH + 3, pos[1] + CARD_HALF_HEIGHT + 3,
                                 4, -1, canvas);
            }

            break;
        default:
            break;
    }

    release_mutex((ValueMutex *) ctx, game_state);

}

void remove_drag(GameState *gameState) {
    if (gameState->dragging_deck) {
        remove_from_deck(gameState->deck.index, &(gameState->deck));
        gameState->dragging_deck = false;
    } else if (gameState->dragging_column < 7) {
        gameState->dragging_column = 8;
    }
    gameState->dragging_hand.index = 0;
}

bool handleInput(GameState *game_state) {
    Hand currentHand = game_state->bottom_columns[game_state->selectColumn];
    switch (game_state->input) {
        case InputKeyUp:
            if (game_state->selectRow > 0) {
                int first = first_non_flipped_card(currentHand);
                first=currentHand.index-first;
                if(first>game_state->selected_card && game_state->dragging_hand.index == 0){
                    game_state->selected_card++;
                }else{
                    game_state->selectRow--;
                    game_state->selected_card=1;
                }
            }
            break;
        case InputKeyDown:
            if (game_state->selectRow < 1) {
                game_state->selectRow++;
                game_state->selected_card=1;
            }else{
                if(game_state->selected_card>1){
                    game_state->selected_card--;
                }
            }
            break;
        case InputKeyRight:
            if (game_state->selectColumn < 6) {
                game_state->selectColumn++;
                game_state->selected_card=1;
            }
            break;
        case InputKeyLeft:
            if (game_state->selectColumn > 0) {
                game_state->selectColumn--;
                game_state->selected_card=1;
            }
            break;
        case InputKeyOk:
            return true;
            break;
        default:
            break;
    }

    if(game_state->dragging_hand.index > 0)
        game_state->selected_card=1;
    return false;
}

void tick(GameState *game_state) {
    uint8_t row = game_state->selectRow;
    uint8_t column = game_state->selectColumn;
    if (game_state->state != GameStatePlay) return;

    if (handleInput(game_state)) {

        if (row == 0 && column == 0 && game_state->dragging_hand.index == 0) {
            FURI_LOG_D(APP_NAME, "Drawing card");
            game_state->deck.index++;
            if (game_state->deck.index >= (game_state->deck.card_count))
                game_state->deck.index = -1;
        }
            //pick/place from deck
        else if (row == 0 && column == 1) {
            //place
            if (game_state->dragging_deck) {
                game_state->dragging_deck = false;
                game_state->dragging_hand.index = 0;
            }
                //pick
            else {
                if (game_state->dragging_hand.index == 0 && game_state->deck.index >= 0) {
                    game_state->dragging_deck = true;
                    add_to_hand(&(game_state->dragging_hand), game_state->deck.cards[game_state->deck.index]);
                }
            }
        }
            //place on top row
        else if (row == 0 && game_state->dragging_hand.index == 1) {
            column -= 3;
            Card c = game_state->top_cards[column];
            Card currCard = game_state->dragging_hand.cards[0];

            if (c.disabled && currCard.character == 12) {
                game_state->top_cards[column] = currCard;
                remove_drag(game_state);
            } else if (c.pip == currCard.pip) {
                int8_t a_letter = (int8_t) c.character;
                int8_t b_letter = (int8_t) currCard.character;
                if (a_letter == 12) a_letter = -1;
                if (b_letter == 12) b_letter = -1;
                if ((a_letter+1) == b_letter) {
                    game_state->top_cards[column] = currCard;
                    remove_drag(game_state);
                }
            }
        }
            //pick/place from bottom
        else if (row == 1) {
            Hand *curr_hand = &(game_state->bottom_columns[column]);
            //pick up
            if (game_state->dragging_hand.index == 0) {
                Card curr_card = curr_hand->cards[curr_hand->index - 1];
                if (curr_card.flipped) {
                    curr_hand->cards[curr_hand->index - 1].flipped = false;
                } else {
                    extract_hand_region(curr_hand,&(game_state->dragging_hand), curr_hand->index-game_state->selected_card);
//                    add_to_hand(&(game_state->dragging_hand), curr_card);
                    game_state->selected_card=1;
                    game_state->dragging_column = column;
                }
            }
                //place
            else {
                Card first = game_state->dragging_hand.cards[0];
                if (game_state->dragging_column == column ||
                    (curr_hand->index == 0 && first.character == 11) ||
                    can_place_card(curr_hand->cards[curr_hand->index - 1], first)
                        ) {
                    add_hand_region(curr_hand, &(game_state->dragging_hand));
                    remove_drag(game_state);
                }
            }
        }

    }
    if (game_state->selectRow == 0 && game_state->selectColumn == 2) {
        if (game_state->input == InputKeyRight)
            game_state->selectColumn++;
        else
            game_state->selectColumn--;
    }
}

void init(GameState *game_state) {
    game_state->selectColumn = 0;
    game_state->selected_card = 1;
    game_state->selectRow = 0;
    generate_deck(&(game_state->deck), 1);
    shuffle_deck(&(game_state->deck));
    game_state->dragging_deck = false;
    game_state->dragging_column = 8;

    for (uint8_t i = 0; i < 7; i++) {
        game_state->bottom_columns[i].index = 0;
        for (uint8_t j = 0; j <= i; j++) {
            Card cur = remove_from_deck(0, &(game_state->deck));
          //  cur.flipped = i != j;
            add_to_hand(&(game_state->bottom_columns[i]), cur);
        }
    }

    for (uint8_t i = 0; i < 4; i++) {
        game_state->top_cards[i] = (Card) {0, 0, true, false};
    }

    game_state->deck.index = -1;
}

void init_start(GameState *game_state) {
    generate_deck(&(game_state->deck), 1);
    game_state->input = InputKeyMAX;
    for (uint8_t i = 0; i < 7; i++)
        init_hand(&(game_state->bottom_columns[i]), 13);

    init_hand(&(game_state->dragging_hand), 13);

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
    set_card_graphics(&I_card_graphics);
    game_state->state = GameStateStart;
    game_state->processing = true;
    ValueMutex state_mutex;
    if (!init_mutex(&state_mutex, game_state, sizeof(GameState))) {
        FURI_LOG_E(APP_NAME, "cannot create mutex\r\n");
        return_code = 255;
        goto free_and_exit;
    }

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
                    switch (event.input.key) {
                        case InputKeyUp:
                        case InputKeyDown:
                        case InputKeyRight:
                        case InputKeyLeft:
                        case InputKeyOk:
                            if (event.input.key == InputKeyOk && localstate->state == GameStateStart)
                                localstate->state = GameStatePlay;
                            else {
                                hadChange = true;
                                localstate->input = event.input.key;
                            }
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
                localstate->input = InputKeyMAX;
            }
        } else {
            FURI_LOG_W(APP_NAME, "osMessageQueue: event timeout");
            // event timeout
        }
        release_mutex(&state_mutex, localstate);
        if (hadChange)
            view_port_update(view_port);
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

    free(game_state->deck.cards);
    queue_clear(&(game_state->queue_state));
    free(game_state);
    furi_message_queue_free(event_queue);
    return return_code;
}