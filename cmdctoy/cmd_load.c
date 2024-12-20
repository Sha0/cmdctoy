/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
/* TODO: Support non-POSIX */
#if CMDCTOY_POSIX
#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "builtins.h"
#include "command.h"
#include "depend.h"
#include "toy.h"
#include "toydef.h"
#include "toyio.h"
#include "toylib.h"
#include "modpriv.h"
#include "module.h"

struct cmd_load;
struct module_observer;

struct cmd_load
  {
    struct command command;
    struct top * ctx;
  };

struct module_observer
  {
    struct dependency_observe dependency_observe;
    struct dependency_want dependency_want;
    /* TODO: cmd_load_want should eventually become a need */
    struct dependency_want cmd_load_want;
    struct work_item work_item;
    struct top * ctx;
    void * dl_handle;
    struct live_module * live_module;
  };

static apifunction_command cmd_list_modules;
static apifunction_command cmd_load;
static apifunction_command cmd_unload;
static void help(struct top *);
static apifunction_dependency_observe_call module_observe;
static func_module_event module_event;
static func_work module_unloaded;

static struct command command_list_modules;
static struct command command_load;
static struct command command_unload;
static struct live_module * live_module;

#if BUILTIN_CMD_LOAD
union module builtin_module_cmd_load =
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
      "cmd_load"
    }
  };

static struct command command_list_modules =
  {
    NULL,
    "list_modules",
    &cmd_list_modules,
    {
      NULL,
      NULL
    }
  };

static struct command command_load =
  {
    NULL,
    "load",
    &cmd_load,
    {
      NULL,
      NULL
    }
  };

static struct command command_unload =
  {
    NULL,
    "unload",
    &cmd_unload,
    {
      NULL,
      NULL
    }
  };

static int cmd_list_modules(struct api_command * api, struct command * command, int argc, char ** argv)
  {
    struct cmd_load * cmd;
    struct top * ctx;
    struct list_item * list_item;
    struct module_private * module_private;
    char serial[sizeof module_private->live_module.module.v1.required.module_serial + 1];

    (void) api;
    (void) argv;

    cmd = type_with_member_at_ptr(struct cmd_load, command, command);
    ctx = cmd->ctx;

    if (argc != 1)
      {
        help(ctx);
        return EXIT_FAILURE;
      }
    serial[sizeof serial - 1] = '\0';
    for (list_item = ctx->module_api->module_list->head.next; list_item != &ctx->module_api->module_list->head; list_item = list_item->next)
      {
        module_private = type_with_member_at_ptr(struct module_private, list_item, list_item);
        memcpy(serial, module_private->live_module.module.v1.required.module_serial, sizeof module_private->live_module.module.v1.required.module_serial);
        (void) ctx->api_stdio->fprintf(ctx->api_stdio, stdout, "Module #%lu with serial '%s' is named '%s'\n", module_private->order, serial, module_private->live_module.module.v1.nice_name);
      }
    return EXIT_SUCCESS;
  }

static int cmd_load(struct api_command * api, struct command * command, int argc, char ** argv)
  {
    struct cmd_load * cmd;
    struct top * ctx;
    enum apivalue_dependency dependency_rv;
    void * dl_handle;
    void * dl_sym;
    int need_cmd_load_unwant;
    int need_dlclose;
    int need_free;
    int new_errno;
    int old_errno;
    int rv;
    struct module_observer * module_cleanup;

    (void) api;

    cmd = type_with_member_at_ptr(struct cmd_load, command, command);
    ctx = cmd->ctx;

    if (argc != 2)
      {
        help(ctx);
        return EXIT_FAILURE;
      }

    /* Assume failure until proven otherwise */
    rv = EXIT_FAILURE;

    need_free = 0;
    module_cleanup = ctx->api_stdlib->malloc(ctx->api_stdlib, sizeof *module_cleanup);
    if (module_cleanup == NULL)
      {
        (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Out of memory while allocating module clean-up\n");
        goto err_module_cleanup;
      }
    need_free = 1;
    module_cleanup->ctx = ctx;
    ctx->api_dependency->initialize_observe(ctx->api_dependency, &module_cleanup->dependency_observe, &module_observe);
    ctx->api_dependency->initialize_want(ctx->api_dependency, &module_cleanup->dependency_want);
    ctx->api_dependency->initialize_want(ctx->api_dependency, &module_cleanup->cmd_load_want);
    module_cleanup->work_item.work = &module_unloaded;
    /* POSIX load */
    need_dlclose = 0;
    old_errno = errno;
    errno = 0;
    dl_handle = dlopen(argv[1], RTLD_NOW);
    new_errno = errno;
    errno = old_errno;
    if (dl_handle == NULL)
      {
        (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Failed to load module with reason '%s' and errno was '%d'\n", dlerror(), new_errno);
        goto err_dl_handle;
      }
    need_dlclose = 1;
    /* Note handle in closer */
    module_cleanup->dl_handle = dl_handle;
    /* Find module */
    old_errno = errno;
    errno = 0;
    dl_sym = dlsym(dl_handle, "module");
    new_errno = errno;
    errno = old_errno;
    if (dl_sym == NULL)
      {
        (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Failed to find module object with reason '%s' and errno was '%d'\n", dlerror(), new_errno);
        goto err_dl_sym;
      }
    /* Create/get a live module from the module */
    rv = ctx->module_api->live_module_from_module(ctx, dl_sym, &module_cleanup->dependency_want, &module_cleanup->live_module);
    if (rv != EXIT_SUCCESS)
      {
        (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Unable to create/get live module from module\n");
        goto err_live_module;
      }
    /* Pin the cmd_load module until the observer has performed clean-up */
    /* TODO: Should eventually become a "need" */
    need_cmd_load_unwant = 1;
    dependency_rv = ctx->api_dependency->want(ctx->api_dependency, &live_module->dependency, &module_cleanup->cmd_load_want);
    if (dependency_rv != apivalue_dependency_success)
      {
        (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Unable to associate shut-down of live module with cmd_load module\n");
        goto err_cmd_load_want;
      }
    /* Attach observer */
    dependency_rv = ctx->api_dependency->observe(ctx->api_dependency, &module_cleanup->live_module->dependency, &module_cleanup->dependency_observe);
    if (dependency_rv != apivalue_dependency_success)
      {
        (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Unable to provide proper shut-down for live module\n");
        goto err_dependency_observe;
      }
    /* The observer is now responsible for freeing the clean-up and for 'dlclose' */
    need_cmd_load_unwant = 0;
    need_dlclose = 0;
    need_free = 0;
    /* Issue load-request */
    rv = ctx->module_api->load(ctx, module_cleanup->live_module);
    if (rv != EXIT_SUCCESS)
      {
        (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Failed to load module with error '%d'\n", rv);
        goto err_load;
      }
    /* Grant the thread context to the module */
    rv = ctx->module_api->thread_started(ctx, module_cleanup->live_module);
    if (rv != EXIT_SUCCESS)
      {
        (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Failed to grant thread context to loaded module with error '%d'\n", rv);
        goto err_thread_started;
      }
    (void) ctx->api_stdio->fprintf(ctx->api_stdio, stdout, "Module '%s' loaded\n", module_cleanup->live_module->module.v1.nice_name);
    return EXIT_SUCCESS;

    err_thread_started:

    (void) ctx->module_api->unload(ctx, module_cleanup->live_module);
    err_load:

    /* The observer will free itself */
    err_dependency_observe:

    if (need_cmd_load_unwant)
      ctx->api_dependency->unwant(ctx->api_dependency, &live_module->dependency, &module_cleanup->cmd_load_want);
    err_cmd_load_want:

    ctx->api_dependency->unwant(ctx->api_dependency, &module_cleanup->live_module->dependency, &module_cleanup->dependency_want);
    err_live_module:

    err_dl_sym:

    if (need_dlclose)
      dlclose(dl_handle);
    err_dl_handle:

    if (need_free)
      ctx->api_stdlib->free(ctx->api_stdlib, module_cleanup);
    err_module_cleanup:

    return rv;
  }

static int cmd_unload(struct api_command * api, struct command * command, int argc, char ** argv)
  {
    struct cmd_load * cmd;
    struct top * ctx;
    char * endptr;
    struct list_item * list_item;
    struct module_private * module_private;
    int new_errno;
    int old_errno;
    unsigned long int order;

    (void) api;

    cmd = type_with_member_at_ptr(struct cmd_load, command, command);
    ctx = cmd->ctx;

    if (argc != 2)
      {
        help(ctx);
        return EXIT_FAILURE;
      }

    /* Find the module */
    old_errno = errno;
    errno = 0;
    order = strtoul(argv[1], &endptr, 0);
    new_errno = errno;
    errno = old_errno;
    if (new_errno != 0)
      {
        (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "While processing module-number, error '%d' was encountered with message '%s'\n", new_errno, strerror(new_errno));
        return new_errno;
      }
    if (*endptr != '\0')
      {
        (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Specified module-number does not appear to be an 'unsigned long int'\n");
        return EXIT_FAILURE;
      }

    for (list_item = ctx->module_api->module_list->head.next; list_item != &ctx->module_api->module_list->head; list_item = list_item->next)
      {
        module_private = type_with_member_at_ptr(struct module_private, list_item, list_item);
        if (module_private->order == order)
          break;
      }
    if (list_item == &ctx->module_api->module_list->head)
      {
        (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Could not find loaded module #%lu\n", order);
        return EXIT_FAILURE;
      }
    /* Found it */
    /* TODO: thread-stop before unload */
    ctx->module_api->unload(ctx, &module_private->live_module);
    (void) ctx->api_stdio->fprintf(ctx->api_stdio, stdout, "Requested module to be unloaded\n");
    return EXIT_SUCCESS;
  }

static void help(struct top * ctx)
  {
    static const char usage[] =
      "Usage:\n"
      "  list_modules         List loaded modules\n"
      "  load FILENAME        Load a module\n"
      "  unload MODULENUMBER  Unload a module\n"
      ;
    (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "%s", usage);
  }

static void module_observe(struct api_dependency * api, struct dependency * dependency, struct dependency_observe * dependency_observe, enum apivalue_dependency type, void * pointer)
  {
    struct module_observer * module_cleanup;
    int rv;

    (void) dependency;

    switch (type)
      {
        case apivalue_dependency_error_going_away:
        /* Remove want */
        module_cleanup = type_with_member_at_ptr(struct module_observer, dependency_observe, dependency_observe);
        api->unwant(api, &module_cleanup->live_module->dependency, &module_cleanup->dependency_want);
        return;

        case apivalue_dependency_observer_departed:
        /* We only care about our own observer */
        if (pointer != dependency_observe)
          return;
        /* Schedule the work-item to 'dlclose' */
        module_cleanup = type_with_member_at_ptr(struct module_observer, dependency_observe, dependency_observe);
        rv = module_cleanup->ctx->schedule_last(live_module, &module_cleanup->work_item, module_cleanup->ctx->work_list);
        if (rv != EXIT_SUCCESS)
          {
            (void) module_cleanup->ctx->api_stdio->fprintf(module_cleanup->ctx->api_stdio, stderr, "Unable to clean up loaded module, so it's been leaked\n");
            return;
          }

        default:
        return;
      }
  }

static int module_event(enum apivalue_module_event_type type, void * event_data)
  {
    struct cmd_load (* commands)[3];
    struct top * ctx;
    size_t i;
    size_t j;
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
        commands = ctx->api_stdlib->malloc(ctx->api_stdlib, sizeof *commands);
        if (commands == NULL)
          {
            (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Out of memory while registering 'load', 'list_modules', 'unload' commands\n");
            return EXIT_FAILURE;
          }
        live_module->module.v1.module_pointers[0] = commands;
        (*commands)[0].command = command_list_modules;
        (*commands)[0].ctx = ctx;
        (*commands)[1].command = command_load;
        (*commands)[1].ctx = ctx;
        (*commands)[2].command = command_unload;
        (*commands)[2].ctx = ctx;
        for (i = 0; i < countof(*commands); ++i)
          {
            (*commands)[i].command.live_module = live_module;
            ctx->api_list->initialize_list_item(ctx->api_list, &(*commands)[i].command.list_item);
            rv = ctx->api_command->add(ctx->api_command, &(*commands)[i].command);
            if (rv != EXIT_SUCCESS)
              {
                (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Error '%d' while attempting to register '%s' command\n", rv, (*commands)[i].command.name);
                for (j = 0; j < i; ++j)
                  (void) ctx->api_command->remove(ctx->api_command, &(*commands)[i].command);
                return rv;
              }
          }
        return rv;

        case apivalue_module_event_type_thread_stop_requested:
        case apivalue_module_event_type_thread_stopped:
        case apivalue_module_event_type_unload_requested:
        return EXIT_SUCCESS;

        case apivalue_module_event_type_unload:
        /* TODO: This clean-up belongs to thread-stopped, but that's not yet implemented */
        commands = live_module->module.v1.module_pointers[0];
        if (commands == NULL)
          return EXIT_SUCCESS;
        /* Just take from the first command */
        ctx = (*commands)[0].ctx;
        for (i = 0; i < countof(*commands); ++i)
          {
            rv = ctx->api_command->remove(ctx->api_command, &(*commands)[i].command);
            if (rv != EXIT_SUCCESS)
              {
                (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "Error '%d' while attempting to deregister '%s' command\n", rv, (*commands)[i].command.name);
                /* Oh well.  Leak */
                return rv;
              }
          }
        ctx->api_stdlib->free(ctx->api_stdlib, commands);
        live_module->module.v1.module_pointers[0] = NULL;
        live_module = NULL;
        return rv;
      }
    return EXIT_FAILURE;
  }

static int module_unloaded(struct work_item * work_item)
  {
    struct top * ctx;
    void * dl_handle;
    struct module_observer * module_cleanup;
    int new_errno;
    int old_errno;
    int rv;
    struct dependency_want temp_want;

    module_cleanup = type_with_member_at_ptr(struct module_observer, work_item, work_item);
    /* Move the cmd_load "want" from the allocation */
    ctx = module_cleanup->ctx;
    ctx->api_dependency->initialize_want(ctx->api_dependency, &temp_want);
    ctx->api_dependency->swap_want(ctx->api_dependency, &live_module->dependency, &module_cleanup->cmd_load_want, &temp_want);
    /* POSIX */
    dl_handle = module_cleanup->dl_handle;
    old_errno = errno;
    errno = 0;
    rv = dlclose(dl_handle);
    new_errno = errno;
    errno = old_errno;
    if (rv != 0 || new_errno != 0)
      {
        (void) ctx->api_stdio->fprintf(ctx->api_stdio, stderr, "A module was unloaded, but 'dlclose' failed with return-value '%d' and errno '%d' with message '%s'\n", rv, new_errno, strerror(new_errno));
        rv = EXIT_FAILURE;
      }
      else
      {
        rv = EXIT_SUCCESS;
      }
    /* Allocated in cmd_load */
    ctx->api_stdlib->free(ctx->api_stdlib, module_cleanup);
    /* Release the cmd_load module */
    ctx->api_dependency->unwant(ctx->api_dependency, &live_module->dependency, &temp_want);
    return rv;
  }

#endif /* CMDCTOY_POSIX */
