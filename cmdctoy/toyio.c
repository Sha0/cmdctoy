/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#include <stdarg.h>
#include <stdio.h>
#include "toyio.h"

static apifunction_stdio_feof stdio_feof;
static apifunction_stdio_ferror stdio_ferror;
static apifunction_stdio_fgetc stdio_fgetc;
static apifunction_stdio_fprintf stdio_fprintf;
static apifunction_stdio_ungetc stdio_ungetc;

static struct api_stdio api_stdio_defaults =
  {
    &api_stdio_initialize,
    &stdio_feof,
    &stdio_ferror,
    &stdio_fgetc,
    &stdio_fprintf,
    &stdio_ungetc
  };

enum apivalue_stdio api_stdio_initialize(struct api_stdio * api)
  {
    *api = api_stdio_defaults;
    return apivalue_stdio_success;
  }

static int stdio_feof(struct api_stdio * api, FILE * stream)
  {
    (void) api;

    return feof(stream);
  }

static int stdio_ferror(struct api_stdio * api, FILE * stream)
  {
    (void) api;

    return ferror(stream);
  }

static int stdio_fgetc(struct api_stdio * api, FILE * stream)
  {
    (void) api;

    return fgetc(stream);
  }

static int stdio_fprintf(struct api_stdio * api, FILE * stream, const char * format, ...)
  {
    va_list ap;
    int rv;

    (void) api;

    va_start(ap, format);
    rv = vfprintf(stream, format, ap);
    va_end(ap);
    return rv;
  }

static int stdio_ungetc(struct api_stdio * api, int c, FILE * stream)
  {
    (void) api;

    return ungetc(c, stream);
  }
