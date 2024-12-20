/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#include <stddef.h>
#include "btree.h"
#include "depend.h"
#include "toydef.h"
#include "toylib.h"
#include "list.h"
#include "main.h"
#include "process.h"

static apifunction_process_initialize_process initialize_process;
static apifunction_dependency_observe_call process_observe;

static struct api_process api_process_defaults =
  {
    NULL,
    NULL,
    NULL,
    NULL,
    &api_process_initialize,
    &initialize_process
  };

enum apivalue_process api_process_initialize(struct api_process * api)
  {
    struct api_btree * btree_api;
    struct api_dependency * dependency_api;
    struct api_list * list_api;
    struct api_stdlib * stdlib_api;

    if (api->api_btree == NULL || api->api_dependency == NULL || api->api_list == NULL || api->api_stdlib == NULL)
      return apivalue_process_error_null_argument;
    btree_api = api->api_btree;
    dependency_api = api->api_dependency;
    list_api = api->api_list;
    stdlib_api = api->api_stdlib;
    *api = api_process_defaults;
    api->api_btree = btree_api;
    api->api_dependency = dependency_api;
    api->api_list = list_api;
    api->api_stdlib = stdlib_api;
    return apivalue_process_success;
  }

static enum apivalue_process initialize_process(struct api_process * api, struct process ** process, struct main_stack * main_stack)
  {
    enum apivalue_btree btree_rv;
    struct api_dependency * dependency_api;
    enum apivalue_dependency dependency_rv;
    enum apivalue_list list_rv;
    int need_free;
    struct process * new_process;
    enum apivalue_process rv;
    struct api_stdlib * stdlib_api;
    enum apivalue_stdlib stdlib_rv;

    if (api == NULL || process == NULL || main_stack == NULL)
      return apivalue_process_error_null_argument;
    /* Allocate the process */
    stdlib_api = api->api_stdlib;
    need_free = 0;
    new_process = stdlib_api->malloc(stdlib_api, sizeof *new_process);
    if (new_process == NULL)
      {
        rv = apivalue_process_error_out_of_memory;
        goto err_new_process;
      }
    need_free = 1;
    /* Use the caller's APIs to populate allocated instances */
    btree_rv = api->api_btree->api_initialize(&new_process->api_btree);
    if (btree_rv != apivalue_btree_success)
      {
        rv = apivalue_process_error_btree_api;
        goto err_api_btree;
      }
    list_rv = api->api_list->api_initialize(&new_process->api_list);
    if (list_rv != apivalue_list_success)
      {
        rv = apivalue_process_error_list_api;
        goto err_api_list;
      }
    new_process->api_dependency.api_list = &new_process->api_list;
    dependency_rv = api->api_dependency->api_initialize(&new_process->api_dependency);
    if (dependency_rv != apivalue_dependency_success)
      {
        rv = apivalue_process_error_dependency_api;
        goto err_api_dependency;
      }
    stdlib_rv = api->api_stdlib->api_initialize(&new_process->api_stdlib);
    if (stdlib_rv != apivalue_stdlib_success)
      {
        rv = apivalue_process_error_stdlib_api;
        goto err_api_stdlib;
      }
    /* Note the main stack */
    new_process->main_stack = main_stack;
    /* Initialize it as a dependency */
    dependency_api = api->api_dependency;
    dependency_api->initialize(dependency_api, &new_process->dependency);
    /* The caller obviously wants the process */
    dependency_api->initialize_want(dependency_api, &new_process->want);
    dependency_rv = dependency_api->want(dependency_api, &new_process->dependency, &new_process->want);
    if (dependency_rv != apivalue_dependency_success)
      {
        rv = apivalue_process_error_dependency_api;
        goto err_want;
      }
    /* The process is provided-for */
    dependency_api->initialize_provide(dependency_api, &new_process->provide);
    dependency_rv = dependency_api->provide(dependency_api, &new_process->dependency, &new_process->provide);
    if (dependency_rv != apivalue_dependency_success)
      {
        rv = apivalue_process_error_dependency_api;
        goto err_provide;
      }
    /* The process is ready */
    dependency_api->initialize_ready(dependency_api, &new_process->ready);
    dependency_rv = dependency_api->dependency_ready(dependency_api, &new_process->dependency, &new_process->ready);
    if (dependency_rv != apivalue_dependency_success)
      {
        rv = apivalue_process_error_dependency_api;
        goto err_ready;
      }
    /* Set up an observer to free the process */
    dependency_api->initialize_observe(dependency_api, &new_process->observe, &process_observe);
    dependency_rv = dependency_api->observe(dependency_api, &new_process->dependency, &new_process->observe);
    if (dependency_rv != apivalue_dependency_success)
      {
        rv = apivalue_process_error_dependency_api;
        goto err_observe;
      }
    /* The observer is now responsible for freeing the process */
    need_free = 0;

    *process = new_process;
    return apivalue_process_success;

    /* The observer will be released when all other links are released */
    err_observe:

    dependency_api->unready(dependency_api, &new_process->dependency, &new_process->ready);
    err_ready:

    dependency_api->unprovide(dependency_api, &new_process->dependency, &new_process->provide);
    err_provide:

    dependency_api->unwant(dependency_api, &new_process->dependency, &new_process->want);
    err_want:

    err_api_stdlib:

    err_api_dependency:

    err_api_list:

    err_api_btree:

    if (need_free)
      stdlib_api->free(stdlib_api, new_process);
    err_new_process:

    return rv;
  }

static void process_observe(struct api_dependency * api, struct dependency * dependency, struct dependency_observe * dependency_observe, enum apivalue_dependency type, void * pointer)
  {
    struct process * process;

    process = type_with_member_at_ptr(struct process, observe, dependency_observe);
    switch (type)
      {
        case apivalue_dependency_error_going_away:
        /* The module is going away */
        api->unready(api, dependency, &process->ready);
        api->unwant(api, dependency, &process->want);
        return;

        case apivalue_dependency_not_needed:
        /* If the process is ready, a need might still come along, at some point */
        if (api->is_ready(api, dependency))
          return;
        /* Otherwise, there's no further need to provide for it */
        api->unprovide(api, dependency, &process->provide);
        return;

        case apivalue_dependency_observer_departed:
        /* We only care about our particular observer */
        if (pointer != dependency_observe)
          return;
        /* Our observer has been detached from the dependency, so free the process */
        process->api_stdlib.free(&process->api_stdlib, process);
        return;

        default:
        return;
      }
  }
