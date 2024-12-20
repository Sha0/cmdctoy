/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#ifndef INC_MODPRIV
#define INC_MODPRIV

#include "depend.h"
#include "list.h"
#include "module.h"

struct module_private
  {
    struct top ctx;
    struct work_item * closer;
    struct list_item list_item;
    struct dependency_want dependency_want;
    struct dependency_observe dependency_observe;
    unsigned long int order;
    struct live_module live_module;
  };

#endif /* INC_MODPRIV */
