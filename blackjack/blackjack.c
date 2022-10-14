
#include <gui/gui.h>
#include <stdlib.h>
#include <dolphin/dolphin.h>
#include <math.h>
#include "util.h"
#include "defines.h"
#include "card.h"
#include "util.h"
#include "ui.h"

#define APP_NAME "Blackjack"
#define STARTING_MONEY 200
#define DEALER_MAX 17


void start_round(GameState *game_state);

static void draw_ui(Canvas *const canvas, const GameState *game_state) {

    draw_money(canvas, game_state->player_score);

    draw_score(canvas, true, handCount(game_state->player_cards, game_state->player_card_count));

    if (!game_state->animating && game_state->state == GameStatePlay) {
        draw_play_menu(canvas, game_state);
    }
}


static void render_callback(Canvas *const canvas, void *ctx) {
    const GameState *game_state = acquire_mutex((ValueMutex *) ctx, 25);

    if (game_state == NULL) {
        return;
    }
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_frame(canvas, 0, 0, 128, 64);

    switch (game_state->state) {
        case GameStateStart:
        case GameStateGameOver:
            draw_message_scene(canvas, game_state);
            break;
        case GameStatePlay:
            draw_player_scene(canvas, game_state);
            break;
        case GameStateDealer:
            draw_dealer_scene(canvas, game_state);
            break;
    }
    if (game_state->state != GameStateStart && game_state->state != GameStateGameOver) {
        animateQueue(game_state, canvas);
        draw_ui(canvas, game_state);
    }

    release_mutex((ValueMutex *) ctx, game_state);
}

//region card draw
Card draw_card(GameState *game_state) {
    Card c = game_state->deck.cards[game_state->deck.index];
    game_state->deck.index++;
    return c;
}

char *letters[4] = {"spade", "hearth", "diamond", "club"};

void drawPlayerCard(GameState *game_state) {
    Card c = draw_card(game_state);
    game_state->player_cards[game_state->player_card_count] = c;
    game_state->player_card_count++;
}

void drawDealerCard(GameState *game_state) {
    Card c = draw_card(game_state);
    game_state->dealer_cards[game_state->dealer_card_count] = c;
    game_state->dealer_card_count++;
    FURI_LOG_D(APP_NAME, "drawing dealer %s %i", letters[c.pip], c.character + 2);
}
//endregion

//region queue callbacks
void to_lose_state(const GameState *game_state, Canvas *const canvas) {
    UNUSED(game_state);
    popupFrame(canvas);
    elements_multiline_text_aligned(canvas, 64, 22, AlignCenter, AlignCenter, "You lost");
}

void to_bust_state(const GameState *game_state, Canvas *const canvas) {
    UNUSED(game_state);
    popupFrame(canvas);
    elements_multiline_text_aligned(canvas, 64, 22, AlignCenter, AlignCenter, "Busted!");
}

void to_draw_state(const GameState *game_state, Canvas *const canvas) {
    UNUSED(game_state);
    popupFrame(canvas);
    elements_multiline_text_aligned(canvas, 64, 22, AlignCenter, AlignCenter, "Draw");
}

void to_dealer_turn(const GameState *game_state, Canvas *const canvas) {
    UNUSED(game_state);
    popupFrame(canvas);
    elements_multiline_text_aligned(canvas, 64, 22, AlignCenter, AlignCenter, "Dealers turn");
}

void to_win_state(const GameState *game_state, Canvas *const canvas) {
    UNUSED(game_state);
    popupFrame(canvas);
    elements_multiline_text_aligned(canvas, 64, 22, AlignCenter, AlignCenter, "You win");
}

void to_start(const GameState *game_state, Canvas *const canvas) {
    UNUSED(game_state);
    popupFrame(canvas);
    elements_multiline_text_aligned(canvas, 64, 22, AlignCenter, AlignCenter, "Round started");
}

void before_start(GameState *gameState) {
    gameState->dealer_card_count = 0;
    gameState->player_card_count = 0;
}


void start(GameState *game_state) {
    start_round(game_state);
}

void draw(GameState *game_state) {
    game_state->player_score += game_state->bet;
    game_state->bet = 0;
    queue(game_state, start, before_start, to_start);
}

void game_over(GameState *game_state) {
    game_state->state = GameStateGameOver;
}

void lose(GameState *game_state) {
    game_state->state = GameStatePlay;
    game_state->bet = 0;
    if (game_state->player_score >= ROUND_PRICE)
        queue(game_state, start, before_start, to_start);
    else
        queue(game_state, game_over, NULL, NULL);
}

void win(GameState *game_state) {
    game_state->state = GameStatePlay;
    game_state->player_score += game_state->bet * 2;
    game_state->bet = 0;
    queue(game_state, start, before_start, to_start);
}


void dealerTurn(GameState *game_state) {
    game_state->state = GameStateDealer;
}

void dealer_card_animation(const GameState *game_state, Canvas *const canvas){
    float t = (float) (furi_get_tick() - game_state->animationStart) / (ANIMATION_TIME - ANIMATION_END_MARGIN);
    Card animatingCard = game_state->deck.cards[game_state->deck.index];
    if(game_state->dealer_card_count>1){
        Vector end = card_pos_at_index(game_state->dealer_card_count);
        if(!is_at_edge(game_state->dealer_card_count))
            end.x-=CARD_HALF_WIDTH;
        draw_card_animation(animatingCard,
                            (Vector){0, 64},
                            (Vector){0, 32},
                            end,
                            t,
                            true,
                            canvas);
    }else{
        draw_card_animation(animatingCard,
                            (Vector){32, -CARD_HEIGHT},
                            (Vector){64, 32},
                            (Vector){2,2},
                            t,
                            false,
                            canvas);
//        drawPlayerDeck(game_state->dealer_cards, game_state->dealer_card_count, canvas);
    }
}
void dealer_back_card_animation(const GameState *game_state, Canvas *const canvas){
    float t = (float) (furi_get_tick() - game_state->animationStart) / (ANIMATION_TIME - ANIMATION_END_MARGIN);
    Vector currentPos=quadratic_2d((Vector){32, -CARD_HEIGHT}, (Vector){64, 32}, (Vector){13,5},t);
    drawCardBackAt(currentPos.x, currentPos.y, canvas);
}

void player_card_animation(const GameState *game_state, Canvas *const canvas){
    float t = (float) (furi_get_tick() - game_state->animationStart) / (ANIMATION_TIME - ANIMATION_END_MARGIN);
    Card animatingCard = game_state->deck.cards[game_state->deck.index];
    Vector end = card_pos_at_index(game_state->player_card_count);
    if(!is_at_edge(game_state->player_card_count))
        end.x-=CARD_HALF_WIDTH;
    draw_card_animation(animatingCard,
                        (Vector){32, -CARD_HEIGHT},
                        (Vector){0, 32},
                        end,
                        t,
                        true,
                        canvas);
//    drawPlayerDeck(game_state->dealer_cards, game_state->player_card_count, canvas);
}
//endregion

void player_tick(GameState *game_state) {
    uint8_t score = handCount(game_state->player_cards, game_state->player_card_count);
    if ((game_state->doubled && score <= 21) || score == 21) {
        queue(game_state, dealerTurn, NULL, NULL);
    } else if (score > 21) {
        queue(game_state, lose, NULL, to_bust_state);
    } else {
        if (game_state->selectDirection == DirectionUp && game_state->selectedMenu > 0) {
            game_state->selectedMenu--;
        }
        if (game_state->selectDirection == DirectionDown && game_state->selectedMenu < 2) {
            game_state->selectedMenu++;
        }
        if (game_state->selectDirection == Select) {
            //double
            if (!game_state->doubled && game_state->selectedMenu == 0 &&
                game_state->player_score >= ROUND_PRICE) {

                game_state->player_score -= ROUND_PRICE;
                game_state->bet += ROUND_PRICE;
                game_state->doubled = true;
                game_state->selectedMenu = 1;
                queue(game_state, drawPlayerCard, NULL, player_card_animation);
                game_state->player_cards[game_state->player_card_count] = game_state->deck.cards[game_state->deck.index];
                score = handCount(game_state->player_cards, game_state->player_card_count + 1);
                if (score > 21)
                    queue(game_state, lose, NULL, to_bust_state);
                else
                    queue(game_state, dealerTurn, NULL, to_dealer_turn);

            } //hit
            else if (game_state->selectedMenu == 1) {
                queue(game_state, drawPlayerCard, NULL, player_card_animation);
            } //stay
            else if (game_state->selectedMenu == 2) {
                queue(game_state, dealerTurn, NULL, to_dealer_turn);
            }
        }
    }
}

void dealer_tick(GameState *game_state) {
    uint8_t dealer_score = handCount(game_state->dealer_cards, game_state->dealer_card_count);
    uint8_t player_score = handCount(game_state->player_cards, game_state->player_card_count);

    if (dealer_score >= DEALER_MAX) {
        if (dealer_score > 21 || dealer_score < player_score)
            queue(game_state, win, NULL, to_win_state);
        else if (dealer_score > player_score)
            queue(game_state, lose, NULL, to_lose_state);
        else if (dealer_score == player_score)
            queue(game_state, draw, NULL, to_draw_state);
    } else {
        queue(game_state, drawDealerCard, NULL, dealer_card_animation);
    }
}

void tick(GameState *game_state) {
    game_state->last_tick = furi_get_tick();

    if (!game_state->started && game_state->state == GameStatePlay) {
        game_state->started = true;
        queue(game_state, drawDealerCard, NULL, dealer_back_card_animation);
        queue(game_state, drawPlayerCard, NULL, player_card_animation);
        queue(game_state, drawDealerCard, NULL, dealer_card_animation);
        queue(game_state, drawPlayerCard, NULL, player_card_animation);
    }

    if (!run_queue(game_state)) {
        if (game_state->state == GameStatePlay) {
            player_tick(game_state);
        } else if (game_state->state == GameStateDealer) {
            dealer_tick(game_state);
        }
    }

    game_state->selectDirection = None;

}

void start_round(GameState *game_state) {
    game_state->player_card_count = 0;
    game_state->dealer_card_count = 0;
    game_state->selectedMenu = 0;
    game_state->started = false;
    game_state->doubled = false;
    game_state->animating = true;
    game_state->animationStart = 0;
    shuffleDeck(&(game_state->deck));
    game_state->doubled = false;
    game_state->bet = ROUND_PRICE;
    if (game_state->player_score < ROUND_PRICE) {
        game_state->state = GameStateGameOver;
    } else {
        game_state->player_score -= ROUND_PRICE;
    }
    game_state->state = GameStatePlay;
}

void init(GameState *game_state) {
    game_state->last_tick = 0;
    game_state->player_score = STARTING_MONEY;
    generateDeck(&(game_state->deck));
    start_round(game_state);
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

int32_t blackjack_app(void *p) {
    UNUSED(p);

    int32_t return_code = 0;

    FuriMessageQueue *event_queue = furi_message_queue_alloc(8, sizeof(AppEvent));

    GameState *game_state = malloc(sizeof(GameState));
    init(game_state);
    game_state->state = GameStateStart;

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
    furi_timer_start(timer, furi_kernel_get_tick_frequency() / 25);

    Gui *gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    AppEvent event;

    for (bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);
        GameState *game_state = (GameState *) acquire_mutex_block(&state_mutex);

        if (event_status == FuriStatusOk) {
            if (event.type == EventTypeKey) {

                if (event.input.type == InputTypePress) {
                    switch (event.input.key) {

                        case InputKeyUp:
                            game_state->selectDirection = DirectionUp;
                            break;
                        case InputKeyDown:
                            game_state->selectDirection = DirectionDown;
                            break;
                        case InputKeyRight:
                            game_state->selectDirection = DirectionRight;
                            break;
                        case InputKeyLeft:
                            game_state->selectDirection = DirectionLeft;
                            break;
                        case InputKeyBack:
                            processing = false;
                            break;

                        case InputKeyOk:
                            if (game_state->state == GameStateGameOver || game_state->state == GameStateStart) {
                                init(game_state);
                            } else {
                                game_state->selectDirection = Select;
                            }
                            break;
                    }
                }
            } else if (event.type == EventTypeTick) {
                tick(game_state);
            }
        } else {
            FURI_LOG_D(APP_NAME, "osMessageQueue: event timeout");
            // event timeout
        }
        view_port_update(view_port);
        release_mutex(&state_mutex, game_state);
    }


    furi_timer_free(timer);
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(view_port);
    delete_mutex(&state_mutex);

    free_and_exit:
    queue_clear();
    free(game_state);
    furi_message_queue_free(event_queue);

    return return_code;
}