/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#ifndef INC_TOY_SCOPE
#define INC_TOY_SCOPE

#include <stddef.h>
#include "btree.h"
#include "type.h"

enum apivalue_toy_scope
  {
    apivalue_toy_scope_success,
    apivalue_toy_scope_error_btree_api,
    apivalue_toy_scope_error_invalid_count,
    apivalue_toy_scope_error_invalid_name,
    apivalue_toy_scope_error_not_found,
    apivalue_toy_scope_error_null_argument,
    apivalue_toy_scope_error_out_of_memory,
    apivalue_toy_scope_zero = 0
  };

struct api_toy_scope;
struct toy_scope;
struct toy_scope_chain;
struct toy_scope_chain_alignment;
struct toy_scope_identifier;

typedef enum apivalue_toy_scope apifunction_toy_scope_add_identifier_to_scope(struct api_toy_scope *, struct toy_scope_identifier *, struct toy_scope *);
typedef enum apivalue_toy_scope apifunction_toy_scope_allocate_identifier(struct api_toy_scope *, struct toy_scope_identifier **, char *, struct type *);
typedef enum apivalue_toy_scope apifunction_toy_scope_api_initialize(struct api_toy_scope *);
typedef enum apivalue_toy_scope apifunction_toy_scope_find_identifier_in_scope(struct api_toy_scope *, struct toy_scope_identifier **, char *, struct toy_scope *);
typedef enum apivalue_toy_scope apifunction_toy_scope_find_identifier_in_scope_chain(struct api_toy_scope *, struct toy_scope_identifier **, char *, struct toy_scope_chain *);
typedef enum apivalue_toy_scope apifunction_toy_scope_grow_allocated_chain(struct api_toy_scope *, struct toy_scope_chain **, size_t);
typedef void apifunction_toy_scope_initialize_identifier(struct api_toy_scope *, struct toy_scope_identifier *);
typedef void apifunction_toy_scope_initialize_scope(struct api_toy_scope *, struct toy_scope *);
typedef enum apivalue_toy_scope apifunction_toy_scope_remove_identifier_from_scope(struct api_toy_scope *, struct toy_scope_identifier *, struct toy_scope *);

extern apifunction_toy_scope_api_initialize api_toy_scope_initialize;

struct api_toy_scope
  {
    struct api_btree * api_btree;
    struct api_stdlib * api_stdlib;
    apifunction_toy_scope_api_initialize * api_initialize;
    apifunction_toy_scope_add_identifier_to_scope * add_identifier_to_scope;
    apifunction_toy_scope_allocate_identifier * allocate_identifier;
    apifunction_toy_scope_find_identifier_in_scope * find_identifier_in_scope;
    apifunction_toy_scope_find_identifier_in_scope_chain * find_identifier_in_scope_chain;
    apifunction_toy_scope_grow_allocated_chain * grow_allocated_chain;
    apifunction_toy_scope_initialize_identifier * initialize_identifier;
    apifunction_toy_scope_initialize_scope * initialize_scope;
    apifunction_toy_scope_remove_identifier_from_scope * remove_identifier_from_scope;
  };

struct toy_scope
  {
    struct btree btree;
  };

struct toy_scope_chain
  {
    size_t count;
    struct toy_scope ** scopes;
  };

/* Not intended for use outside of sizeof and offsetof */
struct toy_scope_chain_alignment
  {
    struct toy_scope_chain chain;
    struct toy_scope * scopes[1];
  };

struct toy_scope_identifier
  {
    struct btree_node btree_node;
    char * name;
    struct type * type;
    void * value;
    char auto_free;
  };

#endif /* INC_TOY_SCOPE */
