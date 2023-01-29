#include <stdlib.h>
#include <furi.h>
#include <dolphin/dolphin.h>
#include "common/engine/game_engine.h"
#include "defines.h"
#include "jumper_icons.h"

void setup_play_scene() {
    FURI_LOG_D("SCENE", "new scene");
    Scene *s = new_scene("Play");
    FURI_LOG_D("SCENE", "new entity");
    entity_t *e = new_entity("Ball");
/*
    FURI_LOG_D("SCENE", "sprite");
    e->sprite.data = icon_get_data(&I_ball);
    FURI_LOG_D("SCENE", "sprite size");
    e->sprite.size=(Vector){16,16};
    FURI_LOG_D("SCENE", "sprite draw");
    e->draw = true;
    */
    FURI_LOG_D("SCENE", "init");
    instantiate_entity(s, e);

    FURI_LOG_D("SCENE", "set");
    set_scene(s);
    FURI_LOG_D("SCENE", "after set");

}


void init(void *state) {
    UNUSED(state);
}

int32_t jumper_app(void *p) {
    UNUSED(p);

    FURI_LOG_D("Jumper", "setup");
    int32_t return_code = setup((SetupState) {
            "Jumper", sizeof(GameState), init, true, furi_kernel_get_tick_frequency() / 1
    });
    FURI_LOG_D("scene", "scene");
    setup_play_scene();
    FURI_LOG_D("start", "start");
    start_loop();
    FURI_LOG_D("clean", "clean");
    cleanup();
    return return_code;
}