/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#ifndef INC_CMDCTOY
#define INC_CMDCTOY

struct top;
struct work_item;
struct work_list;

#include "depend.h"
#include "list.h"
#include "main.h"
#include "module.h"
#include "process.h"

typedef int func_toy_loop(struct process *);
typedef void func_request_shutdown(struct top *);
typedef int func_schedule_last(struct live_module *, struct work_item *, struct work_list *);
typedef int func_schedule_next(struct live_module *, struct work_item *, struct work_list *);
typedef int func_work(struct work_item *);

extern func_toy_loop toy_loop;

struct top
  {
    struct main_stack * main_stack;
    struct api_btree * api_btree;
    struct api_dependency * api_dependency;
    struct module_api * module_api;
    struct api_command * api_command;
    struct live_module * work_module;
    struct api_stdio * api_stdio;
    struct api_stdlib * api_stdlib;
    struct api_toy_scope * api_toy_scope;
    struct api_type * api_type;
    struct api_list * api_list;
    struct work_list * work_list;
    func_schedule_last * schedule_last;
    func_schedule_next * schedule_next;
    func_request_shutdown * request_shutdown;
    int * shutdown_requested;
  };

struct work_item
  {
    func_work * work;
    struct list_item list_item;
    struct top * ctx;
    struct live_module * live_module;
  };

struct work_list
  {
    struct list list;
  };

#endif /* INC_CMDCTOY */
