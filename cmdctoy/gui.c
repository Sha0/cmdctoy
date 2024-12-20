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
#include "toyio.h"
#include "toylib.h"
#include "gui.h"
#include "module.h"

static func_work get_user_input;
static int get_user_input_from_stream(struct work_item *, FILE *);
static func_module_event module_event;

static struct top * ctx;
static struct live_module * live_module;

#if BUILTIN_GET_USER_INPUT
union module builtin_module_get_user_input =
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
      "get_user_input"
    }
  };

static int get_user_input(struct work_item * work_item)
  {
    if (*(ctx->shutdown_requested))
      return EXIT_SUCCESS;

    /* Otherwise, re-schedule to acquire the command after this one */
    (void) ctx->schedule_last(live_module, work_item, ctx->work_list);

    return get_user_input_from_stream(work_item, stdin);
  }

static int get_user_input_from_stream(struct work_item * work_item, FILE * stream)
  {
    char c;
    char cmd_buf[BUFSIZ];
    size_t cmd_len;
    int fgetc_rv;
    int new_errno;
    int old_errno;
    int rv;
    unsigned char uc;

    /* POSIX wants fgetc to set errno upon error */
    old_errno = errno;

    for (cmd_len = 0; cmd_len < sizeof cmd_buf - 1; ++cmd_len)
      {
        /* For POSIX */
        errno = 0;

        fgetc_rv = ctx->api_stdio->fgetc(ctx->api_stdio, stream);

        /* For POSIX */
        new_errno = errno;
        errno = old_errno;

        if (fgetc_rv == EOF)
          {
            /* That's the end of looping, so de-schedule */
            (void) ctx->api_list->remove_list_item(ctx->api_list, &work_item->list_item);
            if (ctx->api_stdio->f_error(ctx->api_stdio, stream))
              {
                (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Error reading input stream, so command has been ignored\n");
                if (new_errno != 0)
                  (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Possible POSIX errno '%d' with strerror message '%s'\n", new_errno, strerror(new_errno));
                return EXIT_FAILURE;
              }
            if (ctx->api_stdio->f_eof(ctx->api_stdio, stream))
              {
                if (cmd_len == 0)
                  return EXIT_SUCCESS;
                (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Command not terminated, so has been ignored\n");
                return EXIT_FAILURE;
              }
          }

        /* Deal with fgetc's unsigned char versus strings' unknown-signed char */
        uc = (unsigned char) fgetc_rv;
        c = *(char *) &uc;

        if (c == '\0')
          {
            (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Null character found in input stream, so command has been ignored\n");
            return EXIT_FAILURE;
          }

        /* Permit and convert this kind of line-ending */
        if (c == '\r')
          {
            c = '\n';
            /* Optimization for a common follow-up */
            fgetc_rv = ctx->api_stdio->fgetc(ctx->api_stdio, stream);
            if (fgetc_rv != '\n')
              (void) ctx->api_stdio->ungetc(ctx->api_stdio, fgetc_rv, stream);
          }

        cmd_buf[cmd_len] = c;

        if (c == '\n')
          {
            /* Command termination */
            ++cmd_len;
            cmd_buf[cmd_len] = '\0';
            break;
          }
      }

    /* Empty command? */
    if (cmd_len == 1)
      return EXIT_SUCCESS;

    /* Check for truncation */
    if (cmd_len == sizeof cmd_buf - 1 && cmd_buf[cmd_len - 1] != '\n')
      {
        (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Command was truncated at %d characters, so has been ignored\n", (int) cmd_len);
        return EXIT_FAILURE;
      }

    rv = ctx->api_command->line(ctx->api_command, cmd_buf, cmd_len);
    return rv;
  }

static int module_event(enum apivalue_module_event_type type, void * event_data)
  {
    struct work_item * work_item;

    switch (type)
      {
        case apivalue_module_event_type_loaded:
        if (live_module != NULL)
          return EXIT_FAILURE;
        live_module = event_data;
        return EXIT_SUCCESS;

        case apivalue_module_event_type_thread_started:
        ctx = event_data;
        work_item = ctx->api_stdlib->malloc(ctx->api_stdlib, sizeof *work_item);
        if (work_item == NULL)
          {
            (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Out of memory while acquiring user-input work-item\n");
            return EXIT_FAILURE;
          }
        live_module->module.v1.module_pointers[0] = work_item;
        (void) ctx->api_list->initialize_list_item(ctx->api_list, &work_item->list_item);
        work_item->work = &get_user_input;
        (void) ctx->schedule_last(live_module, work_item, ctx->work_list);
        return EXIT_SUCCESS;

        case apivalue_module_event_type_thread_stop_requested:
        case apivalue_module_event_type_thread_stopped:
        case apivalue_module_event_type_unload_requested:
        return EXIT_SUCCESS;

        case apivalue_module_event_type_unload:
        work_item = live_module->module.v1.module_pointers[0];
        if (work_item != NULL)
          {
            /* Remove from schedule, if scheduled */
            if (work_item->list_item.next != NULL)
              (void) ctx->api_list->remove_list_item(ctx->api_list, &work_item->list_item);
            ctx->api_stdlib->free(ctx->api_stdlib, work_item);
          }
        live_module = NULL;
        return EXIT_SUCCESS;
      }
    return EXIT_FAILURE;
  }
