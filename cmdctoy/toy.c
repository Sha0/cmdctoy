/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#include <stdlib.h>
#include "builtins.h"
#include "command.h"
#include "depend.h"
#include "toy.h"
#include "toyio.h"
#include "toylib.h"
#include "toydef.h"
#include "toyscope.h"
#include "list.h"
#include "main.h"
#include "module.h"
#include "process.h"
#include "type.h"

static func_module_event module_event;
static func_schedule_last schedule_last;
static func_schedule_next schedule_next;
static func_request_shutdown request_shutdown;
static func_work shutdown_checker;
static func_work startup;

static struct top * ctx;
static struct live_module module;

static struct live_module module =
  {
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
        "toy"
      }
    },
    NULL,
    NULL,
    {
      {
        {
          NULL,
          NULL
        }
      },
      {
        {
          NULL,
          NULL
        }
      },
      {
        {
          NULL,
          NULL
        }
      },
      {
        {
          NULL,
          NULL
        }
      },
      {
        {
          NULL,
          NULL
        }
      },
      apivalue_dependency_success
    },
    1
  };

int toy_loop(struct process * process)
  {
    struct api_btree btree_api;
    enum apivalue_btree btree_rv;
    struct api_command command_api;
    struct api_dependency dependency_api;
    enum apivalue_dependency dependency_rv;
    struct api_list list_api;
    struct list_item * list_item;
    enum apivalue_list list_rv;
    struct module_api module_api;
    int return_value;
    struct work_item startup_work;
    int shutdown_requested;
    struct work_item shutdown_work;
    struct api_stdio stdio_api;
    struct api_stdlib stdlib_api;
    enum apivalue_stdio stdio_rv;
    enum apivalue_stdlib stdlib_rv;
    struct top top_struct;
    struct api_toy_scope toy_scope_api;
    enum apivalue_toy_scope toy_scope_rv;
    struct api_type type_api;
    enum apivalue_type type_rv;
    struct work_item * work_item;
    struct work_list work_list;

    ctx = &top_struct;

    module.ctx = ctx;

    shutdown_requested = 0;

    top_struct.main_stack = process->main_stack;
    top_struct.api_dependency = &dependency_api;
    top_struct.module_api = &module_api;
    top_struct.api_btree = &btree_api;
    top_struct.api_command = &command_api;
    top_struct.work_module = &module;
    top_struct.request_shutdown = &request_shutdown;
    top_struct.shutdown_requested = &shutdown_requested;
    top_struct.api_stdio = &stdio_api;
    top_struct.api_stdlib = &stdlib_api;
    top_struct.api_toy_scope = &toy_scope_api;
    top_struct.api_type = &type_api;
    top_struct.work_list = &work_list;
    top_struct.schedule_last = &schedule_last;
    top_struct.schedule_next = &schedule_next;

    stdio_rv = api_stdio_initialize(&stdio_api);
    if (stdio_rv != apivalue_stdio_success)
      return EXIT_FAILURE;

    list_rv = api_list_initialize(&list_api);
    if (list_rv != apivalue_list_success)
      return EXIT_FAILURE;
    ctx->api_list = &list_api;
    ctx->api_list->initialize_list(ctx->api_list, &work_list.list);

    btree_rv = api_btree_initialize(&btree_api);
    if (btree_rv != apivalue_btree_success)
      return EXIT_FAILURE;

    dependency_api.api_list = &list_api;
    dependency_rv = api_dependency_initialize(&dependency_api);
    if (dependency_rv != apivalue_dependency_success)
      return EXIT_FAILURE;

    stdlib_rv = api_stdlib_initialize(&stdlib_api);
    if (stdlib_rv != apivalue_stdlib_success)
      return EXIT_FAILURE;

    type_rv = api_type_initialize(&type_api);
    if (type_rv != apivalue_type_success)
      return EXIT_FAILURE;

    toy_scope_api.api_btree = &btree_api;
    toy_scope_api.api_stdlib = &stdlib_api;
    toy_scope_rv = api_toy_scope_initialize(&toy_scope_api);
    if (toy_scope_rv != apivalue_toy_scope_success)
      return EXIT_FAILURE;

    (void) ctx->api_list->initialize_list_item(ctx->api_list, &startup_work.list_item);
    startup_work.work = &startup;
    (void) ctx->schedule_last(&module, &startup_work, &work_list);

    (void) ctx->api_list->initialize_list_item(ctx->api_list, &shutdown_work.list_item);
    shutdown_work.work = &shutdown_checker;
    (void) ctx->schedule_last(&module, &shutdown_work, &work_list);

    while ((list_item = ctx->api_list->remove_item_from_list_head(ctx->api_list, &ctx->work_list->list)) != NULL)
      {
        work_item = type_with_member_at_ptr(struct work_item, list_item, list_item);
        ctx = work_item->ctx;
        /* Do the work */
        return_value = work_item->work(work_item);
        ctx = &top_struct;
      }

    return return_value;
  }

static int module_event(enum apivalue_module_event_type type, void * event_data)
  {
    (void) type;
    (void) event_data;
    return EXIT_FAILURE;
  }

static void request_shutdown(struct top * top)
  {
    if (*(top->shutdown_requested))
      return;
    *(top->shutdown_requested) = 1;
    (void) top->api_stdio->fprintf(top->api_stdio, stderr, "Shutdown requested\n");
    /* Modules might schedule shutdown-related work */
    top->module_api->request_shutdown(top);
  }

static int schedule_last(struct live_module * work_module, struct work_item * work_item, struct work_list * work_list)
  {
    work_item->ctx = work_module->ctx;
    work_item->live_module = work_module;
    (void) ctx->api_list->add_item_to_list_tail(ctx->api_list, &work_item->list_item, &work_list->list);
    return EXIT_SUCCESS;
  }

static int schedule_next(struct live_module * work_module, struct work_item * work_item, struct work_list * work_list)
  {
    work_item->ctx = work_module->ctx;
    work_item->live_module = work_module;
    (void) ctx->api_list->add_item_to_list_head(ctx->api_list, &work_item->list_item, &work_list->list);
    return EXIT_SUCCESS;
  }

static int shutdown_checker(struct work_item * work_item)
  {
    if (ctx->api_list->list_is_empty(ctx->api_list, &ctx->work_list->list))
      {
        ctx->module_api->unload_all(ctx);
        return EXIT_SUCCESS;
      }
    /* Otherwise, something arrived after us, so re-schedule to check later on */
    (void) ctx->schedule_last(&module, work_item, ctx->work_list);
    return EXIT_FAILURE;
  }

static int startup(struct work_item * work_item)
  {
    enum apivalue_command command_rv;
    int rv;

    (void) work_item;

    initialize_module_system(ctx);

    ctx->api_command->api_list = ctx->api_list;
    ctx->api_command->api_stdio = ctx->api_stdio;
    command_rv = api_command_initialize(ctx->api_command);
    if (command_rv != apivalue_command_success)
      return EXIT_FAILURE;

    rv = builtin_startup(ctx);
    if (rv != EXIT_SUCCESS)
      return rv;
    rv = ctx->module_api->start_thread(ctx);
    if (rv != EXIT_SUCCESS)
      return rv;
    return EXIT_SUCCESS;
  }
