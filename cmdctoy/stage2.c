/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#include <stdlib.h>
#include "depend.h"
#include "toy.h"
#include "toyio.h"
#include "toylib.h"
#include "list.h"
#include "main.h"
#include "process.h"
#include "stage2.h"

static func_main_work stage2_func;

static struct main_work stage2;

struct main_work * stage2_work = &stage2;

static struct main_work stage2 =
  {
    &stage2_func
  };

static void stage2_func(struct main_stack * main_stack)
  {
    struct api_btree btree_api;
    enum apivalue_btree btree_rv;
    struct api_dependency dependency_api;
    enum apivalue_dependency dependency_rv;
    struct api_list list_api;
    enum apivalue_list list_rv;
    struct process * process;
    struct api_process process_api;
    enum apivalue_process process_rv;
    int return_value;
    struct api_stdio stdio_api;
    struct api_stdlib stdlib_api;
    enum apivalue_stdio stdio_rv;
    enum apivalue_stdlib stdlib_rv;

    /* Assume no further iterations in main, unless something changes, later on */
    main_stack->nonstandard.loop = 0;

    /* Populate APIs */
    stdio_rv = api_stdio_initialize(&stdio_api);
    if (stdio_rv != apivalue_stdio_success)
      {
        /* Need to use implementation's fprintf */
        (void) fprintf(stderr, "Failed to initialize stdio API\n");
        main_stack->semistandard.return_value = EXIT_FAILURE;
        return;
      }
    btree_rv = api_btree_initialize(&btree_api);
    if (btree_rv != apivalue_btree_success)
      {
        (void) stdio_api.fprintf(&stdio_api, stderr, "Failed to initialize binary tree API\n");
        main_stack->semistandard.return_value = EXIT_FAILURE;
        return;
      }
    list_rv = api_list_initialize(&list_api);
    if (list_rv != apivalue_list_success)
      {
        (void) stdio_api.fprintf(&stdio_api, stderr, "Failed to initialize list API\n");
        main_stack->semistandard.return_value = EXIT_FAILURE;
        return;
      }
    dependency_api.api_list = &list_api;
    dependency_rv = api_dependency_initialize(&dependency_api);
    if (dependency_rv != apivalue_dependency_success)
      {
        (void) stdio_api.fprintf(&stdio_api, stderr, "Failed to initialize dependency API\n");
        main_stack->semistandard.return_value = EXIT_FAILURE;
        return;
      }
    stdlib_rv = api_stdlib_initialize(&stdlib_api);
    if (stdlib_rv != apivalue_stdlib_success)
      {
        (void) stdio_api.fprintf(&stdio_api, stderr, "Failed to initialize stdlib API\n");
        main_stack->semistandard.return_value = EXIT_FAILURE;
        return;
      }
    process_api.api_btree = &btree_api;
    process_api.api_dependency = &dependency_api;
    process_api.api_list = &list_api;
    process_api.api_stdlib = &stdlib_api;
    process_rv = api_process_initialize(&process_api);
    if (process_rv != apivalue_process_success)
      {
        (void) stdio_api.fprintf(&stdio_api, stderr, "Failed to initialize process API\n");
        main_stack->semistandard.return_value = EXIT_FAILURE;
        return;
      }

    /* Initialize the process */
    process_rv = process_api.initialize(&process_api, &process, main_stack);
    if (process_rv != apivalue_process_success)
      {
        (void) stdio_api.fprintf(&stdio_api, stderr, "Failed to initialize process\n");
        main_stack->semistandard.return_value = EXIT_FAILURE;
        return;
      }

    /* TODO: send a thread to main */
    return_value = toy_loop(process);
    main_stack->semistandard.return_value = return_value;
  }
