/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#ifndef INC_PROCESS
#define INC_PROCESS

#include "btree.h"
#include "depend.h"
#include "toylib.h"
#include "list.h"
#include "main.h"

enum apivalue_process
  {
    apivalue_process_success,
    apivalue_process_error_null_argument,
    apivalue_process_error_out_of_memory,
    apivalue_process_error_btree_api,
    apivalue_process_error_dependency_api,
    apivalue_process_error_list_api,
    apivalue_process_error_stdlib_api,
    apivalue_process_zero = 0
  };

struct api_process;
struct process;

typedef enum apivalue_process apifunction_process_api_initialize(struct api_process *);
typedef enum apivalue_process apifunction_process_initialize_process(struct api_process *, struct process **, struct main_stack *);

extern apifunction_process_api_initialize api_process_initialize;

struct api_process
  {
    struct api_btree * api_btree;
    struct api_dependency * api_dependency;
    struct api_list * api_list;
    struct api_stdlib * api_stdlib;
    apifunction_process_api_initialize * api_initialize;
    apifunction_process_initialize_process * initialize;
  };

struct process
  {
    struct dependency dependency;
    struct dependency_observe observe;
    struct dependency_provide provide;
    struct dependency_ready ready;
    struct dependency_want want;
    struct api_btree api_btree;
    struct api_dependency api_dependency;
    struct api_list api_list;
    struct api_stdlib api_stdlib;
    struct main_stack * main_stack;
  };

#endif /* INC_PROCESS */
