#include "objects.h"

Scene *new_scene(const char *name) {
    FURI_LOG_D("FlipperGameEngine", "Initializing new scene: %s", name);
    Scene *s = malloc(sizeof(Scene));
    s->name = name;
    s->entities = make_list(sizeof(entity_t));
    return s;
}

entity_t *new_entity(const char *name) {
    FURI_LOG_D("FlipperGameEngine", "Initializing new entity: %s", name);
    entity_t *e = malloc(sizeof(entity_t));
    e->enabled = true;
    e->draw = false;
    e->name = name;
    e->transform.position = (Vector) {0, 0};
    e->components = make_list(sizeof(component_t));
    e->transform.children = make_list(sizeof(transform_t));
    e->transform.entity = e;
    return e;
}

void add_to_scene(Scene *s, entity_t *entity) {
    FURI_LOG_D("FlipperGameEngine", "Adding entity '%s' to scene '%s'", entity->name, s->name);
    list_add(s->entities, entity);
}

void clear_component_data(List *l) {
    t_ListItem *li = l->start;
    if (li == NULL) return;
    while (li) {
        component_t *t = (component_t *) li->data;
        free(t->componentInfo.data);
        li = li->next;
    }

    list_free(l);
}

void clear_branch(List *l) {
    if (l->count == 0) return;

    t_ListItem *li = l->start;
    if (li == NULL) return;
    while (li) {
        entity_t *t = (entity_t *) li->data;
        FURI_LOG_D("FlipperGameEngine", "Clearing entity: %s", t->name);
        clear_branch(t->transform.children);
        FURI_LOG_D("FlipperGameEngine", "Freeing components");
        clear_component_data(t->components);
        li = li->next;
    }
    list_free(l);
}

void clear_scene(Scene *scene) {
    FURI_LOG_D("FlipperGameEngine", "Clearing scene: %s", scene->name);
    if (!scene) return;
    if (scene->entities) {
        clear_branch(scene->entities);
        list_free(scene->entities);
    }
    free(scene);
}

void add_component(entity_t *entity, void (*start)(ComponentInfo *component, void *state),
                   void (*update)(ComponentInfo *component, void *state), size_t data_size) {
    FURI_LOG_D("FlipperGameEngine", "Adding component to: %s", entity->name);
    component_t *component = (component_t *) malloc(sizeof(component_t));
    component->update = update;
    component->start = start;
    component->componentInfo.entity = entity;
    if (data_size > 0)
        component->componentInfo.data = malloc(data_size);
    list_add(entity->components, component);
}