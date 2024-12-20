/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include "builtins.h"
#include "command.h"
#include "toy.h"
#include "toydef.h"
#include "toyio.h"
#include "toylib.h"
#include "list.h"
#include "module.h"

struct cmd_help;

struct cmd_help
  {
    struct command command;
    struct top * ctx;
  };

static apifunction_command cmd_help;
static func_module_event module_event;

static struct command command_help;
static struct live_module * live_module;

#if BUILTIN_CMD_HELP
union module builtin_module_cmd_help =
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
      "cmd_help"
    }
  };

static struct command command_help =
  {
    NULL,
    "help",
    &cmd_help,
    {
      NULL,
      NULL
    }
  };

static int cmd_help(struct api_command * api, struct command * command, int argc, char ** argv)
  {
    struct cmd_help * cmd;
    struct top * ctx;
    struct list_item * list_item;

    (void) api;
    (void) argc;
    (void) argv;

    cmd = type_with_member_at_ptr(struct cmd_help, command, command);
    ctx = cmd->ctx;

    (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Available commands are:\n");
    for (list_item = ctx->api_command->commands.head.next; list_item != &ctx->api_command->commands.head; list_item = list_item->next)
      {
        /* We might as well repurpose 'command' */
        command = type_with_member_at_ptr(struct command, list_item, list_item);
        (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "  '%s' from the '%s' module\n", command->name, command->live_module->module.v1.nice_name);
      }
    return EXIT_SUCCESS;
  }

static int module_event(enum apivalue_module_event_type type, void * event_data)
  {
    struct cmd_help * command;
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
            (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Out of memory while registering 'help' command\n");
            return EXIT_FAILURE;
          }
        live_module->module.v1.module_pointers[0] = command;
        command->command = command_help;
        command->command.live_module = live_module;
        command->ctx = ctx;
        (void) ctx->api_list->initialize_list_item(ctx->api_list, &command->command.list_item);
        rv = ctx->api_command->add(ctx->api_command, &command->command);
        if (rv != EXIT_SUCCESS)
          (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Error '%d' while attempting to register 'help' command\n", rv);
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
