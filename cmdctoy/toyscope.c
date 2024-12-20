/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#include <ctype.h>
#include <stddef.h>
#include <string.h>
#include "btree.h"
#include "toydef.h"
#include "toylib.h"
#include "toyscope.h"
#include "type.h"

static struct api_toy_scope api_toy_scope_defaults;

static apifunction_toy_scope_add_identifier_to_scope toy_scope_add_identifier_to_scope;
static apifunction_toy_scope_allocate_identifier toy_scope_allocate_identifier;
static apifunction_btree_compare toy_scope_compare_identifiers;
static apifunction_toy_scope_find_identifier_in_scope toy_scope_find_identifier_in_scope;
static apifunction_toy_scope_find_identifier_in_scope_chain toy_scope_find_identifier_in_scope_chain;
static apifunction_toy_scope_grow_allocated_chain toy_scope_grow_allocated_chain;
static apifunction_toy_scope_initialize_identifier toy_scope_initialize_identifier;
static apifunction_toy_scope_initialize_scope toy_scope_initialize_scope;
static apifunction_toy_scope_remove_identifier_from_scope toy_scope_remove_identifier_from_scope;

static struct api_toy_scope api_toy_scope_defaults =
  {
    NULL,
    NULL,
    &api_toy_scope_initialize,
    &toy_scope_add_identifier_to_scope,
    &toy_scope_allocate_identifier,
    &toy_scope_find_identifier_in_scope,
    &toy_scope_find_identifier_in_scope_chain,
    &toy_scope_grow_allocated_chain,
    &toy_scope_initialize_identifier,
    &toy_scope_initialize_scope,
    &toy_scope_remove_identifier_from_scope
  };

enum apivalue_toy_scope api_toy_scope_initialize(struct api_toy_scope * api)
  {
    struct api_btree * btree_api;
    struct api_stdlib * stdlib_api;

    btree_api = api->api_btree;
    stdlib_api = api->api_stdlib;
    if (btree_api == NULL || stdlib_api == NULL)
      return apivalue_toy_scope_error_null_argument;
    *api = api_toy_scope_defaults;
    api->api_btree = btree_api;
    api->api_stdlib = stdlib_api;
    return apivalue_toy_scope_success;
  }

static enum apivalue_toy_scope toy_scope_add_identifier_to_scope(struct api_toy_scope * api, struct toy_scope_identifier * identifier, struct toy_scope * scope)
  {
    struct api_btree * btree_api;
    enum apivalue_btree btree_rv;
    struct btree_node * old_btree_node;
    struct toy_scope_identifier * old_identifier;
    struct api_stdlib * stdlib_api;

    if (identifier == NULL || scope == NULL)
      return apivalue_toy_scope_error_null_argument;

    btree_api = api->api_btree;
    stdlib_api = api->api_stdlib;

    btree_rv = btree_api->find_or_insert(btree_api, &identifier->btree_node, &scope->btree, &toy_scope_compare_identifiers, apivalue_btree_insertion_always, &old_btree_node);
    if (btree_rv != apivalue_btree_success)
      return apivalue_toy_scope_error_btree_api;
    if (old_btree_node != NULL)
      {
        old_identifier = type_with_member_at_ptr(struct toy_scope_identifier, btree_node, old_btree_node);
        if (old_identifier->auto_free == 1)
          stdlib_api->free(stdlib_api, old_identifier);
      }
    return apivalue_toy_scope_success;
  }

static enum apivalue_toy_scope toy_scope_allocate_identifier(struct api_toy_scope * api, struct toy_scope_identifier ** identifier, char * name, struct type * type)
  {
    size_t alignment;
    struct api_btree * btree_api;
    char * mem;
    size_t name_size;
    struct toy_scope_identifier * new_identifier;
    struct type_object * object_type;
    size_t padding;
    struct api_stdlib * stdlib_api;
    size_t value_size;

    if (identifier == NULL || name == NULL)
      return apivalue_toy_scope_error_null_argument;
    if (name[0] == '\0')
      return apivalue_toy_scope_error_invalid_name;
    for (name_size = 0; name[name_size] != '\0'; ++name_size)
      {
        if (isspace(name[name_size]))
          return apivalue_toy_scope_error_invalid_name;
      }
    /* Add terminator */
    ++name_size;
    if (type != NULL && type->partition == apivalue_type_partition_object)
      {
        object_type = (void *) type;
        alignment = object_type->alignment;
        if (alignment > 0)
          padding = (sizeof **identifier + name_size + alignment) % alignment;
          else
          padding = 0;
        value_size = object_type->size;
      }
      else
      {
        padding = 0;
        value_size = 0;
      }
    stdlib_api = api->api_stdlib;
    mem = stdlib_api->malloc(stdlib_api, sizeof *new_identifier + name_size + padding + value_size);
    if (mem == NULL)
      return apivalue_toy_scope_error_out_of_memory;
    new_identifier = (void *) mem;
    btree_api = api->api_btree;
    btree_api->initialize_node(btree_api, &new_identifier->btree_node);
    new_identifier->name = mem + sizeof *new_identifier;
    (void) memcpy(new_identifier->name, name, name_size);
    new_identifier->type = type;
    if (value_size > 0)
      new_identifier->value = (void *) (mem + sizeof *new_identifier + name_size + padding);
      else
      new_identifier->value = NULL;
    new_identifier->auto_free = 1;
    *identifier = new_identifier;
    return apivalue_toy_scope_success;
  }

static int toy_scope_compare_identifiers(struct api_btree * api, struct btree * btree, struct btree_node * btree_node_a, struct btree_node * btree_node_b)
  {
    struct toy_scope_identifier * identifier_a;
    struct toy_scope_identifier * identifier_b;
    int strcmp_rv;

    (void) api;
    (void) btree;

    identifier_a = type_with_member_at_ptr(struct toy_scope_identifier, btree_node, btree_node_a);
    identifier_b = type_with_member_at_ptr(struct toy_scope_identifier, btree_node, btree_node_b);
    strcmp_rv = strcmp(identifier_a->name, identifier_b->name);
    return strcmp_rv;
  }

static enum apivalue_toy_scope toy_scope_find_identifier_in_scope(struct api_toy_scope * api, struct toy_scope_identifier ** identifier, char * name, struct toy_scope * scope)
  {
    struct api_btree * btree_api;
    struct btree_node * btree_node;
    enum apivalue_btree btree_rv;
    struct toy_scope_identifier search;

    if (identifier == NULL || name == NULL || scope == NULL)
      return apivalue_toy_scope_error_null_argument;

    btree_api = api->api_btree;

    api->initialize_identifier(api, &search);
    search.name = name;
    btree_rv = btree_api->find_or_insert(btree_api, &search.btree_node, &scope->btree, &toy_scope_compare_identifiers, apivalue_btree_insertion_none, &btree_node);
    if (btree_rv != apivalue_btree_success)
      return apivalue_toy_scope_error_btree_api;
    if (btree_node == NULL)
      return apivalue_toy_scope_error_not_found;
    *identifier = type_with_member_at_ptr(struct toy_scope_identifier, btree_node, btree_node);
    return apivalue_toy_scope_success;
  }

static enum apivalue_toy_scope toy_scope_find_identifier_in_scope_chain(struct api_toy_scope * api, struct toy_scope_identifier ** identifier, char * name, struct toy_scope_chain * chain)
  {
    size_t i;
    enum apivalue_toy_scope rv;
    struct toy_scope * scope;
    struct toy_scope_identifier search;

    if (identifier == NULL || name == NULL || chain == NULL)
      return apivalue_toy_scope_error_null_argument;

    api->initialize_identifier(api, &search);
    search.name = name;
    for (i = chain->count; i > 0; --i)
      {
        scope = chain->scopes[i - 1];
        rv = api->find_identifier_in_scope(api, identifier, name, scope);
        if (rv == apivalue_toy_scope_success)
          return apivalue_toy_scope_success;
      }
    return apivalue_toy_scope_error_not_found;
  }

static enum apivalue_toy_scope toy_scope_grow_allocated_chain(struct api_toy_scope * api, struct toy_scope_chain ** chain, size_t new_count)
  {
    unsigned char * mem;
    struct toy_scope_chain * new_chain;
    struct api_stdlib * stdlib_api;

    if (chain == NULL)
      return apivalue_toy_scope_error_null_argument;
    if (new_count == 0)
      return apivalue_toy_scope_error_invalid_count;
    stdlib_api = api->api_stdlib;
    mem = stdlib_api->realloc(stdlib_api, *chain, sizeof (struct toy_scope_chain_alignment) + (new_count - 1) * sizeof *(*chain)->scopes);
    if (mem == NULL)
      return apivalue_toy_scope_error_out_of_memory;
    new_chain = (void *) mem;
    new_chain->count = new_count;
    new_chain->scopes = (void *) (mem + offsetof(struct toy_scope_chain_alignment, scopes));
    *chain = new_chain;
    return apivalue_toy_scope_success;
  }

static void toy_scope_initialize_identifier(struct api_toy_scope * api, struct toy_scope_identifier * identifier)
  {
    struct api_btree * btree_api;

    btree_api = api->api_btree;
    btree_api->initialize_node(btree_api, &identifier->btree_node);
    identifier->name = NULL;
    identifier->type = NULL;
    identifier->value = NULL;
    identifier->auto_free = 0;
  }

static void toy_scope_initialize_scope(struct api_toy_scope * api, struct toy_scope * scope)
  {
    struct api_btree * btree_api;

    btree_api = api->api_btree;
    btree_api->initialize(btree_api, &scope->btree);
  }

static enum apivalue_toy_scope toy_scope_remove_identifier_from_scope(struct api_toy_scope * api, struct toy_scope_identifier * identifier, struct toy_scope * scope)
  {
    struct api_btree * btree_api;
    enum apivalue_btree btree_rv;

    btree_api = api->api_btree;
    btree_rv = btree_api->delete(btree_api, &scope->btree, &identifier->btree_node);
    if (btree_rv != apivalue_btree_success)
      return apivalue_toy_scope_error_not_found;
    /* Unlike a replacement during addition, the caller must free the identifier, if appropriate */
    return apivalue_toy_scope_success;
  }
