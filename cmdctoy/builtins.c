/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include "builtins.h"
#include "depend.h"
#include "toy.h"
#include "toydef.h"
#include "toyio.h"
#include "toylib.h"
#include "module.h"

struct builtin;
struct builtins;

static apifunction_dependency_observe_call builtin_cleanup;
static apifunction_dependency_observe_call builtin_shutdown;

#if BUILTIN_CMD_EXIT
extern union module builtin_module_cmd_exit;
#endif
#if BUILTIN_CMD_HELP
extern union module builtin_module_cmd_help;
#endif
#if BUILTIN_CMD_HEXDUMP
extern union module builtin_module_cmd_hexdump;
#endif
#if BUILTIN_CMD_LOAD
extern union module builtin_module_cmd_load;
#endif
#if BUILTIN_CMD_MONO
extern union module builtin_module_cmd_monolith;
#endif
#if BUILTIN_CMD_TYPE
extern union module builtin_module_cmd_type;
#endif
#if BUILTIN_GET_USER_INPUT
extern union module builtin_module_get_user_input;
#endif
#if BUILTIN_MOD2
extern union module builtin_module_mod2;
#endif

static union module * builtin_modules_array[] =
  {
#if BUILTIN_CMD_EXIT
    &builtin_module_cmd_exit,
#endif
#if BUILTIN_CMD_HELP
    &builtin_module_cmd_help,
#endif
#if BUILTIN_CMD_HEXDUMP
    &builtin_module_cmd_hexdump,
#endif
#if BUILTIN_CMD_LOAD
    &builtin_module_cmd_load,
#endif
#if BUILTIN_CMD_MONO
    &builtin_module_cmd_monolith,
#endif
#if BUILTIN_CMD_TYPE
    &builtin_module_cmd_type,
#endif
#if BUILTIN_GET_USER_INPUT
    &builtin_module_get_user_input,
#endif
#if BUILTIN_MOD2
    &builtin_module_mod2,
#endif
    NULL
  };
static union module ** builtin_modules = builtin_modules_array;
static size_t builtin_modules_count = countof(builtin_modules_array) - 1;

struct builtin
  {
    struct dependency_observe cleanup;
    struct dependency_want builtins_want;
    struct dependency_want module_want;
    struct builtins * builtins;
    struct live_module * live_module;
    struct top * top;
  };

struct builtins
  {
    struct dependency dependency;
    struct dependency_observe cleanup;
    struct builtin * builtins;
    struct top * top;
  };

static void builtin_cleanup(struct api_dependency * api, struct dependency * dependency, struct dependency_observe * dependency_observe, enum apivalue_dependency type, void * pointer)
  {
    struct builtin * builtin;

    builtin = type_with_member_at_ptr(struct builtin, cleanup, dependency_observe);
    switch (type)
      {
        case apivalue_dependency_error_going_away:
        /* The module is going away */
        api->unwant(api, dependency, &builtin->module_want);
        return;

        case apivalue_dependency_observer_departed:
        /* We only care about our particular observer */
        if (pointer != dependency_observe)
          return;
        /* Our observer has been detached from the module, so release the allocation that held this observer */
        api->unwant(api, &builtin->builtins->dependency, &builtin->builtins_want);
        return;

        default:
        return;
      }
  }

static void builtin_shutdown(struct api_dependency * api, struct dependency * dependency, struct dependency_observe * dependency_observe, enum apivalue_dependency type, void * pointer)
  {
    struct builtins * builtins;

    (void) api;
    (void) dependency;

    switch (type)
      {
        case apivalue_dependency_observer_departed:
        /* We only care about our particular observer */
        if (pointer != dependency_observe)
          return;
        /* We have special knowledge that 'builtins' is at the front of the allocation to be freed */
        builtins = type_with_member_at_ptr(struct builtins, cleanup, dependency_observe);
        (void) builtins->top->api_stdio->fprintf(builtins->top->api_stdio, stderr, "All built-in modules have been released\n");
        builtins->top->api_stdlib->free(builtins->top->api_stdlib, builtins);
        return;

        default:
        return;
      }
  }

int builtin_startup(struct top * top)
  {
    enum apivalue_dependency dependency_rv;
    size_t i;
    size_t j;
    union module * module;
    char * mem;
    int need_free;
    int rv;
    struct dependency_want temp_want;
    struct usage
      {
        struct builtins inner;
        struct builtin alignment[1];
      }
      * usage;

    if (builtin_modules_count == 0)
      return EXIT_SUCCESS;

    need_free = 0;
    top->api_dependency->initialize_want(top->api_dependency, &temp_want);

    mem = top->api_stdlib->malloc(top->api_stdlib, sizeof *usage + ((builtin_modules_count - 1) * sizeof usage->alignment));
    if (mem == NULL)
      {
        (void) top->api_stdio->fprintf(top->api_stdio, stderr, "Out of memory while allocating built-in modules\n");
        goto err_usage;
      }
    need_free = 1;
    usage = (void *) mem;
    usage->inner.builtins = (void *) (mem + offsetof(struct usage, alignment));
    usage->inner.top = top;
    top->api_dependency->initialize(top->api_dependency, &usage->inner.dependency);
    top->api_dependency->initialize_observe(top->api_dependency, &usage->inner.cleanup, &builtin_shutdown);
    dependency_rv = top->api_dependency->want(top->api_dependency, &usage->inner.dependency, &temp_want);
    if (dependency_rv != apivalue_dependency_success)
      {
        (void) top->api_stdio->fprintf(top->api_stdio, stderr, "Dependency API 'want' unexpectedly failed for built-in modules\n");
        goto err_want;
      }
    dependency_rv = top->api_dependency->observe(top->api_dependency, &usage->inner.dependency, &usage->inner.cleanup);
    if (dependency_rv != apivalue_dependency_success)
      {
        (void) top->api_stdio->fprintf(top->api_stdio, stderr, "Dependency API 'observe' unexpectedly failed for built-in modules\n");
        goto err_observe;
      }
    /* The clean-up is now responsible for freeing */
    need_free = 0;
    /* Failure-free pass */
    for (i = 0; i < builtin_modules_count; ++i)
      {
        usage->inner.builtins[i].live_module = NULL;
        top->api_dependency->initialize_observe(top->api_dependency, &usage->inner.builtins[i].cleanup, &builtin_cleanup);
        top->api_dependency->initialize_want(top->api_dependency, &usage->inner.builtins[i].builtins_want);
        top->api_dependency->initialize_want(top->api_dependency, &usage->inner.builtins[i].module_want);
        usage->inner.builtins[i].builtins = &usage->inner;
        usage->inner.builtins[i].top = top;
      }
    /* Can fail */
    for (i = 0; i < builtin_modules_count; ++i)
      {
        dependency_rv = top->api_dependency->want(top->api_dependency, &usage->inner.dependency, &usage->inner.builtins[i].builtins_want);
        if (dependency_rv != apivalue_dependency_success)
          {
            (void) top->api_stdio->fprintf(top->api_stdio, stderr, "Dependency API 'want' unexpectedly failed for a built-in module\n");
            goto err_builtins_want;
          }
      }
    for (i = 0; i < builtin_modules_count; ++i)
      {
        module = builtin_modules[i];
        rv = top->module_api->live_module_from_module(top, module, &usage->inner.builtins[i].module_want, &usage->inner.builtins[i].live_module);
        if (rv != EXIT_SUCCESS)
          {
            (void) top->api_stdio->fprintf(top->api_stdio, stderr, "Failed to create live module for a built-in module\n");
            goto err_live_module_want;
          }
      }
    for (i = 0; i < builtin_modules_count; ++i)
      {
        module = builtin_modules[i];
        dependency_rv = top->api_dependency->observe(top->api_dependency, &usage->inner.builtins[i].live_module->dependency, &usage->inner.builtins[i].cleanup);
        if (dependency_rv != apivalue_dependency_success)
          {
            (void) top->api_stdio->fprintf(top->api_stdio, stderr, "Dependency API 'observe' unexpectedly failed for a built-in module\n");
            goto err_live_module_observe;
          }
      }
    for (i = 0; i < builtin_modules_count; ++i)
      {
        rv = top->module_api->load(top, usage->inner.builtins[i].live_module);
        if (rv != EXIT_SUCCESS)
          {
            (void) top->api_stdio->fprintf(top->api_stdio, stderr, "A built-in module failed to load\n");
            goto err_load;
          }
      }
    /* The built-ins should now have "wants" for the allocated dependency, so release the original */
    top->api_dependency->unwant(top->api_dependency, &usage->inner.dependency, &temp_want);
    return EXIT_SUCCESS;

    i = builtin_modules_count;
    err_load:
    for (j = 0; j < i; ++j)
      top->module_api->unload(top, usage->inner.builtins[i].live_module);

    i = builtin_modules_count;
    err_live_module_observe:
    for (j = 0; j < i; ++j)
      top->api_dependency->unobserve(top->api_dependency, &usage->inner.builtins[i].live_module->dependency, &usage->inner.builtins[i].cleanup);

    i = builtin_modules_count;
    err_live_module_want:
    for (j = 0; j < i; ++j)
      top->api_dependency->unwant(top->api_dependency, &usage->inner.builtins[i].live_module->dependency, &usage->inner.builtins[i].module_want);

    i = builtin_modules_count;
    err_builtins_want:
    for (j = 0; j < i; ++j)
      top->api_dependency->unwant(top->api_dependency, &usage->inner.dependency, &usage->inner.builtins[i].builtins_want);

    i = builtin_modules_count;
    /* Our final observer frees itself */
    err_observe:

    top->api_dependency->unwant(top->api_dependency, &usage->inner.dependency, &temp_want);
    err_want:

    if (need_free)
      top->api_stdlib->free(top->api_stdlib, usage);
    err_usage:

    top->request_shutdown(top);
    return EXIT_FAILURE;
  }
