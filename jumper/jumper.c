#include <furi.h>
#include "common/engine/game_engine.h"
#include "defines.h"
#include "jumper_icons.h"

typedef struct {
    float range;
    bool flipped;
} ball_data;

void init_ball(ComponentInfo *component, void *state) {
    UNUSED(state);
    ball_data *data = (ball_data *) component->data;
    data->range = 10;
    data->flipped = false;
}

void update_ball(ComponentInfo *component, void *state) {
    UNUSED(state);
    ball_data *data = (ball_data *) component->data;
    component->entity->transform.position.x += data->flipped ? -1 : 1;

    if (component->entity->transform.position.x > data->range || component->entity->transform.position.x <= 0)
        data->flipped = !data->flipped;
}

Scene *setup_play_scene() {
    //Create new scene
    Scene *s = new_scene("Play");
    //Create new entity and set up data for it
    entity_t *e = new_entity("Ball");
    e->transform.position = (Vector) {10, 10};
    //Store image that will be drawn on render
    e->sprite.data = icon_get_data(&I_ball);
    //Size of the image
    e->sprite.size = (Vector) {16, 16};
    //Enable drawing
    e->draw = true;

    add_component(e, init_ball, update_ball, sizeof(ball_data));

    //Add to scene
    add_to_scene(s, e);

    return s;
}

void init(void *state) {
    UNUSED(state);
}

int32_t jumper_app(void *p) {
    UNUSED(p);
    Scene *scene = setup_play_scene();

    int32_t return_code = setup_engine((SetupState) {
            "Game",             //APP name
            sizeof(GameState),  //size of game state
            init,               //callback to initialize game state
            true,               //keep backlight on
            30                  //update freq
    });
    set_scene(scene);
    start_loop();                                   //start main loop
    return return_code;
}