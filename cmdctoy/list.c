/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#include <stddef.h>
#include "list.h"

static apifunction_list_add_item_to_list_head add_item_to_list_head;
static apifunction_list_add_item_to_list_tail add_item_to_list_tail;
static apifunction_list_initialize_list initialize_list;
static apifunction_list_initialize_list_item initialize_list_item;
static apifunction_list_list_is_empty list_is_empty;
static apifunction_list_remove_item_from_list_head remove_item_from_list_head;
static apifunction_list_remove_item_from_list_tail remove_item_from_list_tail;
static apifunction_list_remove_list_item remove_list_item;
static apifunction_list_replace_list_item replace_list_item;

static struct api_list api_list_defaults =
  {
    &api_list_initialize,
    &add_item_to_list_head,
    &add_item_to_list_tail,
    &initialize_list,
    &initialize_list_item,
    &list_is_empty,
    &remove_item_from_list_head,
    &remove_item_from_list_tail,
    &remove_list_item,
    &replace_list_item
  };

static struct list_item * add_item_to_list_head(struct api_list * api, struct list_item * list_item, struct list * list)
  {
    (void) api;

    list_item->previous = &list->head;
    list_item->next = list->head.next;
    list->head.next = list_item;
    list_item->next->previous = list_item;
    return list_item;
  }

static struct list_item * add_item_to_list_tail(struct api_list * api, struct list_item * list_item, struct list * list)
  {
    (void) api;

    list_item->previous = list->head.previous;
    list_item->next = &list->head;
    list->head.previous = list_item;
    list_item->previous->next = list_item;
    return list_item;
  }

enum apivalue_list api_list_initialize(struct api_list * api)
  {
    *api = api_list_defaults;
    return apivalue_list_success;
  }

static struct list * initialize_list(struct api_list * api, struct list * list)
  {
    (void) api;

    list->head.previous = &list->head;
    list->head.next = &list->head;
    return list;
  }

static struct list_item * initialize_list_item(struct api_list * api, struct list_item * list_item)
  {
    (void) api;

    list_item->previous = NULL;
    list_item->next = NULL;
    return list_item;
  }

static int list_is_empty(struct api_list * api, struct list * list)
  {
    (void) api;

    if (list->head.previous == &list->head)
      return 1;
    return 0;
  }

static struct list_item * remove_item_from_list_head(struct api_list * api, struct list * list)
  {
    if (list->head.next == &list->head)
      return NULL;
    return remove_list_item(api, list->head.next);
  }

static struct list_item * remove_item_from_list_tail(struct api_list * api, struct list * list)
  {
    if (list->head.previous == &list->head)
      return NULL;
    return remove_list_item(api, list->head.previous);
  }

static struct list_item * remove_list_item(struct api_list * api, struct list_item * list_item)
  {
    (void) api;

    list_item->previous->next = list_item->next;
    list_item->next->previous = list_item->previous;
    list_item->previous = NULL;
    list_item->next = NULL;
    return list_item;
  }

static struct list_item * replace_list_item(struct api_list * api, struct list_item * old_item, struct list_item * new_item)
  {
    (void) api;

    new_item->previous = old_item->previous;
    new_item->next = old_item->next;
    new_item->previous->next = new_item;
    new_item->next->previous = new_item;
    old_item->previous = NULL;
    old_item->next = NULL;
    return new_item;
  }
