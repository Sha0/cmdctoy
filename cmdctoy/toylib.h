/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#ifndef INC_CMDCTOY_STDLIB
#define INC_CMDCTOY_STDLIB

#include <stddef.h>

enum apivalue_stdlib
  {
    apivalue_stdlib_success,
    apivalue_stdlib_zero = 0
  };

struct api_stdlib;

typedef enum apivalue_stdlib apifunction_stdlib_api_initialize(struct api_stdlib *);
typedef void apifunction_stdlib_free(struct api_stdlib *, void *);
typedef void * apifunction_stdlib_malloc(struct api_stdlib *, size_t);
typedef void * apifunction_stdlib_realloc(struct api_stdlib *, void *, size_t);

extern apifunction_stdlib_api_initialize api_stdlib_initialize;

struct api_stdlib
  {
    apifunction_stdlib_api_initialize * api_initialize;
    apifunction_stdlib_free * free;
    apifunction_stdlib_malloc * malloc;
    apifunction_stdlib_realloc * realloc;
  };

#endif /* INC_CMDCTOY_STDLIB */
