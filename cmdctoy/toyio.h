/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#ifndef INC_CMDCTOY_STDIO
#define INC_CMDCTOY_STDIO

#include <stdio.h>

enum apivalue_stdio
  {
    apivalue_stdio_success,
    apivalue_stdio_stdio_zero = 0
  };

struct api_stdio;

typedef enum apivalue_stdio apifunction_stdio_api_initialize(struct api_stdio *);
typedef int apifunction_stdio_feof(struct api_stdio *, FILE *);
typedef int apifunction_stdio_ferror(struct api_stdio *, FILE *);
typedef int apifunction_stdio_fgetc(struct api_stdio *, FILE *);
typedef int apifunction_stdio_fprintf(struct api_stdio *, FILE *, const char *, ...);
typedef int apifunction_stdio_ungetc(struct api_stdio *, int, FILE *);

extern apifunction_stdio_api_initialize api_stdio_initialize;

struct api_stdio
  {
    apifunction_stdio_api_initialize * api_initialize;
    /* The underscore is someWat of a hack for compilers that make feof and ferror macros */
    apifunction_stdio_feof * f_eof;
    apifunction_stdio_ferror * f_error;
    apifunction_stdio_fgetc * fgetc;
    apifunction_stdio_fprintf * fprintf;
    apifunction_stdio_ungetc * ungetc;
  };

#endif /* INC_CMDCTOY_STDIO */
