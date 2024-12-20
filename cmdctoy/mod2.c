/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#include <stdlib.h>
#include "builtins.h"
#include "toy.h"
#include "module.h"

static func_module_event module_event;

#if BUILTIN_MOD2
union module builtin_module_mod2 =
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
      "mod2"
    }
  };

static int module_event(enum apivalue_module_event_type type, void * event_data)
  {
    (void) type;
    (void) event_data;
    return EXIT_SUCCESS;
  }
