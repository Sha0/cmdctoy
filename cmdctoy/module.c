/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "depend.h"
#include "toy.h"
#include "toyio.h"
#include "toylib.h"
#include "toydef.h"
#include "list.h"
#include "modpriv.h"
#include "module.h"

static apifunction_dependency_observe_call live_module_cleanup;
static func_module_live_module_from_module live_module_from_module;
static func_module_load module_load;
static func_module_request_shutdown module_request_shutdown;
static func_module_start_thread start_thread;
static func_module_thread_started module_thread_started;
static func_module_unload module_unload;
static func_module_unload_all module_unload_all;

static unsigned long int order;
static struct module_api api_module_defaults;
static struct list module_list;

static unsigned long int order = 0;

static struct module_api api_module_defaults =
  {
    NULL,
    NULL,
    NULL,
    NULL,
    &live_module_from_module,
    &module_load,
    &module_request_shutdown,
    &start_thread,
    &module_thread_started,
    &module_unload,
    &module_unload_all,
    &module_list
  };

static struct list module_list =
  {
    {
      &module_list.head,
      &module_list.head
    }
  };

void initialize_module_system(struct top * top)
  {
    struct api_dependency * dependency_api;
    struct api_list * list_api;
    struct module_api * module_api;
    struct api_stdio * stdio_api;
    struct api_stdlib * stdlib_api;

    dependency_api = top->api_dependency;
    list_api = top->api_list;
    module_api = top->module_api;
    stdio_api = top->api_stdio;
    stdlib_api = top->api_stdlib;

    *module_api = api_module_defaults;
    module_api->api_dependency = dependency_api;
    module_api->api_list = list_api;
    module_api->api_stdio = stdio_api;
    module_api->api_stdlib = stdlib_api;
  }

static void live_module_cleanup(struct api_dependency * api, struct dependency * dependency, struct dependency_observe * dependency_observe, enum apivalue_dependency type, void * pointer)
  {
    struct api_list * list_api;
    struct module_private * module_private;
    int module_rv;
    struct live_module * old_module;
    struct api_stdio * stdio_api;
    struct api_stdlib * stdlib_api;

    (void) api;
    (void) dependency;

    /* We only care about clean-up */
    if (type != apivalue_dependency_observer_departed)
      return;
    /* We only care about when we are departing */
    if (pointer != dependency_observe)
      return;
    /* Otherwise, we have departed, so it's time to free the live_module */
    module_private = type_with_member_at_ptr(struct module_private, dependency_observe, dependency_observe);
    list_api = module_private->ctx.api_list;
    stdio_api = module_private->ctx.api_stdio;
    stdlib_api = module_private->ctx.api_stdlib;
    old_module = module_private->ctx.work_module;
    module_private->ctx.work_module = &module_private->live_module;
    module_rv = module_private->live_module.module.v1.event_func(apivalue_module_event_type_unload, NULL);
    module_private->ctx.work_module = old_module;
    if (module_rv != EXIT_SUCCESS)
      {
        (void) stdio_api->fprintf(stdio_api, stderr, "Module having nice-name '%s' failed to unload with return-value '%d'\n", module_private->live_module.module.v1.nice_name, module_rv);
        /* Too bad.  It'll be unlinked, anyway */
      }
      else
      {
        (void) stdio_api->fprintf(stdio_api, stderr, "Module having nice-name '%s' was unloaded\n", module_private->live_module.module.v1.nice_name);
      }
    (void) list_api->remove_list_item(list_api, &module_private->list_item);
    stdlib_api->free(stdlib_api, module_private);
  }

static int live_module_from_module(struct top * top, union module * module, struct dependency_want * dependency_want, struct live_module ** live_module)
  {
    struct api_dependency * dependency_api;
    enum apivalue_dependency dependency_rv;
    struct api_list * list_api;
    struct list_item * list_item;
    struct module_api * module_api;
    struct module_private * module_private;
    int need_free;
    struct api_stdio * stdio_api;
    struct api_stdlib * stdlib_api;

    need_free = 0;

    if (dependency_want == NULL)
      return EXIT_FAILURE;
    module_api = top->module_api;
    dependency_api = module_api->api_dependency;
    list_api = module_api->api_list;
    stdio_api = module_api->api_stdio;
    stdlib_api = module_api->api_stdlib;
    if (strncmp(module->v1.required.signature, module_signature, apivalue_module_signature_length) != 0)
      {
        (void) stdio_api->fprintf(stdio_api, stderr, "Invalid signature for a module\n");
        return EXIT_FAILURE;
      }
    if (module->v1.required.api_version < 1)
      {
        (void) stdio_api->fprintf(stdio_api, stderr, "Unsupported API version for a module\n");
        return EXIT_FAILURE;
      }
    /* TODO: Locking */
    /* Check if a live module has already derived from the same origin */
    for (list_item = module_api->module_list->head.next; list_item != &module_api->module_list->head; list_item = list_item->next)
      {
        module_private = type_with_member_at_ptr(struct module_private, list_item, list_item);
        if (module_private->live_module.origin == module)
          {
            /* Found it */
            dependency_rv = dependency_api->want(dependency_api, &module_private->live_module.dependency, dependency_want);
            if (dependency_rv != apivalue_dependency_success)
              {
                (void) stdio_api->fprintf(stdio_api, stderr, "Dependency API 'want' unexpectedly failed for existing module having nice-name '%s'\n", module->v1.nice_name);
                return EXIT_FAILURE;
              }
            if (live_module != NULL)
              *live_module = &module_private->live_module;
            return EXIT_SUCCESS;
          }
      }
    /* Check for wrap-around */
    if (order + 1 == 0)
      {
        (void) stdio_api->fprintf(stdio_api, stderr, "Far too many live modules have been created, so could not create one for module having nice-name '%s'\n", module->v1.nice_name);
        return EXIT_FAILURE;
      }
    /* Create one, to be freed after its user-count goes to 0 */
    module_private = stdlib_api->malloc(stdlib_api, sizeof *module_private);
    if (module_private == NULL)
      {
        (void) stdio_api->fprintf(stdio_api, stderr, "Out of memory while loading module '%s'\n", module->v1.nice_name);
        goto err_module_private;
      }
    need_free = 1;
    /* TODO: locking */
    module_private->order = order;
    ++order;
    /* Copy the original module details */
    module_private->live_module.module = *module;
    /* Not yet loaded */
    module_private->live_module.loaded = 0;
    /* Note the original */
    module_private->live_module.origin = module;
    /* Copy the context */
    module_private->ctx = *top;
    /* Adjust the context to point to this newly-created module */
    module_private->ctx.work_module = &module_private->live_module;
    module_private->live_module.ctx = &module_private->ctx;
    /* Set up the dependency */
    dependency_api->initialize(dependency_api, &module_private->live_module.dependency);
    dependency_rv = dependency_api->want(dependency_api, &module_private->live_module.dependency, dependency_want);
    if (dependency_rv != apivalue_dependency_success)
      {
        (void) stdio_api->fprintf(stdio_api, stderr, "Dependency API 'want' unexpectedly failed for module having nice-name '%s'\n", module->v1.nice_name);
        goto err_want;
      }
    dependency_api->initialize_observe(dependency_api, &module_private->dependency_observe, &live_module_cleanup);
    dependency_rv = dependency_api->observe(dependency_api, &module_private->live_module.dependency, &module_private->dependency_observe);
    if (dependency_rv != apivalue_dependency_success)
      {
        (void) stdio_api->fprintf(stdio_api, stderr, "Dependency API 'observe' unexpectedly failed for module having nice-name '%s'\n", module->v1.nice_name);
        goto err_observe;
      }
    /* The clean-up observer will free, after this point */
    need_free = 0;
    /* Add the live module to the list */
    (void) list_api->initialize_list_item(list_api, &module_private->list_item);
    (void) list_api->add_item_to_list_tail(list_api, &module_private->list_item, &module_list);
    if (live_module != NULL)
      *live_module = &module_private->live_module;
    return EXIT_SUCCESS;

    /* Our final observer frees itself */
    err_observe:

    dependency_api->unwant(dependency_api, &module_private->live_module.dependency, &module_private->dependency_want);
    err_want:

    if (need_free)
      stdlib_api->free(stdlib_api, module_private);
    err_module_private:

    return EXIT_FAILURE;
  }

static int module_load(struct top * top, struct live_module * live_module)
  {
    struct module_api * module_api;
    struct live_module * old_module;
    int rv;
    struct api_stdio * stdio_api;

    module_api = top->module_api;
    stdio_api = module_api->api_stdio;
    if (live_module->loaded != 0)
      {
        (void) stdio_api->fprintf(stdio_api, stderr, "Module already loaded\n");
        return EXIT_FAILURE;
      }
    /* Push the current module context */
    old_module = top->work_module;
    /* Send the load-event to the module */
    top->work_module = live_module;
    rv = live_module->module.v1.event_func(apivalue_module_event_type_loaded, live_module);
    /* Pop the module context */
    top->work_module = old_module;
    if (rv != EXIT_SUCCESS)
      {
        (void) stdio_api->fprintf(stdio_api, stderr, "Module having nice-name '%s' loaded with return-value '%d'\n", live_module->module.v1.nice_name, rv);
        goto event_loaded_failed;
      }
    live_module->loaded = 1;
    return EXIT_SUCCESS;

    module_api->unload(top, live_module);
    event_loaded_failed:

    return rv;
  }

static void module_request_shutdown(struct top * top)
  {
    struct api_list * list_api;
    struct list_item * list_item;
    struct module_api * module_api;
    struct module_private * module_private;
    int module_rv;
    struct live_module * old_module;
    struct api_stdio * stdio_api;

    module_api = top->module_api;
    list_api = module_api->api_list;
    stdio_api = module_api->api_stdio;
    while ((list_item = list_api->remove_item_from_list_tail(list_api, &module_list)) != NULL)
      {
        module_private = type_with_member_at_ptr(struct module_private, list_item, list_item);
        old_module = top->work_module;
        top->work_module = &module_private->live_module;
        module_rv = module_private->live_module.module.v1.event_func(apivalue_module_event_type_unload_requested, NULL);
        top->work_module = old_module;
        if (module_rv != EXIT_SUCCESS)
          (void) stdio_api->fprintf(stdio_api, stderr, "Module having nice-name '%s' failed to respond to unload-request with return-value '%d'\n", module_private->live_module.module.v1.nice_name, module_rv);
      }
  }

static int module_thread_started(struct top * top, struct live_module * live_module)
  {
    struct module_api * module_api;
    int module_rv;
    struct api_stdio * stdio_api;

    module_api = top->module_api;
    stdio_api = module_api->api_stdio;
    module_rv = live_module->module.v1.event_func(apivalue_module_event_type_thread_started, live_module->ctx);
    if (module_rv != EXIT_SUCCESS)
      (void) stdio_api->fprintf(stdio_api, stderr, "Module having nice-name '%s' failed thread-start with return-value '%d'\n", live_module->module.v1.nice_name, module_rv);
    return module_rv;
  }

static void module_unload(struct top * top, struct live_module * live_module)
  {
    struct api_dependency * dependency_api;
    struct module_api * module_api;

    /* Do not unload if it's not loaded */
    if (live_module->loaded == 0)
      return;
    module_api = top->module_api;
    dependency_api = module_api->api_dependency;
    dependency_api->go_away(dependency_api, &live_module->dependency);
  }

static void module_unload_all(struct top * top)
  {
    struct api_list * list_api;
    struct list_item * list_item;
    struct module_api * module_api;
    struct module_private * module_private;
    struct list swap;

    module_api = top->module_api;
    list_api = module_api->api_list;
    /* Move the modules to a temporary list */
    list_api->initialize_list(list_api, &swap);
    while ((list_item = list_api->remove_item_from_list_head(list_api, &module_list)) != NULL)
      list_api->add_item_to_list_tail(list_api, list_item, &swap);
    /* Send each module back to the primary list and process each one */
    while ((list_item = list_api->remove_item_from_list_head(list_api, &swap)) != NULL)
      {
        (void) list_api->add_item_to_list_tail(list_api, list_item, &module_list);
        module_private = type_with_member_at_ptr(struct module_private, list_item, list_item);
        module_api->unload(top, &module_private->live_module);
      }
    /* The result should have been a single visit to each module */
  }

static int start_thread(struct top * top)
  {
    struct list_item * list_item;
    struct module_private * module_private;
    int module_rv;

    for (list_item = module_list.head.next; list_item != &module_list.head; list_item = list_item->next)
      {
        module_private = type_with_member_at_ptr(struct module_private, list_item, list_item);
        module_rv = module_thread_started(top, &module_private->live_module);
        if (module_rv != EXIT_SUCCESS)
          return module_rv;
      }
    return EXIT_SUCCESS;
  }
