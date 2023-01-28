#include "objects.h"

Scene *new_scene(const char *name) {
    Scene *s = malloc(sizeof(Scene));
    s->name = name;
    s->entities = make_list(sizeof(entity_t));
    return s;
}

entity_t *new_entity(const char *name) {
    entity_t *e = malloc(sizeof(entity_t));
    e->enabled = true;
    e->draw = false;
    e->name = name;
    e->transform.position = (Vector) {0, 0};
    e->components = make_list(sizeof(component_t));
    e->transform.children = make_list(sizeof(transform_t));
    e->transform.entity = e;
    e->sprite.flipped = false;
    return e;
}

void instantiate_entity(Scene *s, entity_t *entity) {
    list_add(s->entities, entity);
}

void clear_component_data(List *l) {
    t_ListItem *li = l->start;
    while (li) {
        component_t *t = (component_t *) li->data;
        free(t->data);
        li = li->next;
    }

    list_free(l);
}

void clear_branch(List *l) {
    if (l->count == 0) return;

    t_ListItem *li = l->start;
    while (li) {
        entity_t *t = (entity_t *) li->data;
        clear_branch(t->transform.children);
        clear_component_data(t->components);
        li = li->next;
    }
    list_free(l);
}

void clear_scene(Scene *scene) {
    FURI_LOG_D("obj", "SCENE CLEAR");
    if(!scene) return;
    if(scene->entities) {
        clear_branch(scene->entities);
        list_free(scene->entities);
    }
    free(scene);
}