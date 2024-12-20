/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "command.h"
#include "toyio.h"
#include "toydef.h"
#include "list.h"

static struct api_command api_command_defaults;

static apifunction_command_add add_command;
static apifunction_command_line command_line;
static apifunction_command_remove remove_command;

static struct api_command api_command_defaults =
  {
    NULL,
    NULL,
    &api_command_initialize,
    &add_command,
    &command_line,
    &remove_command,
    {
      {
        NULL,
        NULL
      }
    }
  };

static int add_command(struct api_command * api, struct command * command)
  {
    (void) api->api_list->add_item_to_list_head(api->api_list, &command->list_item, &api->commands);
    return EXIT_SUCCESS;
  }

static int command_line(struct api_command * api, char * cmd, size_t cmd_len)
  {
    int argc;
    char * argv[apivalue_command_max_argc + 1];
    struct command * command;
    size_t i;
    struct list_item * list_item;
    int rv;

    if (cmd[cmd_len] != '\0')
      {
        (void) api->api_stdio->fprintf(api->api_stdio, stderr, "Command not null-terminated, so it has been ignored\n");
        return EXIT_FAILURE;
      }
    argc = 0;
    for (i = 0; i < cmd_len; ++i)
      {
        while (isspace(cmd[i]))
          ++i;
        if (cmd[i] == '\0')
          break;
        if (argc == apivalue_command_max_argc)
          {
            (void) api->api_stdio->fprintf(api->api_stdio, stderr, "More than %d arguments for command, so it has been ignored\n", apivalue_command_max_argc);
            return EXIT_FAILURE;
          }
        argv[argc] = cmd + i;
        ++argc;
        while (!isspace(cmd[i]) && cmd[i] != '\0')
          ++i;
        if (cmd[i] == '\0')
          break;
        cmd[i] = '\0';
      }
    if (argc == 0)
      return EXIT_SUCCESS;
    argv[argc] = NULL;

    for (list_item = api->commands.head.next; list_item != &api->commands.head; list_item = list_item->next)
      {
        command = type_with_member_at_ptr(struct command, list_item, list_item);
        if (strcmp(argv[0], command->name) != 0)
          continue;
        /* A match */
        rv = command->handler(api, command, argc, argv);
        return rv;
      }
    (void) api->api_stdio->fprintf(api->api_stdio, stderr, "Awful command or file name\n");
    return EXIT_FAILURE;
  }

enum apivalue_command api_command_initialize(struct api_command * api)
  {
    struct api_list * list_api;
    struct api_stdio * stdio_api;

    list_api = api->api_list;
    stdio_api = api->api_stdio;
    if (list_api == NULL || stdio_api == NULL)
      return apivalue_command_error_null_argument;
    *api = api_command_defaults;
    api->api_list = list_api;
    api->api_stdio = stdio_api;
    list_api->initialize_list(list_api, &api->commands);
    return apivalue_command_success;
  }

static int remove_command(struct api_command * api, struct command * command)
  {
    (void) api->api_list->remove_list_item(api->api_list, &command->list_item);
    return EXIT_SUCCESS;
  }
