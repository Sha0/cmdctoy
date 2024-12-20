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
#include "module.h"

struct cmd_exit;

struct cmd_exit
  {
    struct command command;
    struct top * ctx;
  };

static apifunction_command cmd_exit;
static func_module_event module_event;

static struct command command_exit;
static struct live_module * live_module;

#if BUILTIN_CMD_EXIT
union module builtin_module_cmd_exit =
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
      "cmd_exit"
    }
  };

static struct command command_exit =
  {
    NULL,
    "exit",
    &cmd_exit,
    {
      NULL,
      NULL
    }
  };

static int cmd_exit(struct api_command * api, struct command * command, int argc, char ** argv)
  {
    struct cmd_exit * cmd;
    struct top * ctx;

    (void) api;
    (void) argc;
    (void) argv;

    cmd = type_with_member_at_ptr(struct cmd_exit, command, command);
    ctx = cmd->ctx;
    (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "The program is exiting.  Good day to you.\n");
    ctx->request_shutdown(ctx);
    return EXIT_SUCCESS;
  }

static int module_event(enum apivalue_module_event_type type, void * event_data)
  {
    struct cmd_exit * command;
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
            (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Out of memory while registering 'exit' command\n");
            return EXIT_FAILURE;
          }
        live_module->module.v1.module_pointers[0] = command;
        command->command = command_exit;
        command->command.live_module = live_module;
        command->ctx = ctx;
        (void) ctx->api_list->initialize_list_item(ctx->api_list, &command->command.list_item);
        rv = ctx->api_command->add(ctx->api_command, &command->command);
        if (rv != EXIT_SUCCESS)
          (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Error '%d' while attempting to register 'exit' command\n", rv);
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
