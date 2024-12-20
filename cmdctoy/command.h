/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#ifndef INC_COMMAND
#define INC_COMMAND

#include <stddef.h>
#include "list.h"

enum apivalue_command
  {
    apivalue_command_success,
    apivalue_command_error_null_argument,
    apivalue_command_max_argc = 11,
    apivalue_command_zero = 0
  };

struct command;
struct api_command;

typedef int apifunction_command(struct api_command *, struct command *, int, char **);
typedef int apifunction_command_add(struct api_command *, struct command *);
typedef enum apivalue_command apifunction_command_api_initialize(struct api_command *);
typedef int apifunction_command_line(struct api_command *, char *, size_t);
typedef int apifunction_command_remove(struct api_command *, struct command *);

extern apifunction_command_api_initialize api_command_initialize;

struct command
  {
    struct live_module * live_module;
    const char * name;
    apifunction_command * handler;
    struct list_item list_item;
  };

struct api_command
  {
    struct api_list * api_list;
    struct api_stdio * api_stdio;
    apifunction_command_api_initialize * api_initialize;
    apifunction_command_add * add;
    apifunction_command_line * line;
    apifunction_command_remove * remove;
    struct list commands;
  };

#endif /* INC_COMMAND */
