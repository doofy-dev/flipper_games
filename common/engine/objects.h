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
    entity_t *parent;
    List *children;
};

typedef struct{
    void *data;
    entity_t *entity;
}ComponentInfo;

struct Component {
    ComponentInfo componentInfo;

    void (*start)(ComponentInfo *component, void *game_state);

    void (*update)(ComponentInfo *component, void *game_state);
};

//multiple sprite type
struct Sprite{
    const uint8_t *data;
    Vector size;
};


struct Entity {
    transform_t transform;
    List *components;
    sprite_t sprite;

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
void add_to_scene(Scene *s, entity_t *entity);
void clear_scene(Scene *scene);
void add_component(entity_t *entity, void (*start)(ComponentInfo *component, void *state), void (*update)(ComponentInfo *component, void *state), size_t data_size);
void add_to_entity(entity_t *parent, entity_t *child);