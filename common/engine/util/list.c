#include "list.h"
#include <furi.h>


List *make_list(size_t item_size) {
    List *l = malloc(sizeof(List));
    l->count = 0;
    l->item_size = item_size;
    return l;
}

void list_add(List *list, void *data) {
    list->count++;
    if (list->count > 1) {
        t_ListItem *c = list->start;
        while (c->next) {
            c = c->next;
        }
        c->next = malloc(sizeof(t_ListItem));
        c->next->data = data;
    } else {
        list->start = malloc(sizeof(t_ListItem));
        list->start->data = data;
    }
}

void *list_get(List *list, uint32_t index) {
    if (index < list->count) {
        uint32_t curr_id = 0;
        t_ListItem *s = list->start;
        while (curr_id < index) {
            s = s->next;
            curr_id++;
        }
        return s->data;
    } else {
        FURI_LOG_E("list_get", "Index out of range!");
        return NULL;
    }
}

bool list_splice(List *list, uint32_t index, uint32_t count) {
    if (index < list->count && (index + count) < list->count) {
        uint32_t curr_id = 0;
        t_ListItem *s = list->start;
        while (curr_id < index) {
            s = s->next;
            curr_id++;
        }
        t_ListItem *e = s;
        while (curr_id < (index + count)) {
            t_ListItem *t = e;
            e = e->next;
            free(t->data);
            free(t);
            curr_id++;
        }
        if (index > 0)
            s->next = e;
        else
            list->start = e;

        list->count -= count;

        return true;
    } else {
        FURI_LOG_E("list_splice", "Index out of range!");
        return false;
    }
}

void list_clear(List *list) {
    t_ListItem *item = list->start;
    if(item == NULL) return;
    while (item->next) {
        t_ListItem *t = item;
        item = item->next;
        if(t->data)
            free(t->data);
        free(t);
    }
    free(item->data);
    free(item);
    list->count = 0;
}

void list_free(List *list) {
    list_clear(list);
    free(list);
}