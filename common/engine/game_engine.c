#include "game_engine.h"

EngineState *engineState;
const char *AppName;
void update_tree(List *items, void *render_state);

static void update(void *render_state) {
    FURI_LOG_D(AppName, "Update called");

    update_tree(engineState->scene->entities, render_state);
}

static void render(Canvas *const canvas, void *ctx) {
    RenderQueue *queue = (RenderQueue *) ctx;
    for (int i = 0; i < queue->render_count; i++) {
        RenderInfo s = queue->render_list[i];
        canvas_draw_bitmap(canvas, s.position.x, s.position.y, s.image.size.x, s.image.size.y, s.image.data);
    }
    queue->render_count = 0;
}


static void handle_input(InputEvent *input_event, FuriMessageQueue *event_queue) {
    furi_assert(event_queue);
    AppEvent event = {.type = EventInput, .inputEvent = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void update_timer(FuriMessageQueue *event_queue) {
    furi_assert(event_queue);
    AppEvent event = {.type = EventTick};
    furi_message_queue_put(event_queue, &event, 0);
}

int32_t setup(SetupState state) {
    AppName = state.AppName;
    engineState = malloc(sizeof(EngineState));
    engineState->event_queue = furi_message_queue_alloc(8, sizeof(AppEvent));

    engineState->gameState = malloc(state.stateSize);
    engineState->renderQue = malloc(sizeof(RenderQueue));
    engineState->renderQue->render_count = 0;
    state.initState(engineState->gameState);
    if (!init_mutex(&(engineState->render_mutex), engineState->renderQue, sizeof(RenderQueue))) {
        FURI_LOG_E(AppName, "Cannot create mutex!");
        engineState->loaded = false;
        return 255;
    }


    engineState->scene = NULL;
    engineState->notificationApp = furi_record_open(RECORD_NOTIFICATION);
    if (state.alwaysOnScreen)
        notification_message_block(engineState->notificationApp, &sequence_display_backlight_enforce_on);

    engineState->viewPort = view_port_alloc();
    view_port_draw_callback_set(engineState->viewPort, render, &(engineState->render_mutex));
    view_port_input_callback_set(engineState->viewPort, handle_input, &(engineState->event_queue));

    engineState->timer = furi_timer_alloc(update_timer, FuriTimerTypePeriodic, &(engineState->event_queue));

    furi_timer_start(engineState->timer, state.tickRate);

    engineState->gui = furi_record_open("gui");

    gui_add_view_port(engineState->gui, engineState->viewPort, GuiLayerFullscreen);

    engineState->loaded = true;

    return 0;
}

void start_loop() {
    if (!engineState->loaded) {
        FURI_LOG_E(AppName, "Cannot start game, load failed!");
        return;
    }

    AppEvent event;
    engineState->processing = true;

    FURI_LOG_D(AppName, "Entering main loop");
    while (engineState->processing) {

        FuriStatus event_status = furi_message_queue_get(engineState->event_queue, &event, 150);

        void *render_state = acquire_mutex_block(&(engineState->render_mutex));

        if (event_status == FuriStatusOk) {
            if (event.type == EventInput) {
                engineState->lastInput.inputType = event.inputEvent.type;
                engineState->lastInput.inputKey = event.inputEvent.key;
                if (event.inputEvent.type == InputTypePress) {
                    engineState->inputState[event.inputEvent.key] = InputDown;
                } else if (event.inputEvent.type == InputTypeLong) {
                    engineState->inputState[event.inputEvent.key] = InputPress;
                } else if (event.inputEvent.type == InputTypeRelease) {
                    engineState->inputState[event.inputEvent.key] = InputUp;
                }
            }
        } else if (event.type == EventTick) {
            update(render_state);
            engineState->lastInput.inputKey = InputKeyMAX;
            engineState->lastInput.inputType = InputTypeMAX;
        } else {
            FURI_LOG_W(AppName, "osMessageQueue: event timeout");
        }
        view_port_update(engineState->viewPort);
        release_mutex(&(engineState->render_mutex), render_state);
    }
    FURI_LOG_D(AppName, "Exiting loop");
}

void cleanup() {
    FURI_LOG_D(AppName, "Cleaning up");
    if (engineState->loaded) {
        clear_scene(engineState->scene);
        notification_message_block(engineState->notificationApp, &sequence_display_backlight_enforce_auto);
        furi_timer_free(engineState->timer);
        view_port_enabled_set(engineState->viewPort, false);
        gui_remove_view_port(engineState->gui, engineState->viewPort);
        furi_record_close(RECORD_GUI);
        furi_record_close(RECORD_NOTIFICATION);
        view_port_free(engineState->viewPort);
        delete_mutex(&(engineState->render_mutex));
    }

    free(engineState->renderQue);
    free(engineState->gameState);
    furi_message_queue_free(engineState->event_queue);
    free(engineState);
}


void init_tree(List *items) {
    t_ListItem *curr = items->start;
    if (!curr) return;
    while (curr) {
        entity_t *e = (entity_t *) curr->data;
        if (e->enabled) {
            t_ListItem *component = e->components->start;
            while (component) {
                component_t *c = (component_t *) component->data;
                c->start(engineState->gameState);
                component = component->next;
            }
            init_tree(e->transform.children);
        }
        curr = curr->next;
    }
}

void set_scene(Scene *s) {
    FURI_LOG_D(AppName, "SCENE SET");
//   if (engineState->scene)
//        clear_scene(engineState->scene);
    engineState->scene = s;

//    init_tree(s->entities);
}

void update_tree(List *items, void *render_state) {
    return;
    t_ListItem *curr = items->start;
    if (!curr) return;
    while (curr) {
        entity_t *e = (entity_t *) curr->data;
        if (e->enabled) {
            t_ListItem *component = e->components->start;
            while (component) {
                component_t *c = (component_t *) component->data;
                c->update(engineState->gameState);
                component = component->next;
            }
            update_tree(e->transform.children, render_state);
        }
        if (e->draw) {
            RenderQueue *rq = (RenderQueue *) render_state;
            if (rq->render_count < 63) {
                rq->render_list[rq->render_count + 1] = (RenderInfo){
                    e->sprite, e->transform.position
                };
                rq->render_count++;
            }
        }

        curr = curr->next;
    }
}

void exit_app() {
    engineState->processing = false;
}
