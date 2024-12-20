/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#include <stdlib.h>
#include "toylib.h"

static apifunction_stdlib_free stdlib_free;
static apifunction_stdlib_malloc stdlib_malloc;
static apifunction_stdlib_realloc stdlib_realloc;

static struct api_stdlib api_stdlib_defaults =
  {
    &api_stdlib_initialize,
    &stdlib_free,
    &stdlib_malloc,
    &stdlib_realloc
  };

static void stdlib_free(struct api_stdlib * api, void * ptr)
  {
    (void) api;

    free(ptr);
  }

enum apivalue_stdlib api_stdlib_initialize(struct api_stdlib * api)
  {
    *api = api_stdlib_defaults;
    return apivalue_stdlib_success;
  }

static void * stdlib_malloc(struct api_stdlib * api, size_t size)
  {
    (void) api;

    return malloc(size);
  }

static void * stdlib_realloc(struct api_stdlib * api, void * ptr, size_t size)
  {
    (void) api;

    return realloc(ptr, size);
  }
