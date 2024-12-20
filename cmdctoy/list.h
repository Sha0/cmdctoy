/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#ifndef INC_LIST
#define INC_LIST

enum apivalue_list
  {
    apivalue_list_success,
    apivalue_list_zero = 0
  };

struct list;
struct api_list;
struct list_item;

typedef struct list_item * apifunction_list_add_item_to_list_head(struct api_list *, struct list_item *, struct list *);
typedef struct list_item * apifunction_list_add_item_to_list_tail(struct api_list *, struct list_item *, struct list *);
typedef enum apivalue_list apifunction_list_api_initialize(struct api_list *);
typedef struct list * apifunction_list_initialize_list(struct api_list *, struct list *);
typedef struct list_item * apifunction_list_initialize_list_item(struct api_list *, struct list_item *);
typedef int apifunction_list_list_is_empty(struct api_list *, struct list *);
typedef struct list_item * apifunction_list_remove_item_from_list_head(struct api_list *, struct list *);
typedef struct list_item * apifunction_list_remove_item_from_list_tail(struct api_list *, struct list *);
typedef struct list_item * apifunction_list_remove_list_item(struct api_list *, struct list_item *);
typedef struct list_item * apifunction_list_replace_list_item(struct api_list *, struct list_item *, struct list_item *);

extern apifunction_list_api_initialize api_list_initialize;

struct list_item
  {
    struct list_item * previous;
    struct list_item * next;
  };

struct list
  {
    struct list_item head;
  };

struct api_list
  {
    apifunction_list_api_initialize * api_initialize;
    apifunction_list_add_item_to_list_head * add_item_to_list_head;
    apifunction_list_add_item_to_list_tail * add_item_to_list_tail;
    apifunction_list_initialize_list * initialize_list;
    apifunction_list_initialize_list_item * initialize_list_item;
    apifunction_list_list_is_empty * list_is_empty;
    apifunction_list_remove_item_from_list_head * remove_item_from_list_head;
    apifunction_list_remove_item_from_list_tail * remove_item_from_list_tail;
    apifunction_list_remove_list_item * remove_list_item;
    apifunction_list_replace_list_item * replace_list_item;
  };

#endif /* INC_LIST */
