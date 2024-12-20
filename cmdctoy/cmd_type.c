/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "builtins.h"
#include "command.h"
#include "toy.h"
#include "toydef.h"
#include "toyio.h"
#include "toylib.h"
#include "list.h"
#include "module.h"
#include "type.h"

struct cmd_type;

struct cmd_type
  {
    struct command command;
    struct top * ctx;
  };

static apifunction_command cmd_typedump;
static int dump_type(struct top *, struct type *, size_t);
static func_module_event module_event;

static struct command command_type;
static struct live_module * live_module;

#if BUILTIN_CMD_TYPE
union module builtin_module_cmd_type =
#else
union module module =
#endif
  {
    {
      {
        module_signature,
        "2024120200",
        1
      },
      &module_event,
      {
        NULL,
        NULL,
        NULL,
        NULL
      },
      "cmd_type"
    }
  };

static struct command command_type =
  {
    NULL,
    "typedump",
    &cmd_typedump,
    {
      NULL,
      NULL
    }
  };

static int cmd_typedump(struct api_command * api, struct command * command, int argc, char ** argv)
  {
    struct cmd_type * cmd;
    struct top * ctx;
    char * endptr;
    size_t i;
    int new_errno;
    int old_errno;
    struct type * type;
    struct api_type * type_api;
    size_t type_count;
    static const char usage[] =
      "Usage:\n"
      "  typedump TYPE  Show detail about TYPE\n"
      "Notes:\n"
      "  TYPE can be either a type-name (if the type-name has no spaces)\n"
      "  or an 'unsigned long int' number (without any octothorpe).\n"
      ;
    unsigned long int which;

    (void) api;
    (void) argv;

    cmd = type_with_member_at_ptr(struct cmd_type, command, command);
    ctx = cmd->ctx;
    type_api = ctx->api_type;
    type_count = type_api->type_count;

    if (argc != 2)
      {
        (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "%s", usage);
        (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Known types are:\n");
        for (i = 0; i < type_count; ++i)
          {
            type = type_api->types[i].type;
            (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "  #%lu '%s'\n", (unsigned long int) i, type->nice_name);
          }
        return EXIT_FAILURE;
      }
    /* Otherwise, search for the type */
    for (i = 0; i < type_count; ++i)
      {
        type = type_api->types[i].type;
        if (strcmp(argv[1], type->nice_name) == 0)
          break;
      }
    if (i == type_count)
      {
        /* Try as a number */
        old_errno = errno;
        errno = 0;
        which = strtoul(argv[1], &endptr, 0);
        new_errno = errno;
        errno = old_errno;
        if (new_errno != 0)
          {
            (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Unrecognized 'unsigned long int' format for TYPE, with error '%d' and message '%s'\n", new_errno, strerror(new_errno));
            return new_errno;
          }
        if (*endptr != '\0')
          {
            (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Unrecognized 'unsigned long int' format for TYPE\n");
            return EXIT_FAILURE;
          }
        if (which >= type_count)
          {
            (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Unknown type #%lu\n", which);
            return EXIT_FAILURE;
          }
        type = type_api->types[(size_t) which].type;
      }
    return dump_type(ctx, type, 0);
  }

static int dump_type(struct top * ctx, struct type * type, size_t indentation)
  {
    struct type_struct * full_type;
    struct type * full_type_inner;
    struct type_object * full_type_object;
    size_t i;
    size_t j;
    struct type_struct_member * member;
    struct type_object * member_object_type;
    size_t member_size;
    struct api_type * type_api;

    (void) indentation;

    type_api = ctx->api_type;
    full_type_inner = type_api->fulltype_of_type(type_api, type);
    if (full_type_inner == NULL)
      {
        (void) ctx->api_stdio->fprintf(ctx->api_stdio, stdout, "Unable to discover detail about the type\n", (unsigned int) type->partition);
        return EXIT_FAILURE;
      }
    /* Otherwise, we now have a structure about structure */
    full_type_object = type_with_member_at_ptr(struct type_object, type, full_type_inner);
    full_type = type_with_member_at_ptr(struct type_struct, object, full_type_object);
    (void) ctx->api_stdio->fprintf(ctx->api_stdio, stdout, "Type partition is '%u'\n", (unsigned int) type->partition);
    (void) ctx->api_stdio->fprintf(ctx->api_stdio, stdout, "Nice name for type is '%s'\n", type->nice_name);
    for (i = 0; i < full_type->member_count; ++i)
      {
        member = full_type->members + i;
        member_object_type = member->type;
        member_size = member_object_type->size;
        (void) ctx->api_stdio->fprintf(ctx->api_stdio, stdout, "Type has a property '%s' of type '%s' and value-bytes: L", member->name, member_object_type->type.nice_name);
        for (j = 0; j < member_size; ++j)
          (void) ctx->api_stdio->fprintf(ctx->api_stdio, stdout, " 0x%02X", (((unsigned char *) type) + member->offset)[j]);
        (void) ctx->api_stdio->fprintf(ctx->api_stdio, stdout, " H\n");
      }
    return EXIT_SUCCESS;
  }

static int module_event(enum apivalue_module_event_type type, void * event_data)
  {
    struct cmd_type * command;
    struct top * ctx;
    int rv;

    switch (type)
      {
        case apivalue_module_event_type_loaded:
        if (live_module != NULL)
          return EXIT_FAILURE;
        live_module = event_data;
        return EXIT_SUCCESS;

        case apivalue_module_event_type_thread_started:
        ctx = event_data;
        command = ctx->api_stdlib->malloc(ctx->api_stdlib, sizeof *command);
        if (command == NULL)
          {
            (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Out of memory while registering 'typedump' command\n");
            return EXIT_FAILURE;
          }
        live_module->module.v1.module_pointers[0] = command;
        command->command = command_type;
        command->command.live_module = live_module;
        command->ctx = ctx;
        (void) ctx->api_list->initialize_list_item(ctx->api_list, &command->command.list_item);
        rv = ctx->api_command->add(ctx->api_command, &command->command);
        if (rv != EXIT_SUCCESS)
          (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Error '%d' while attempting to register 'typedump' command\n", rv);
        return rv;

        case apivalue_module_event_type_thread_stop_requested:
        case apivalue_module_event_type_thread_stopped:
        case apivalue_module_event_type_unload_requested:
        return EXIT_SUCCESS;

        case apivalue_module_event_type_unload:
        /* Assume success until proven otherwise */
        rv = EXIT_SUCCESS;
        command = live_module->module.v1.module_pointers[0];
        if (command != NULL)
          {
            ctx = command->ctx;
            rv = ctx->api_command->remove(ctx->api_command, &command->command);
            if (rv == EXIT_SUCCESS)
              ctx->api_stdlib->free(ctx->api_stdlib, command);
          }
        live_module = NULL;
        return rv;
      }
    return EXIT_FAILURE;
  }
