/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#include <stddef.h>
#include "btree.h"

static apifunction_btree_delete btree_delete;
static apifunction_btree_find_or_insert btree_find_or_insert;
static apifunction_btree_initialize btree_initialize;
static apifunction_btree_initialize_node btree_initialize_node;
static apifunction_btree_ordered_visit btree_ordered_visit;

static struct api_btree api_btree_defaults;

static struct api_btree api_btree_defaults =
  {
    &api_btree_initialize,
    &btree_delete,
    &btree_find_or_insert,
    &btree_initialize,
    &btree_initialize_node,
    &btree_ordered_visit
  };

static enum apivalue_btree btree_delete(struct api_btree * api, struct btree * btree, struct btree_node * btree_node)
  {
    enum apivalue_btree direction;
    unsigned char downlink_count;
    struct btree_node * less;
    struct btree_node * more;
    struct btree_node * next;
    enum apivalue_btree opposite;
    struct btree_node * replacement;
    enum apivalue_btree rv;
    struct btree_node * up;

    if (api == NULL || btree == NULL || btree_node == NULL)
      return apivalue_btree_error_null_argument;

    less = btree_node->links[apivalue_btree_direction_less];
    more = btree_node->links[apivalue_btree_direction_more];
    up = btree_node->links[apivalue_btree_direction_up];

    /* If the node is at the top, verify that the tree agrees, since it'll be mutated */
    if (up == NULL && btree->root != btree_node)
      return apivalue_btree_error_wrong_tree;

    /* Pick a replacement */
    downlink_count = 0;
    if (less != NULL)
      ++downlink_count;
    if (more != NULL)
      ++downlink_count;
    switch (downlink_count)
      {
        case 0:
        /* Very simple */
        replacement = NULL;
        break;

        case 1:
        /* Not very difficult */
        if (less)
          {
            replacement = less;
            btree_node->links[apivalue_btree_direction_less] = NULL;
          }
          else
          {
            replacement = more;
            btree_node->links[apivalue_btree_direction_more] = NULL;
          }
        break;

        case 2:
        /* Pick from the choice stored in the binary tree */
        if (btree->last_direction == apivalue_btree_direction_less)
          {
            direction = apivalue_btree_direction_less;
            opposite = apivalue_btree_direction_more;
          }
          else
          {
            direction = apivalue_btree_direction_more;
            opposite = apivalue_btree_direction_less;
          }
        /* Move down in the direction, then all the way down in the opposite direction */
        for (next = btree_node->links[direction]; next != NULL; next = next->links[opposite])
          replacement = next;
        /* "Delete" (detach) the replacement, which will have its own case 0 or 1, but not 2 */
        rv = api->delete(api, btree, replacement);
        if (rv != apivalue_btree_success)
          /* This should not be possible, but pass the error along */
          return rv;
        /* Otherwise, the replacement is now available */
        replacement->links[opposite] = btree_node->links[opposite];
        replacement->links[opposite]->links[apivalue_btree_direction_up] = replacement;
        btree_node->links[opposite] = NULL;
        /* Note that this could have just changed to NULL, but that's acceptable */
        replacement->links[direction] = btree_node->links[direction];
        if (replacement->links[direction] != NULL)
          replacement->links[direction]->links[apivalue_btree_direction_up] = replacement;
        btree_node->links[direction] = NULL;
        break;

        default:
        return apivalue_btree_error_unexpected_branch;
      }
    /* Upper management */
    if (up == NULL)
      {
        /* The replacement is the new root */
        btree->root = replacement;
        return apivalue_btree_success;
      }
    /* Otherwise, fasten the upper and the replacement together */
    if (up->links[apivalue_btree_direction_less] == btree_node)
      up->links[apivalue_btree_direction_less] = replacement;
      else
      up->links[apivalue_btree_direction_more] = replacement;
    if (replacement != NULL)
      replacement->links[apivalue_btree_direction_up] = up;
    btree_node->links[apivalue_btree_direction_up] = NULL;
    return apivalue_btree_success;
  }

static enum apivalue_btree btree_find_or_insert(struct api_btree * api, struct btree_node * key, struct btree * btree, apifunction_btree_compare * compare, enum apivalue_btree insertion_mode, struct btree_node ** old_btree_node)
  {
    struct btree_node * btree_node;
    int compare_rv;
    enum apivalue_btree direction;
    enum apivalue_btree i;
    struct btree_node * upper;

    if (api == NULL || key == NULL)
      return apivalue_btree_error_null_argument;
    if (insertion_mode >= apivalue_btree_insertion_modes)
      return apivalue_btree_error_invalid_mode;

    /* Get as close as possible */
    direction = apivalue_btree_directions;
    upper = NULL;
    for (btree_node = btree->root; btree_node != NULL; btree_node = upper->links[direction])
      {
        compare_rv = compare(api, btree, key, btree_node);
        if (compare_rv == 0)
          break;
        upper = btree_node;
        if (compare_rv < 0)
          direction = apivalue_btree_direction_less;
          else
          direction = apivalue_btree_direction_more;
      }
    /* If the caller cares, tell them the match or non-match status */
    if (old_btree_node != NULL)
      *old_btree_node = btree_node;
    /* Are we finished? */
    if (insertion_mode == apivalue_btree_insertion_none)
      return apivalue_btree_success;
    if (btree_node == NULL && insertion_mode == apivalue_btree_insertion_replace_only_if_found)
      return apivalue_btree_success;
    if (btree_node != NULL && insertion_mode == apivalue_btree_insertion_only_if_not_found)
      return apivalue_btree_success;
    /* Otherwise, the key will be inserted, so check it */
    for (i = 0; i < apivalue_btree_directions; ++i)
      {
        if (key->links[i] != NULL)
          return apivalue_btree_error_uninitialized_argument;
      }
    /* Check for replacement */
    if (btree_node != NULL)
      {
        /* Swap links */
        for (i = 0; i < apivalue_btree_directions; ++i)
          key->links[i] = btree_node->links[i];
        api->initialize_node(api, btree_node);
        /* And downward nodes' upward links */
        for (i = 1; i < apivalue_btree_directions; ++i)
          {
            if (key->links[i] == NULL)
              continue;
            key->links[i]->links[apivalue_btree_direction_up] = key;
          }
      }
    /* Check for root node */
    if (upper == NULL)
      {
        /* This is the root node */
        btree->root = key;
        return apivalue_btree_success;
      }
    /* Otherwise, attach it to the tree */
    upper->links[direction] = key;
    key->links[apivalue_btree_direction_up] = upper;
    /* Arbitrary choice of breaking ties during deletions */
    btree->last_direction = direction;
    return apivalue_btree_success;
  }

enum apivalue_btree api_btree_initialize(struct api_btree * api)
  {
    *api = api_btree_defaults;
    return apivalue_btree_success;
  }

static void btree_initialize(struct api_btree * api, struct btree * btree)
  {
    (void) api;

    btree->root = NULL;
    btree->last_direction = apivalue_btree_direction_less;
  }

static void btree_initialize_node(struct api_btree * api, struct btree_node * btree_node)
  {
    enum apivalue_btree i;

    (void) api;

    for (i = 0; i < apivalue_btree_directions; ++i)
      btree_node->links[i] = NULL;
  }

static struct btree_node * btree_ordered_visit(struct api_btree * api, struct btree * btree, struct btree_node * btree_node, enum apivalue_btree direction)
  {
    struct btree_node * next;
    enum apivalue_btree opposite;

    (void) api;

    if (direction != apivalue_btree_direction_less && direction != apivalue_btree_direction_more)
      return NULL;

    if (direction == apivalue_btree_direction_more)
      opposite = apivalue_btree_direction_less;
      else
      opposite = apivalue_btree_direction_more;

    if (btree_node == NULL)
      {
        for (next = btree->root; next != NULL; next = next->links[opposite])
          btree_node = next;
        return btree_node;
      }
    /* Try down and in the specified direction, then all the way down and in the opposite direction */
    if (btree_node->links[direction] != NULL)
      {
        for (next = btree_node->links[direction]; next != NULL; next = next->links[opposite])
          btree_node = next;
        return btree_node;
      }
    /* Navigate upward until becoming a node being the opposite direction of its upper (or if this node is already one such) */
    while (btree_node->links[apivalue_btree_direction_up] != NULL)
      {
        if (btree_node->links[apivalue_btree_direction_up]->links[opposite] == btree_node)
          /* Up is greater */
          return btree_node->links[apivalue_btree_direction_up];
        /* Otherwise, move up */
        btree_node = btree_node->links[apivalue_btree_direction_up];
      }
    /* At the root, with nothing greater */
    return NULL;
  }
