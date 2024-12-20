/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#ifndef INC_BTREE
#define INC_BTREE

enum apivalue_btree
  {
    apivalue_btree_success,
    apivalue_btree_error_invalid_mode,
    apivalue_btree_error_null_argument,
    apivalue_btree_error_unexpected_branch,
    apivalue_btree_error_uninitialized_argument,
    apivalue_btree_error_wrong_tree,
    apivalue_btree_insertion_none = 0,
    apivalue_btree_insertion_only_if_not_found,
    apivalue_btree_insertion_always,
    apivalue_btree_insertion_replace_only_if_found,
    apivalue_btree_insertion_modes,
    apivalue_btree_direction_up = 0,
    apivalue_btree_direction_less,
    apivalue_btree_direction_more,
    apivalue_btree_directions,
    apivalue_btree_zero = 0
  };

struct api_btree;
struct btree;
struct btree_node;

typedef int apifunction_btree_compare(struct api_btree *, struct btree *, struct btree_node *, struct btree_node *);
typedef enum apivalue_btree apifunction_btree_delete(struct api_btree *, struct btree *, struct btree_node *);
typedef enum apivalue_btree apifunction_btree_find_or_insert(struct api_btree *, struct btree_node *, struct btree *, apifunction_btree_compare *, enum apivalue_btree, struct btree_node **);
typedef void apifunction_btree_initialize(struct api_btree *, struct btree *);
typedef enum apivalue_btree apifunction_btree_api_initialize(struct api_btree *);
typedef void apifunction_btree_initialize_node(struct api_btree *, struct btree_node *);
typedef struct btree_node * apifunction_btree_ordered_visit(struct api_btree *, struct btree *, struct btree_node *, enum apivalue_btree);

extern apifunction_btree_api_initialize api_btree_initialize;

struct btree
  {
    struct btree_node * root;
    enum apivalue_btree last_direction;
  };

struct btree_node
  {
    struct btree_node * links[apivalue_btree_directions];
  };

struct api_btree
  {
    apifunction_btree_api_initialize * api_initialize;
    apifunction_btree_delete * delete;
    apifunction_btree_find_or_insert * find_or_insert;
    apifunction_btree_initialize * initialize;
    apifunction_btree_initialize_node * initialize_node;
    apifunction_btree_ordered_visit * ordered_visit;
  };

#endif /* INC_BTREE */
