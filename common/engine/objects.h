#pragma once
#include <furi.h>
#include "util/list.h"
#include "../dml.h"

typedef struct Entity entity_t;
typedef struct Transform transform_t;
typedef struct Component component_t;
typedef struct Sprite sprite_t;

struct Transform {
    Vector position;
    entity_t *entity;
    transform_t *parent;
    List *children;
};

struct Component {
    void *data;

    void (*start)(void *state);

    void (*update)(void *state);

    entity_t *entity;
};

//multiple sprite type
typedef struct{
    const uint8_t *data;
    bool flipped;
    Vector size;
} Sprite;


struct Entity {
    transform_t transform;
    List *components;
    Sprite sprite;

    bool enabled;
    bool draw;
    const char* name;
};

typedef struct {
    const char* name;
    List *entities;
} Scene;

Scene *new_scene(const char* name);
entity_t *new_entity(const char* name);
void instantiate_entity(Scene *s, entity_t *entity);
void clear_scene(Scene *scene);