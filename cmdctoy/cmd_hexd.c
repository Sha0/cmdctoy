/*
 * Copyright (C) 2013, 2024 Shao Miller.  All rights reserved.
 * Original hexdump function (2013-12-22) at:
 * - https://www.iso-9899.info/wiki/Code_snippets:hexdump
 */
#include <ctype.h>
#include <errno.h>
#include <stddef.h>
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

struct cmd_hexdump;

struct cmd_hexdump
  {
    struct command command;
    struct top * ctx;
  };

static apifunction_command cmd_hexdump;
static void hexdump(struct top *, const void *, unsigned long int);
static func_module_event module_event;

static struct command command_hexdump;
static struct live_module * live_module;

#if BUILTIN_CMD_HEXDUMP
union module builtin_module_cmd_hexdump =
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
      "cmd_hexdump"
    }
  };

static struct command command_hexdump =
  {
    NULL,
    "hexdump",
    &cmd_hexdump,
    {
      NULL,
      NULL
    }
  };

static int cmd_hexdump(struct api_command * api, struct command * command, int argc, char ** argv)
  {
    struct cmd_hexdump * cmd;
    struct top * ctx;
    char * endptr;
    int new_errno;
    int old_errno;
    void * pointer;
    int rv;
    unsigned long int size;
    static const char usage[] =
      "Usage:\n"
      "  hexdump POINTER SIZE  Show SIZE bytes at memory-address POINTER\n"
      "Warnings:\n"
      "  Pointer %p representations are not portable.  Using a %p sequence that\n"
      "  hasn't been printed earlier yields an undefined result.\n"
      "Note:\n"
      "  SIZE is expected to be an 'unsigned long int'\n"
      "  The 'top' pointer is: "
      ;

    (void) api;

    cmd = type_with_member_at_ptr(struct cmd_hexdump, command, command);
    ctx = cmd->ctx;

    if (argc != 3)
      {
        (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "%s%p\n", usage, (void *) ctx);
        return EXIT_FAILURE;
      }

    old_errno = errno;

    errno = 0;
    rv = sscanf(argv[1], "%p", &pointer);
    new_errno = errno;
    errno = old_errno;
    if (rv != 1)
      {
        (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Unrecognized pointer format\n");
        if (new_errno != 0)
          {
            (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "...and errno was '%d' with message '%s'\n", new_errno, strerror(new_errno));
            return new_errno;
          }
        return EXIT_FAILURE;
      }

    errno = 0;
    size = strtoul(argv[2], &endptr, 0);
    new_errno = errno;
    errno = old_errno;
    if (new_errno != 0)
      {
        (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Unrecognized 'unsigned long int' format for SIZE, with error '%d' and message '%s'\n", new_errno, strerror(new_errno));
        return new_errno;
      }
    if (*endptr != '\0')
      {
        (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Unrecognized 'unsigned long int' format for SIZE\n");
        return EXIT_FAILURE;
      }

    hexdump(ctx, pointer, size);
    return EXIT_SUCCESS;
  }

static void hexdump(struct top * ctx, const void * memory, unsigned long int bytes)
  {
    const unsigned char * p;
    const unsigned char * q;
    unsigned int i;

    p = memory;
    while (bytes)
      {
        q = p;
        ctx->api_stdio->fprintf(ctx->api_stdio, stdout, "%p: ", (void *) p);
        for (i = 0; i < 8 && bytes; ++i)
          {
            ctx->api_stdio->fprintf(ctx->api_stdio, stdout, "%02X ", *p);
            ++p;
            --bytes;
          }
        if (i == 8)
          ctx->api_stdio->fprintf(ctx->api_stdio, stdout, " ");
        for (; i < 16 && bytes; ++i)
          {
            ctx->api_stdio->fprintf(ctx->api_stdio, stdout, "%02X ", *p);
            ++p;
            --bytes;
          }
        bytes += i;
        while (i < 8)
          {
            ctx->api_stdio->fprintf(ctx->api_stdio, stdout, "XX ");
            ++i;
          }
        if (i == 8)
          ctx->api_stdio->fprintf(ctx->api_stdio, stdout, " ");
        while (i < 16)
          {
            ctx->api_stdio->fprintf(ctx->api_stdio, stdout, "XX ");
            ++i;
          }
        ctx->api_stdio->fprintf(ctx->api_stdio, stdout, "| ");
        p = q;
        for (i = 0; i < 16 && bytes; ++i)
          {
            ctx->api_stdio->fprintf(ctx->api_stdio, stdout, "%c", isprint(*p) && !isspace(*p) ? *p : ' ');
            ++p;
            --bytes;
          }
        while (i < 16)
          {
            ctx->api_stdio->fprintf(ctx->api_stdio, stdout, " ");
            ++i;
          }
        ctx->api_stdio->fprintf(ctx->api_stdio, stdout, " |\n");
      }
  }

static int module_event(enum apivalue_module_event_type type, void * event_data)
  {
    struct cmd_hexdump * command;
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
            (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Out of memory while registering 'hexdump' command\n");
            return EXIT_FAILURE;
          }
        live_module->module.v1.module_pointers[0] = command;
        command->command = command_hexdump;
        command->ctx = ctx;
        command->command.live_module = live_module;
        (void) ctx->api_list->initialize_list_item(ctx->api_list, &command->command.list_item);
        rv = ctx->api_command->add(ctx->api_command, &command->command);
        if (rv != EXIT_SUCCESS)
          (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Error '%d' while attempting to register 'hexdump' command\n", rv);
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
