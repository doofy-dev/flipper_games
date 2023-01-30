#include <furi.h>
#include "common/engine/game_engine.h"
#include "defines.h"
#include "jumper_icons.h"

typedef struct {
    int range;
    int centerX;
    int centerY;
    bool flipped;
} ball_data;

void init_ball(ComponentInfo *component, void *state) {
    UNUSED(state);
    ball_data *data = (ball_data *) component->data;
    data->range = 10;
    data->centerX = component->entity->transform.position.x + data->range/2;
    data->centerY = component->entity->transform.position.y + data->range/2;
    data->flipped = component->entity->transform.position.x>=10? true : false;
}

void update_ball(ComponentInfo *component, void *state) {
    UNUSED(state);
    ball_data *data = (ball_data *) component->data;
    component->entity->transform.position.x += data->flipped ? -1 : 1;
    int diff = component->entity->transform.position.x - data->centerX;
    if (abs(diff)>=data->range)
        data->flipped = !data->flipped;
}

void update_ballY(ComponentInfo *component, void *state) {
    UNUSED(state);
    ball_data *data = (ball_data *) component->data;
    component->entity->transform.position.y += data->flipped ? -1 : 1;
    int diff = component->entity->transform.position.y - data->centerY;
    if (abs(diff)>=data->range)
        data->flipped = !data->flipped;
}

void setup_play_scene() {
    //Create new scene
    Scene *s = new_scene("Play");
    //Create new entity and set up data for it
    entity_t *e = new_entity("Ball");
    e->transform.position = (Vector) {20, -20};
    //Store image that will be drawn on render
    e->sprite.data = icon_get_data(&I_ball);
    //Size of the image
    e->sprite.size = (Vector) {16, 16};
    //Enable drawing
    e->draw = true;

    entity_t *e2 = new_entity("Ball2");
    e2->transform.position = (Vector) {-20, -20};
    e2->sprite.data = icon_get_data(&I_ball);
    e2->sprite.size = (Vector) {16, 16};
    e2->draw = true;

    entity_t *e3 = new_entity("Ball3");
    e3->transform.position = (Vector) {30, 30};
    e3->sprite.data = icon_get_data(&I_ball);
    e3->sprite.size = (Vector) {16, 16};
    e3->draw = true;

    add_component(e, init_ball, update_ball, sizeof(ball_data));
    add_component(e2, init_ball, update_ball, sizeof(ball_data));
    add_component(e3, init_ball, update_ballY, sizeof(ball_data));

    //Add to scene
    add_to_scene(s, e3);

    add_to_entity(e3, e);
    add_to_entity(e3, e2);
    set_scene(s);
}

void init(void *state) {
    UNUSED(state);
}

int32_t jumper_app(void *p) {
    UNUSED(p);
    int32_t return_code = setup_engine((SetupState) {
            "Game",             //APP name
            sizeof(GameState),  //size of game state
            init,               //callback to initialize game state
            true,               //keep backlight on
            25                  //update freq
    });
    setup_play_scene();
    start_loop();                                   //start main loop
    return return_code;
}