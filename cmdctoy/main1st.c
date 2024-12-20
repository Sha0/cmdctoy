/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#include <stdlib.h>
#include "toydef.h"
#include "main.h"
#include "stage2.h"

enum
  {
    main1st_signature = 42,
    main1st_zero = 0
  };

static func_main_work first_work_func;
static func_main_work not_ready_func;

static struct main_work not_ready;

struct first_work
  {
    struct main_work work;
    int sig;
  };

static struct first_work first_work =
  {
    {
      &first_work_func
    },
    main1st_signature
  };

struct main_work * main_first_work = &first_work.work;

static struct main_work not_ready =
  {
    &not_ready_func
  };

static void first_work_func(struct main_stack * main_stack)
  {
    struct first_work * container;

    /* Assume failure until proven otherwise */
    not_ready_func(main_stack);

    /* Sanity-check */
    container = type_with_member_at_ptr(struct first_work, work, *(main_stack->nonstandard.first_work));
    if (container->sig != main1st_signature)
      return;

    /* Subsequent calls to main should fail until such a time as everything's ready */
    *(main_stack->nonstandard.first_work) = &not_ready;

    main_stack->nonstandard.work = stage2_work;
    main_stack->nonstandard.loop = 1;
  }

static void not_ready_func(struct main_stack * main_stack)
  {
    main_stack->semistandard.return_value = EXIT_FAILURE;
    main_stack->nonstandard.loop = 0;
  }
