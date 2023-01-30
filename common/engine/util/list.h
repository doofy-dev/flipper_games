#pragma once

#include <furi.h>
typedef struct ListItem t_ListItem;

struct ListItem{
    t_ListItem *next;
    void *data;
};

typedef struct {
    uint32_t count;
    t_ListItem *start;
    size_t item_size;
} List;

List *make_list(size_t item_size);
void *list_get(List *list, uint32_t index);
void list_add(List *list, void *data);
bool list_splice(List *list, uint32_t index, uint32_t count);
void list_clear(List *list);
void list_free(List *list);
void list_remove_item(List *list, void *data);